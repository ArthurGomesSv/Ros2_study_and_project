#!/usr/bin/env python3
import rclpy
from rclpy.node import Node
from std_msgs.msg import Int64
from example_interfaces.srv import SetBool

class NumberPublisherNode(Node):

    def __init__(self):
        super().__init__("number_publisher")
        self.number_to_publish_ = 4
        self.publisher_ = self.create_publisher(Int64,"number",10)
        self.create_timer(1,self.number_callback)
        self.get_logger().info("Number publisher has been started")
        self.server_ = self.create_service(SetBool,"change_number_to_publish",self.change_number_callback)
        self.get_logger().info("Change number server has been started")

    def number_callback(self):
        msg = Int64()
        msg.data = self.number_to_publish_
        self.publisher_.publish(msg)
    
    def change_number_callback(self, request, response):
        if request.data:
            self.number_to_publish_ = 10
            response.success = True
            self.get_logger().info("Number to publish has been changed to 10")
        else:
            self.get_logger().info("Server number change access denied")
            response.success = False
        return response
        
        
def main(args=None):
    rclpy.init(args=args)
    node=NumberPublisherNode()
    rclpy.spin(node)
    rclpy.shutdown()

if __name__=="__main__":
    main()