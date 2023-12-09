#include "rclcpp/rclcpp.hpp"
#include "my_robot_interfaces/msg/led_panel_state.hpp"
#include "my_robot_interfaces/srv/set_led.hpp"

class LedPanelNode : public rclcpp::Node
{
public:
    LedPanelNode() : Node("led_panel_node")
    {
        led_status_publisher_ = this->create_publisher<my_robot_interfaces::msg::LedPanelState>("led_panel_state",10);
        RCLCPP_INFO(this->get_logger(),"Led panel node has been started");
        server_ = this->create_service<my_robot_interfaces::srv::SetLed>("set_led",std::bind(&LedPanelNode::callback_set_led,this,
                                                                            std::placeholders::_1,
                                                                            std::placeholders::_2));
        RCLCPP_INFO(this->get_logger(),"Led panel service is online");
    }

private:
    rclcpp::Publisher<my_robot_interfaces::msg::LedPanelState>::SharedPtr led_status_publisher_;
    rclcpp::Service<my_robot_interfaces::srv::SetLed>::SharedPtr server_;
    int led_panel[3] = {0, 0, 0};

    void callback_set_led(const my_robot_interfaces::srv::SetLed::Request::SharedPtr request,
                            const my_robot_interfaces::srv::SetLed::Response::SharedPtr response)
                            {
                                if (request->state)
                                {
                                    led_panel[request->led_number-1] = 1;
                                    publisher_led_status(request->led_number,true);
                                    RCLCPP_INFO(this->get_logger(),"Led status has been updated");
                                }else
                                {
                                    led_panel[request->led_number-1] = 0;
                                    publisher_led_status(request->led_number,false);
                                    RCLCPP_INFO(this->get_logger(),"Led status has been updated");
                                }
                                response->success = true;
                            }

    void publisher_led_status(int led_number, bool led_status)
    {
        auto led_status_msg = my_robot_interfaces::msg::LedPanelState();
        led_status_msg.led_state = led_status;
        if (led_status)
        {
            led_status_msg.msg = std::string("Led de number") + std::to_string(led_number) + std::string(" is on") ;
        }else
        {
            led_status_msg.msg = std::string("Led de number") + std::to_string(led_number) + std::string(" is off") ;
        }
        
        led_status_publisher_->publish(led_status_msg);
    }
};

int main(int argc, char **argv)
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<LedPanelNode>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}