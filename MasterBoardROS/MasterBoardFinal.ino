#include <Arduino.h>
#include <micro_ros_arduino.h>
#include <stdio.h>
#include <rcl/rcl.h>
#include <rcl/error_handling.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>

// ROS 2 Message Types
#include <std_msgs/msg/bool.h>
#include <geometry_msgs/msg/twist.h> // The standard ROS 2 driving message

#include "globals.h"

// --- ROS 2 Components ---
rcl_subscription_t sub_danger;
rcl_subscription_t sub_warning;
rcl_subscription_t sub_cmd_vel;

std_msgs__msg__Bool msg_danger;
std_msgs__msg__Bool msg_warning;
geometry_msgs__msg__Twist msg_cmd_vel;

rclc_executor_t executor;
rclc_support_t support;
rcl_allocator_t allocator;
rcl_node_t node;

// --- External Setup Functions ---
void throttle_and_brakes_setup();
void throttle_and_brakes();
void reverse_setup();
void reverse();
void lift_setup();
void lift();
void turning_setup();
void turning();

#define RCCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){error_loop();}}
#define RCSOFTCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){}}

void error_loop(){
  while(1){
    // If micro-ROS fails, halt the rover immediately
    robot_danger = true;
    throttle_and_brakes();
    delay(100);
  }
}

// --- ROS 2 Callbacks ---

void danger_callback(const void * msgin) {
  const std_msgs__msg__Bool * msg = (const std_msgs__msg__Bool *)msgin;
  robot_danger = msg->data;
}

void warning_callback(const void * msgin) {
  const std_msgs__msg__Bool * msg = (const std_msgs__msg__Bool *)msgin;
  robot_warning = msg->data;
}

void cmd_vel_callback(const void * msgin) {
  const geometry_msgs__msg__Twist * msg = (const geometry_msgs__msg__Twist *)msgin;

  // 1. Steering (angular.z)
  // Nav2 outputs turning speed in Radians per Second.
  // We multiply by (180/PI) to convert it to degrees (-30 to +30) for turning.cpp
  turnangle = msg->angular.z * (180.0 / 3.14159265);

  // 2. Throttle (linear.x)
  // Nav2 outputs forward speed in Meters per Second (e.g., 0.5 to 2.0).
  forward_speed = msg->linear.x;
}

void setup() {
  // Initialize USB transport for micro-ROS (Replaces Serial and Serial2)
  set_microros_transports();

  // Initialize hardware
  throttle_and_brakes_setup();
  reverse_setup();
  lift_setup();
  turning_setup();

  delay(2000);

  allocator = rcl_get_default_allocator();

  // Initialize micro-ROS
  RCCHECK(rclc_support_init(&support, 0, NULL, &allocator));
  RCCHECK(rclc_node_init_default(&node, "master_board_node", "", &support));

  // Subscribe to Danger
  RCCHECK(rclc_subscription_init_default(
    &sub_danger, &node, ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Bool), "rover/danger"));

  // Subscribe to Warning
  RCCHECK(rclc_subscription_init_default(
    &sub_warning, &node, ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Bool), "rover/warning"));

  // Subscribe to AI Driving Commands
  RCCHECK(rclc_subscription_init_default(
    &sub_cmd_vel, &node, ROSIDL_GET_MSG_TYPE_SUPPORT(geometry_msgs, msg, Twist), "cmd_vel"));

  // Create Executor (Number '3' means we have 3 subscriptions to listen to)
  RCCHECK(rclc_executor_init(&executor, &support.context, 3, &allocator));
  RCCHECK(rclc_executor_add_subscription(&executor, &sub_danger, &msg_danger, &danger_callback, ON_NEW_DATA));
  RCCHECK(rclc_executor_add_subscription(&executor, &sub_warning, &msg_warning, &warning_callback, ON_NEW_DATA));
  RCCHECK(rclc_executor_add_subscription(&executor, &sub_cmd_vel, &msg_cmd_vel, &cmd_vel_callback, ON_NEW_DATA));
}

void loop() {
  // 1. Read incoming ROS 2 messages from the Pi
  rclc_executor_spin_some(&executor, RCL_MS_TO_NS(10));

  // 2. Drive the hardware using the updated global variables
  throttle_and_brakes();
  reverse();
  lift();
  turning();
}
