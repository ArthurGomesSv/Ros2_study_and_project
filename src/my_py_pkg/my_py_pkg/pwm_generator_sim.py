#!/usr/bin/env python3
import rclpy
from rclpy.node import Node
from std_msgs.msg import Int32


class NumberPublisherNode(Node):

    def __init__(self):
        super().__init__("pwm_generator_sim")
        self.number_to_publish_ = 0
        self.isup = True
        self.publisher_ = self.create_publisher(Int32,"control/pwm",10)
        self.create_timer(0.1,self.number_callback)
        self.get_logger().info("Pwm sim publisher has been started")

    def number_callback(self):
        msg = Int32()
        msg.data = self.number_to_publish_
    
        if self.isup:
            if self.number_to_publish_<8100:
                self.number_to_publish_+=100
                self.get_logger().info("Pwm = " + str(self.number_to_publish_))
            else:
                self.number_to_publish_-=100
                self.isup = False
        else:
            if self.number_to_publish_>0:
                self.number_to_publish_-=100
                self.get_logger().info("Pwm = " + str(self.number_to_publish_))
            else:
                self.number_to_publish_+=100
                self.isup = True
                
        self.publisher_.publish(msg)


        
        
def main(args=None):
    rclpy.init(args=args)
    node=NumberPublisherNode()
    rclpy.spin(node)
    rclpy.shutdown()

if __name__=="__main__":
    main()