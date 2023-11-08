#pragma once

#include <mbed.h>
#include <ImuDriver.h>

#include <rmw_microros/rmw_microros.h>
#include <micro_ros_utilities/string_utilities.h>
#include <sensor_msgs/msg/imu.h>


static ImuDriver *imu_driver_ptr;
static sensor_msgs__msg__Imu imu_msg;
extern Mail<ImuDriver::ImuMeasurement, 10> imu_sensor_mail_box;


static const char* imu_sensor_type_string[] = { "BNO055_ADDR_A", "BNO055_ADDR_B", "MPU9250", "MPU9255", "BHI260", "UNKNOWN" };
static char imu_description_string[64] = "";

void init_imu(mbed::I2C *i2c_ptr);
void fill_imu_msg(sensor_msgs__msg__Imu *msg);
void fill_imu_msg_with_measurements(sensor_msgs__msg__Imu* msg, ImuDriver::ImuMeasurement* measurements);