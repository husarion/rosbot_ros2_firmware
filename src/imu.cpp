#include <imu.hpp>

void init_imu(mbed::I2C* i2c_ptr)
{
  ImuDriver::Type type = ImuDriver::getType(i2c_ptr, 2);
  sprintf(imu_description_string, "Detected sensor: %s\r\n", imu_sensor_type_string[type]);

  if (type != ImuDriver::UNKNOWN)
  {
    imu_driver_ptr = new ImuDriver(i2c_ptr, type);
    imu_driver_ptr->init();
    imu_driver_ptr->start();
  }

  fill_imu_msg(&imu_msg);
}

void fill_imu_msg(sensor_msgs__msg__Imu* msg)
{
  msg->header.frame_id = micro_ros_string_utilities_set(msg->header.frame_id, "imu");

  if (rmw_uros_epoch_synchronized())
  {
    msg->header.stamp.sec = (int32_t)(rmw_uros_epoch_nanos() / 1000000000);
    msg->header.stamp.nanosec = (uint32_t)(rmw_uros_epoch_nanos() % 1000000000);
  }
  msg->orientation.x = 0;
  msg->orientation.y = 0;
  msg->orientation.z = 0;
  msg->orientation.w = 1;
  msg->angular_velocity.x = 0;
  msg->angular_velocity.y = 0;
  msg->angular_velocity.z = 0;
  msg->linear_acceleration.x = 0;
  msg->linear_acceleration.y = 0;
  msg->linear_acceleration.z = 0;
  for (auto i = 0u; i < 9u; ++i)
  {
    msg->angular_velocity_covariance[i] = msg->linear_acceleration_covariance[i] = msg->orientation_covariance[i] = 0.0;
  }
  msg->orientation_covariance[9] = 0.0;
  msg->orientation_covariance[10] = 0.0;
  msg->orientation_covariance[11] = 0.0;
}

void fill_imu_msg_with_measurements(sensor_msgs__msg__Imu* msg, ImuDriver::ImuMeasurement* measurements)
{
  msg->orientation.y = measurements->orientation[1];
  msg->orientation.z = measurements->orientation[2];
  msg->orientation.x = measurements->orientation[0];
  msg->orientation.w = measurements->orientation[3];

  msg->angular_velocity.x = measurements->angular_velocity[0];
  msg->angular_velocity.y = measurements->angular_velocity[1];
  msg->angular_velocity.z = measurements->angular_velocity[2];

  msg->linear_acceleration.x = measurements->linear_acceleration[0];
  msg->linear_acceleration.y = measurements->linear_acceleration[1];
  msg->linear_acceleration.z = measurements->linear_acceleration[2];
}