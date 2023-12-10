cmake_minimum_required(VERSION 3.8)
project(communication_pkg)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)

add_executable(holder_ball_robot src/holder_ball_robot.cpp)
ament_target_dependencies(holder_ball_robot rclcpp std_msgs)

install(TARGETS
  holder_ball_robot
  DESTINATION lib/${PROJECT_NAME}
)

ament_package()
