#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/int64.hpp"
#include "example_interfaces/srv/set_bool.hpp"

class NumberCounterNode : public rclcpp::Node
{
public:
    NumberCounterNode() : Node("number_counter")
    {
        counter_.data = 0;
        subscriber_ = this->create_subscription<std_msgs::msg::Int64>("number",10,std::bind(&NumberCounterNode::callback_number_counter,this,std::placeholders::_1));
        publisher_ = this->create_publisher<std_msgs::msg::Int64>("number_counter",10);
        RCLCPP_INFO(this->get_logger(),"Counter Node has been started");

        server_ = this->create_service<example_interfaces::srv::SetBool>("reset_number_count",std::bind(&NumberCounterNode::callback_reset_service,this,
                                                                        std::placeholders::_1,std::placeholders::_2));
        RCLCPP_INFO(this->get_logger(),"Reset server has been started");
    }

private:
    std_msgs::msg::Int64 counter_;

    rclcpp::Subscription<std_msgs::msg::Int64>::SharedPtr subscriber_;
    rclcpp::Publisher<std_msgs::msg::Int64>::SharedPtr publisher_;
    rclcpp::Service<example_interfaces::srv::SetBool>::SharedPtr server_;

    void callback_number_counter(const std_msgs::msg::Int64::SharedPtr msg)
    {
        counter_.data += msg->data;
        publisher_counter();
    }

    void publisher_counter()
    {
        publisher_->publish(counter_);
    }

    void callback_reset_service(const example_interfaces::srv::SetBool::Request::SharedPtr request,
                                const example_interfaces::srv::SetBool::Response::SharedPtr response)
    {
        if (request->data)
        {
            this->counter_.data = 0;
            response->success = true;
            RCLCPP_INFO(this->get_logger(),"Counter has been reseted");
        } else
        {
            RCLCPP_INFO(this->get_logger(),"Counter reset service access denied");
            response->success = false;
        } 
    }

};

int main(int argc, char **argv)
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<NumberCounterNode>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}