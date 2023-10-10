#pragma once

#include <mbed.h>

#include <RosbotDrive.h>

#include <rmw_microros/rmw_microros.h>
#include <micro_ros_utilities/string_utilities.h>
#include <sensor_msgs/msg/joint_state.h>
#include <std_msgs/msg/float32_multi_array.h>

// #include "leds.hpp"

// Motors setup
#define MOTOR_FR MOTOR1
#define MOTOR_FL MOTOR4
#define MOTOR_RR MOTOR2
#define MOTOR_RL MOTOR3

constexpr const char* FRONT_LEFT_MOTOR_NAME = "fl_wheel_joint";
constexpr const char* FRONT_RIGHT_MOTOR_NAME = "fr_wheel_joint";
constexpr const char* REAR_LEFT_MOTOR_NAME = "rl_wheel_joint";
constexpr const char* REAR_RIGHT_MOTOR_NAME = "rr_wheel_joint";

static volatile float last_wheels_speed_calc_time = 0.0;
static volatile uint32_t last_speed_command_time = 0;
static volatile bool is_speed_watchdog_active = false;
static volatile uint32_t spin_count;
static volatile bool is_speed_watchdog_enabled = true;
static volatile uint64_t speed_watchdog_interval = 1000;  // ms

static mbed::Timer odom_watchdog_timer;
static rtos::Thread wheels_state_read_thread;

constexpr uint8_t POLARITY = 0b00111100;
constexpr float ROBOT_LENGTH = 0.197;
constexpr uint8_t ENCODER_CPR = 48;
constexpr float ROBOT_LENGTH_HALF = ROBOT_LENGTH / 2.0;
constexpr float DISTANCE_FRONT_TO_REAR_WHEEL = 0.11;
constexpr float WHEEL_SEPARATION_LENGTH = DISTANCE_FRONT_TO_REAR_WHEEL / 2;
constexpr float ROBOT_WIDTH = 0.215;  // 0.22 0.195
constexpr float ROBOT_WIDTH_HALF = ROBOT_WIDTH / 2.0;
constexpr float DIAMETER_MODIFICATOR = 1.106;  // 1.24, 1.09, 1.164
constexpr float TYRE_DEFLATION = 1.042;        // theoretical distance / real distance
constexpr float GEAR_RATIO = 34.014;
constexpr float WHEEL_DIAMETER = 0.085;
constexpr float WHEEL_RADIUS = WHEEL_DIAMETER / 2.0;

static sensor_msgs__msg__JointState wheels_state_msg;
extern std_msgs__msg__Float32MultiArray wheels_command_msg;

enum Motors
{
  motor_right_rear,
  motor_left_rear,
  motor_right_front,
  motor_left_front,
  MOTORS_COUNT
};

enum MotorsState
{
  motor_state_position,
  motor_state_velocity,
  motor_state_effort,
  MOTORS_STATE_COUNT
};

void init_wheels();
void update_wheels_states();
void check_speed_watchdog();
void wheels_state_read_and_watchdog_task();

void wheels_command_callback(const void* message);

void fill_wheels_command_msg(std_msgs__msg__Float32MultiArray *msg);
void fill_wheels_state_msg(sensor_msgs__msg__JointState* msg);