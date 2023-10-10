#pragma once
#include <ImuDriver.h>
#include <RosbotDrive.h>

#include <memory_debug_message_info.hpp>
#include <microros.hpp>
#include <map>

#ifndef KINEMATIC_TYPE
#define KINEMATIC_TYPE 0
#endif

#ifndef JOINT_STATES_ENABLE
#define JOINT_STATES_ENABLE 0
#endif

#if JOINT_STATES_ENABLE
static volatile bool joint_states_enabled = true;
#else
static volatile bool joint_states_enabled = false;
#endif
static UARTSerial microros_serial(RPI_SERIAL_TX, RPI_SERIAL_RX);

static volatile bool distance_sensors_enabled = false;

static volatile bool buttons_publish_flag[BUTTONS_COUNT] = {false, false};
static volatile float button_release_time = 0.1;

static volatile float battery_voltage = 0.0;

static DigitalOut sens_power(SENS_POWER_ON, 0);

static InterruptIn button1(BUTTON1);
static InterruptIn button2(BUTTON2);
static ImuDriver *imu_driver_ptr;
static rosbot_sensors::ServoManger servo_manager;

std::map<double, uint8_t> servo_voltage_configuration{
    {5.0, 0},
    {6.0, 1},
    {7.4, 2},
    {8.6, 3}
};
