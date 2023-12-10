#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/int32.hpp"
#include <chrono>

class ControlNode : public rclcpp::Node
{
public:
    ControlNode() : Node("control_node")
    {
        //Create subscriber
        ball_heigh_subscriber_ = this->create_subscription<std_msgs::msg::Int32>("camera/ball_height_traking"
                                            ,10,std::bind(&ControlNode::ball_heigh_callback,this,std::placeholders::_1));
        ball_heigh_control_ref_subscriber_ = this->create_subscription<std_msgs::msg::Int32>("communication/holder_ball_height_ref"
                                            ,10,std::bind(&ControlNode::ball_heigh_ref_callback,this,std::placeholders::_1));
        //Create publisher
        pwm_publisher_ = this->create_publisher<std_msgs::msg::Int32>("control/pwm",10);

        //Create Param and get
        this->declare_parameter("KP",100);
        this->declare_parameter("KD",100);
        this->declare_parameter("KI",100);
        KP = this->get_parameter("KP").as_int();
        KD = this->get_parameter("KD").as_int();
        KI = this->get_parameter("KI").as_int();

        //Timers
        timer_to_up_ = this->create_wall_timer(std::chrono::seconds(5), std::bind(&ControlNode::timer_to_up_callback, this));
        timer_to_down_ = this->create_wall_timer(std::chrono::milliseconds(100), std::bind(&ControlNode::timer_to_down_callback, this));
        timer_to_down_->cancel();
        timer_to_calibration_ = this->create_wall_timer(std::chrono::milliseconds(500), std::bind(&ControlNode::timer_to_calibration_callback, this));
        timer_to_calibration_->cancel();

        //Inicio da calibração
        START = true;
        ball_pwm.data = 2000;
        pwm_publisher_->publish(ball_pwm);
        RCLCPP_INFO(this->get_logger(),"Inicio da calibração...");
    }

private:
    //Comunicação ROS
    rclcpp::Subscription<std_msgs::msg::Int32>::SharedPtr ball_heigh_subscriber_;
    rclcpp::Subscription<std_msgs::msg::Int32>::SharedPtr ball_heigh_control_ref_subscriber_;

    rclcpp::Publisher<std_msgs::msg::Int32>::SharedPtr pwm_publisher_;

    std_msgs::msg::Int32 ball_pwm;

    //Variável para o controle
    int ball_height;
    int ball_height_origin;
    int ball_height_control_ref = 0;
    int ball_height_error = 0;
    int ball_height_error_1 = 0;
    int PWM_max = 4095;

    std::chrono::time_point<std::chrono::system_clock> time_now, time_1;
    std::chrono::duration<double> T0;

    bool START;

    //Variáveis para calibração
    bool end_calibiration = false;
    int pwm_min_to_fly = 500;

    //Timers para calibração
    rclcpp::TimerBase::SharedPtr timer_to_up_;
    bool UP = false;
    rclcpp::TimerBase::SharedPtr timer_to_down_;
    bool DOWN = false;
    rclcpp::TimerBase::SharedPtr timer_to_calibration_;
    int number_height_to_calibration = 0;
    int total_number_height_to_calibration = 50;

    //Variável para o PID
    uint64_t sum_error = 0;

    int KP;
    int KD;
    int KI;

    void ball_heigh_callback(const std_msgs::msg::Int32::SharedPtr msg)
    {
        if(end_calibiration)
        {
            //Colocar a chamada da função de controle e publicar aqui!!
            ball_height = msg->data; //Recebe altura da bola em mm (mm)

            ball_height_error_1 = ball_height_error;
            ball_height_error = ball_height_control_ref-ball_height;
            sum_error +=ball_height_error;

            if(START)
            {
                time_1 = std::chrono::system_clock::now();
                START = false;
            }
            else
            {
                time_now = std::chrono::system_clock::now();
                T0 = time_now - time_1;

                ball_pwm.data = (int)(KP*ball_height_error + KI*T0.count()*sum_error + ((double)KD/T0.count())*(ball_height_error-ball_height_error_1));
                if (ball_pwm.data > PWM_max) ball_pwm.data = PWM_max;
                if (ball_pwm.data < pwm_min_to_fly) ball_pwm.data = pwm_min_to_fly;

                pwm_publisher_->publish(ball_pwm);
                time_1 = time_now;
                }
        }
        else
        {
            //Rotina de calibração da posição inicial
            if(UP && !DOWN)
            {
                if(timer_to_down_->is_canceled())timer_to_down_->reset();
            }
            if(UP && DOWN)
            {
                if(timer_to_calibration_->is_canceled())timer_to_calibration_->reset();
            }
        }
    }

    void timer_to_up_callback()
    {
        RCLCPP_INFO(this->get_logger(),"Bola descendo...");
        UP = true;
        timer_to_up_->cancel();
    }

    void timer_to_down_callback()
    {
        if(ball_pwm.data>pwm_min_to_fly)
        {
            ball_pwm.data -=10;
            pwm_publisher_->publish(ball_pwm);
        }
        else
        {
            DOWN = true;
            timer_to_down_->cancel();
            RCLCPP_INFO(this->get_logger(),"Determinando origem...");
        }
    }

    void timer_to_calibration_callback()
    {
        if(number_height_to_calibration<total_number_height_to_calibration)
        {
            ball_height_origin += ball_height;
            number_height_to_calibration++;
        }
        else
        {
            ball_height_origin = (int)((float)ball_height_origin)/((float)total_number_height_to_calibration);
            end_calibiration = true;
            timer_to_calibration_->cancel();
            RCLCPP_INFO(this->get_logger(),"Calibração finalizada...");
            RCLCPP_INFO(this->get_logger(),"Iniciando controle...");
        }
    }

    void ball_heigh_ref_callback(const std_msgs::msg::Int32::SharedPtr msg)
    {
        ball_height_control_ref = msg->data+ball_height_origin;
    }
};

int main(int argc, char **argv)
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<ControlNode>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}