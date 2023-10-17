#pragma once
#include <RosbotDrive.h>

#include <memory_debug_message_info.hpp>
#include <microros.hpp>
#include <map>
#include "MultiDistanceSensor.h"

static UARTSerial microros_serial(RPI_SERIAL_TX, RPI_SERIAL_RX);

static volatile bool distance_sensors_enabled = false;
static DigitalOut sens_power(SENS_POWER_ON, 0);
