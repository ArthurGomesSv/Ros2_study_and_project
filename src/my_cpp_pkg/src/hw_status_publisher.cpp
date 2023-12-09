#include "rclcpp/rclcpp.hpp"
#include "my_robot_interfaces/msg/hardware_status.hpp"

class HwStatusPublisherNode : public rclcpp::Node
{
public:
    HwStatusPublisherNode() : Node("hardware_status_publisher")
    {
        publisher_ = this->create_publisher<my_robot_interfaces::msg::HardwareStatus>("hardware_status",10);
        timer_= this->create_wall_timer(std::chrono::milliseconds(500), std::bind(&HwStatusPublisherNode::publisherHardwareStatus,this));
        RCLCPP_INFO(this->get_logger(),"Hardware status publisher has been started");
    }

private:
    void publisherHardwareStatus()
    {
        auto msg = my_robot_interfaces::msg::HardwareStatus();
        msg.temperature = 45;
        msg.are_motors_ready = true;
        msg.debug_message = "ok";
        publisher_->publish(msg);
    }

    rclcpp::Publisher<my_robot_interfaces::msg::HardwareStatus>::SharedPtr publisher_;
    rclcpp::TimerBase::SharedPtr timer_;
};

int main(int argc, char **argv)
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<HwStatusPublisherNode>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}