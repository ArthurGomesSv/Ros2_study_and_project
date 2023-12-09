#include "rclcpp/rclcpp.hpp"
#include "example_interfaces/srv/add_two_ints.hpp"

class AddTwoIntsServeNode : public rclcpp::Node
{
public:
    AddTwoIntsServeNode() : Node("add_two_ints_server_no_oop")
    {
        server_ = this->create_service<example_interfaces::srv::AddTwoInts>("add_two_ints",std::bind(&AddTwoIntsServeNode::callback_add_two_ints,this,
                                        std::placeholders::_1,std::placeholders::_2));
        RCLCPP_INFO(this->get_logger(),"Add two ints server has been started");        
    }

private:
    void callback_add_two_ints(const example_interfaces::srv::AddTwoInts::Request::SharedPtr request,
                                const example_interfaces::srv::AddTwoInts::Response::SharedPtr response)
        {
            response->sum = request->a + request->b;
            RCLCPP_INFO(this->get_logger(),"%d + %d = %d",request->a,request->b,response->sum);
        }
    rclcpp::Service<example_interfaces::srv::AddTwoInts>::SharedPtr server_;
};

int main(int argc, char **argv)
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<AddTwoIntsServeNode>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}