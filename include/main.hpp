#pragma once
#include <microros.hpp>

static UARTSerial microros_serial(RPI_SERIAL_TX, RPI_SERIAL_RX);

static volatile bool distance_sensors_enabled = false;
static DigitalOut sens_power(SENS_POWER_ON, 0);
