#pragma once
#include <microros.hpp>

static UARTSerial microros_serial_rpi(RPI_SERIAL_TX, RPI_SERIAL_RX);
static UARTSerial microros_serial_ftdi(FT_SERIAL_TX, FT_SERIAL_RX);

static volatile bool distance_sensors_enabled = false;
static DigitalOut sens_power(SENS_POWER_ON, 0);
