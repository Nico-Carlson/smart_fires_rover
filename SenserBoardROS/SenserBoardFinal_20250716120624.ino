#include <Arduino.h>
#include <micro_ros_arduino.h>
#include <stdio.h>
#include <rcl/rcl.h>
#include <rcl/error_handling.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>
#include <std_msgs/msg/bool.h>
#include <std_msgs/msg/float64.h>

#include "globals.h"

// --- ROS 2 Components ---
rcl_publisher_t pub_danger;
rcl_publisher_t pub_warning;
rcl_publisher_t pub_turnangle;
std_msgs__msg__Bool msg_danger;
std_msgs__msg__Bool msg_warning;
std_msgs__msg__Float64 msg_turnangle;

rclc_executor_t executor;
rclc_support_t support;
rcl_allocator_t allocator;
rcl_node_t node;
rcl_timer_t timer;

// --- External Setup Functions ---
void gyro_setup();
void gyro();
void echo_setup();
void echo();
void warning_switch_setup();
void warning_switch();
void light_setup();
void light();
void bluetooth_setup();
void bluetooth();
void battery();
void lower_limit_setup();
void lower_limit();
void upper_limit_setup();
void upper_limit();

#define RCCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){error_loop();}}
#define RCSOFTCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){}}

void error_loop(){
  while(1){
    digitalWrite(LightPin, HIGH);
    delay(100);
    digitalWrite(LightPin, LOW);
    delay(100);
  }
}

// --- The Main ROS 2 Loop (Runs every 100ms) ---
void timer_callback(rcl_timer_t * timer, int64_t last_call_time)
{
  RCLC_UNUSED(last_call_time);
  if (timer != NULL) {

    // 1. Run all your hardware checks
    gyro();
    echo();
    warning_switch();
    bluetooth();
    battery();
    lower_limit();
    upper_limit();

    // 2. Package data for ROS 2
    msg_danger.data = robot_danger;
    msg_warning.data = robot_warning;
    msg_turnangle.data = turnangle;

    // 3. Publish to the Raspberry Pi 5
    RCSOFTCHECK(rcl_publish(&pub_danger, &msg_danger, NULL));
    RCSOFTCHECK(rcl_publish(&pub_warning, &msg_warning, NULL));
    RCSOFTCHECK(rcl_publish(&pub_turnangle, &msg_turnangle, NULL));
  }
}

void setup() {
  // Initialize USB transport for micro-ROS (Replaces Serial2)
  set_microros_transports();

  // Initialize your hardware
  gyro_setup();
  echo_setup();
  warning_switch_setup();
  bluetooth_setup();
  lower_limit_setup();
  upper_limit_setup();

  delay(2000); // Give MPU6050 time to boot

  allocator = rcl_get_default_allocator();

  RCCHECK(rclc_support_init(&support, 0, NULL, &allocator));
  RCCHECK(rclc_node_init_default(&node, "sensor_board_node", "", &support));

  // Create Publishers
  RCCHECK(rclc_publisher_init_default(
    &pub_danger, &node, ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Bool), "rover/danger"));
  RCCHECK(rclc_publisher_init_default(
    &pub_warning, &node, ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Bool), "rover/warning"));
  RCCHECK(rclc_publisher_init_default(
    &pub_turnangle, &node, ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Float64), "rover/turnangle"));

  // Create timer (Runs at 10Hz / 100ms)
  const unsigned int timer_timeout = 100;
  RCCHECK(rclc_timer_init_default(&timer, &support, RCL_MS_TO_NS(timer_timeout), timer_callback));

  RCCHECK(rclc_executor_init(&executor, &support.context, 1, &allocator));
  RCCHECK(rclc_executor_add_timer(&executor, &timer));
}

void loop() {
  // Spin the ROS 2 executor to handle timing and communications
  delay(10);
  rclc_executor_spin_some(&executor, RCL_MS_TO_NS(100));
}
