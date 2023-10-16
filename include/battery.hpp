#pragma once

#include <mbed.h>

#include <sensor_msgs/msg/battery_state.h>
#include <rmw_microros/rmw_microros.h>
#include <micro_ros_utilities/string_utilities.h>

constexpr int MEASUREMENT_SERIES = 10;
constexpr float BATTERY_VOLTAGE_LOW = 10.8;

enum
{
  BATTERY_LOW = 1,
  BATTERY_OK = 0
};

struct BatteryData
{
  float voltage;
  float threshold;
  uint8_t status;
};

static BatteryData battery_data = { 0.0, BATTERY_VOLTAGE_LOW, BATTERY_OK };
static DigitalOut battery_led(LED1, 1);
static Ticker battery_led_flipper;
static AnalogIn battery_adc(BAT_MEAS);

extern sensor_msgs__msg__BatteryState battery_msg;
static rtos::Thread read_battery_voltage_and_show_status_thead;

void init_battery();
float read_battery_voltage_and_show_status();
void read_battery_voltage_and_show_status_task();
void fill_battery_msg(sensor_msgs__msg__BatteryState *msg);