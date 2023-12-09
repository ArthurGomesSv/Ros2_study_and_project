#include "rclcpp/rclcpp.hpp"
#include "my_robot_interfaces/srv/set_led.hpp"


class BatteryNode : public rclcpp::Node
{
public:
    BatteryNode() : Node("battery_node")
    {
        battery_state_ = true;
        RCLCPP_INFO(this->get_logger(), "Battery node has been started");
        RCLCPP_INFO(this->get_logger(), "Battery is full");
        timer_until_empty_ = this->create_wall_timer(std::chrono::seconds(4),std::bind(&BatteryNode::callback_battery_empty,this));
        timer_until_full_ = this->create_wall_timer(std::chrono::seconds(6),std::bind(&BatteryNode::callback_battery_full,this));
        timer_until_full_->cancel();
    }

    void call_set_led_service(int led_number, bool state)
    {
        auto client_ = this->create_client<my_robot_interfaces::srv::SetLed>("set_led");
        while (!client_->wait_for_service(std::chrono::seconds(1)))
        {
            RCLCPP_WARN(this->get_logger(),"Waiting for the server to be online...");
        }

        auto request = std::make_shared<my_robot_interfaces::srv::SetLed::Request>();
        request->led_number = led_number;
        request->state = state;

        auto future = client_->async_send_request(request);
        try
        {
            auto response = future.get();
            if (response->success)
            {
                if(state)
                {
                    RCLCPP_INFO(this->get_logger(),"Battery is charging");
                    timer_until_full_->reset();
                }else
                {
                    RCLCPP_INFO(this->get_logger(),"Battery is full");
                    timer_until_empty_->reset();
                }
            }else RCLCPP_INFO(this->get_logger(),"Set led fail");
        }
        catch(const std::exception& e)
        {
            RCLCPP_ERROR(this->get_logger(),"Service call failed");
        }
    }

private:
    bool battery_state_;

    rclcpp::TimerBase::SharedPtr timer_until_empty_;
    rclcpp::TimerBase::SharedPtr timer_until_full_;

    std::vector<std::thread> threads_;

    void callback_battery_empty()
    {
        battery_state_ = false;
        bool led_state_ = !battery_state_;
        timer_until_empty_->cancel();
        threads_.push_back(std::thread(std::bind(&BatteryNode::call_set_led_service,this,3,led_state_)));
    }

    void callback_battery_full()
    {
        battery_state_ = true;
        bool led_state_ = !battery_state_;
        timer_until_full_->cancel();
        threads_.push_back(std::thread(std::bind(&BatteryNode::call_set_led_service,this,3,led_state_)));
    }
};

int main(int argc, char **argv)
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<BatteryNode>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}