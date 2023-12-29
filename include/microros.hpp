#pragma once
#include <RosbotDrive.h>
#include <mbed.h>
#include <micro_ros_utilities/string_utilities.h>
#include <rcl/error_handling.h>
#include <rcl/rcl.h>
#include <rclc/executor.h>
#include <rclc/rclc.h>
#include <rmw_microros/rmw_microros.h>
#include <microros_transport/mbed_serial_transport.hpp>
#include <rclc_parameter/rclc_parameter.h>

#include <leds.hpp>
#include <wheels.hpp>
#include <buttons.hpp>
#include <battery.hpp>
#include <servos.hpp>
#include <imu.hpp>
#include <ranges.hpp>

constexpr const uint8_t UXR_CLIENT_DOMAIN_ID_TO_OVERRIDE_WITH_ENV = 255;

constexpr const char *NODE_NAME = "rosbot_ros2_firmware";
constexpr const char *IMU_TOPIC_NAME = "_imu/data_raw";
constexpr const char *WHEELS_STATE_TOPIC_NAME = "_motors_response";
constexpr const char *WHEELS_COMMAND_TOPIC_NAME = "_motors_cmd";
constexpr const char *BATTERY_TOPIC_NAME = "battery";
constexpr const char *SERVOS_COMMAND_TOPIC_NAME = "cmd_ser";

constexpr const char *GET_CPU_ID_SERVICE_NAME = "/get_cpu_id";

enum AgentStates {
    WAITING_AGENT,
    AGENT_AVAILABLE,
    AGENT_CONNECTED,
    AGENT_DISCONNECTED
};

bool microros_deinit();

#define RCCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){return false;}}
#define RCSOFTCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){led3 = 1;}}

#define EXECUTE_EVERY_N_MS(MS, X)          \
    do {                                   \
        static volatile int64_t init = -1; \
        if (init == -1) {                  \
            init = uxr_millis();           \
        }                                  \
        if (uxr_millis() - init > MS) {    \
            X;                             \
            init = uxr_millis();           \
        }                                  \
    } while (0)

void error_loop();

bool microros_init();
bool microros_deinit();
bool microros_spin();

bool init_imu_publisher();
bool init_wheels_state_publisher();
bool init_battery_publisher();
bool init_range_publishers();
bool init_wheels_command_subscriber();
bool init_servos_command_subscriber();
bool init_button_publishers();
bool init_led_subscribers();
bool init_param_server();
bool init_parameters();
bool init_services();

bool publish_imu_msg(sensor_msgs__msg__Imu *imu_msg);
bool publish_wheels_state_msg(sensor_msgs__msg__JointState *msg);
bool publish_battery_msg(sensor_msgs__msg__BatteryState *msg);
bool publish_range_msg(sensor_msgs__msg__Range *msg, uint8_t id);
bool publish_button_msg(std_msgs__msg__Bool *msg, uint8_t id);

void get_cpu_id_service_callback(const void *request, void *response);