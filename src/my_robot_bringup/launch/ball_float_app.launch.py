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

    image_processing = Node(
        package="camera_pkg",
        executable="image_processing"
    )

    control_node = Node(
        package="control_pkg",
        executable="control",
        parameters=[
            {"KP":120},
            {"KD":120},
            {"KI":120}
        ]
    )

    holder_ball_node = Node(
        package="communication_pkg",
        executable="holder_ball_robot"
    )


    ld.add_action(realsense_camera_node)
    ld.add_action(image_processing)
    ld.add_action(control_node)
    ld.add_action(holder_ball_node)

    return ld