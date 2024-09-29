#pragma once

#include <mbed.h>
#include <MultiDistanceSensor.h>

#include <rmw_microros/rmw_microros.h>
#include <micro_ros_utilities/string_utilities.h>
#include <sensor_msgs/msg/laser_scan.h>

enum Ranges {
    range_right_front,
    range_left_front,
    range_right_rear,
    range_left_rear,
    RANGES_COUNT
};

static const char *range_frame_names[] = {"fr_range", "fl_range", "rr_range", "rl_range"};

static sensor_msgs__msg__LaserScan range_msgs[RANGES_COUNT];

void init_ranges();
void fill_range_msg(sensor_msgs__msg__LaserScan* msg, uint8_t id);
void fill_range_msg_with_measurements(sensor_msgs__msg__LaserScan* msg, float range);
