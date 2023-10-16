#pragma once
#include <ImuDriver.h>
#include <RosbotDrive.h>

#include <memory_debug_message_info.hpp>
#include <microros.hpp>
#include <map>
#include "MultiDistanceSensor.h"

static UARTSerial microros_serial(RPI_SERIAL_TX, RPI_SERIAL_RX);

static volatile bool distance_sensors_enabled = false;
static DigitalOut sens_power(SENS_POWER_ON, 0);

static ImuDriver *imu_driver_ptr;
static rosbot_sensors::ServoManger servo_manager;

std::map<double, uint8_t> servo_voltage_configuration{
    {5.0, 0},
    {6.0, 1},
    {7.4, 2},
    {8.6, 3}
};
