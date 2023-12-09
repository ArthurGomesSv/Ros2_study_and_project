from launch import LaunchDescription
from launch_ros.actions import Node

def generate_launch_description():
    ld = LaunchDescription()

    realsense_camera_node  = Node(
        package="realsense2_camera",
        executable="realsense2_camera_node",
        parameters=[
            {"align_depth.enable":True},
            {"enable_rgbd":True},
            {"enable_sync":True},
            {"enable_color":True},
            {"enable_depth":True}
        ]
    )

    holder_ball_robot_node = Node(
        package="my_cpp_pkg",
        executable="holder_ball_robot"
    )

    image_processing = Node(
        package="camera_pkg",
        executable="image_processing"
    )


    ld.add_action(realsense_camera_node)
    ld.add_action(holder_ball_robot_node)
    ld.add_action(image_processing)

    return ld