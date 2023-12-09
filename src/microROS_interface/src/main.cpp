#include <Arduino.h>
#include <micro_ros_platformio.h>

#include <rcl/rcl.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>

#include <std_msgs/msg/int32.h>

#if !defined(MICRO_ROS_TRANSPORT_ARDUINO_SERIAL)
#error This example is only avaliable for Arduino framework with serial transport.
#endif

u_int32_t pwm;
u_int8_t pwm_pin = 2;
u_int8_t pwm_channel = 0;
u_int32_t pwm_channel_freq = 1000;
u_int8_t pwm_channel_res = 13;

std_msgs__msg__Int32 msg_pwm;
rcl_subscription_t subscriber_;

rclc_executor_t executor_sub;
rclc_support_t support;
rcl_allocator_t allocator;
rcl_node_t node;

#define RCCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){error_loop();}}
#define RCSOFTCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){}}

// Error handle loop
void error_loop() {
  while(1) {
    delay(100);
  }
}

void subscriber_callback(const void * msgin)
{
  const std_msgs__msg__Int32 *msg_pwm = (const std_msgs__msg__Int32 *)msgin;

  ledcWrite(pwm_channel,(u_int32_t)msg_pwm->data);

}

void setup() {
  // Configure serial transport
  Serial.begin(115200);
  set_microros_serial_transports(Serial);
  delay(2000);

  //Pins configs
  pinMode(pwm_pin,OUTPUT);
  ledcAttachPin(pwm_pin,pwm_channel);
  ledcSetup(pwm_channel,pwm_channel_freq,pwm_channel_res);

  allocator = rcl_get_default_allocator();

  //create init_options
  RCCHECK(rclc_support_init(&support, 0, NULL, &allocator));

  // create node
  RCCHECK(rclc_node_init_default(&node, "micro_ros_esp32_pwm_node", "", &support));

  // create subscriber
  RCCHECK(rclc_subscription_init_default(
      &subscriber_,
      &node,
      ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int32),
      "esp/holder_pwm"));

  // create executor

  RCCHECK(rclc_executor_init(&executor_sub, &support.context, 1, &allocator));
  RCCHECK(rclc_executor_add_subscription(&executor_sub, &subscriber_, &msg_pwm, &subscriber_callback, ON_NEW_DATA));

}

void loop() {
  delay(10);
  RCSOFTCHECK(rclc_executor_spin_some(&executor_sub, RCL_MS_TO_NS(10)));
}