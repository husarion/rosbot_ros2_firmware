#pragma once
#include "UARTSerial.h"

#define GET_DOMAIN_ID_LENGHT 3

void set_microros_serial_transports(mbed::UARTSerial *serial);
size_t get_ros_domain_id(mbed::UARTSerial *serial, size_t* ros_domain_id);