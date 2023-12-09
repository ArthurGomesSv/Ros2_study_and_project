#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/int32.hpp"
#include "std_msgs/msg/float64.hpp"

class HolderNode : public rclcpp::Node
{
public:
    HolderNode() : Node("holder_ball_robot")
    {
        pwm_.data = 0;
        ball_heigh_.data = 0;
        ball_heigh_ref_.data = 0;

        pwm_subscriber_ = this->create_subscription<std_msgs::msg::Int32>("control/pwm",
                                                            10,std::bind(&HolderNode::callback_holder_pwm,this,std::placeholders::_1));
        pwm_publisher_ = this->create_publisher<std_msgs::msg::Int32>("esp/holder_pwm",10);

        ball_height_subscriber_ = this->create_subscription<std_msgs::msg::Float64>("camera/ball_height_traking",
                                                            10,std::bind(&HolderNode::callback_holder_ball_height,this,std::placeholders::_1));
        ball_height_publisher_ = this->create_publisher<std_msgs::msg::Float64>("communication/holder_ball_height_traking",10);

        ball_heigh_ref_subscriber_ = this->create_subscription<std_msgs::msg::Float64>("communication/inter_graph_ball_height_ref",
                                                            10,std::bind(&HolderNode::callback_holder_ball_height_ref,this,std::placeholders::_1));
        ball_heigh_ref_publisher_ = this->create_publisher<std_msgs::msg::Float64>("communication/holder_ball_height_ref",10);

        RCLCPP_INFO(this->get_logger(),"Holder Node has been started");
    }

private:
    std_msgs::msg::Int32 pwm_;
    std_msgs::msg::Float64 ball_heigh_;
    std_msgs::msg::Float64 ball_heigh_ref_;

    rclcpp::Subscription<std_msgs::msg::Int32>::SharedPtr pwm_subscriber_;
    rclcpp::Subscription<std_msgs::msg::Float64>::SharedPtr ball_height_subscriber_;
    rclcpp::Subscription<std_msgs::msg::Float64>::SharedPtr ball_heigh_ref_subscriber_;

    rclcpp::Publisher<std_msgs::msg::Int32>::SharedPtr pwm_publisher_;
    rclcpp::Publisher<std_msgs::msg::Float64>::SharedPtr ball_height_publisher_;
    rclcpp::Publisher<std_msgs::msg::Float64>::SharedPtr ball_heigh_ref_publisher_;

    void callback_holder_pwm(const std_msgs::msg::Int32::SharedPtr msg)
    {
        pwm_.data = msg->data;
        publisher_pwm();
    }

    void callback_holder_ball_height(const std_msgs::msg::Float64::SharedPtr msg)
    {
        ball_heigh_.data = msg->data;
        publisher_ball_height();
    }

    void callback_holder_ball_height_ref(const std_msgs::msg::Float64::SharedPtr msg)
    {
        ball_heigh_ref_.data = msg->data;
        publisher_ball_height_ref();
    }

    void publisher_pwm()
    {
        pwm_publisher_->publish(pwm_);
    }

    void publisher_ball_height()
    {
        ball_height_publisher_->publish(ball_heigh_);
    }

    void publisher_ball_height_ref()
    {
        ball_heigh_ref_publisher_->publish(ball_heigh_ref_);
    }

};

int main(int argc, char **argv)
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<HolderNode>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}