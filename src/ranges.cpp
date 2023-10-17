#include <ranges.hpp>

void init_ranges()
{
  MultiDistanceSensor& distance_sensors = MultiDistanceSensor::getInstance();
  if (distance_sensors.init() > 0)
  {
    uint8_t* data = distance_sensor_commands.alloc();
    *data = 1;
    distance_sensor_commands.put(data);
  }

  for (auto i = 0u; i < RANGES_COUNT; ++i)
  {
    fill_range_msg(&range_msgs[i], i);
  }
}

void fill_range_msg(sensor_msgs__msg__Range* msg, uint8_t id)
{
  msg->header.frame_id = micro_ros_string_utilities_set(msg->header.frame_id, range_frame_names[id]);

  if (rmw_uros_epoch_synchronized())
  {
    msg->header.stamp.sec = (int32_t)(rmw_uros_epoch_nanos() / 1000000000);
    msg->header.stamp.nanosec = (uint32_t)(rmw_uros_epoch_nanos() % 1000000000);
  }

  msg->radiation_type = sensor_msgs__msg__Range__INFRARED;
  msg->field_of_view = 0.26;
  msg->min_range = 0.01;
  msg->max_range = 0.90;
}

void fill_range_msg_with_measurements(sensor_msgs__msg__Range* msg, float range)
{
  msg->range = range;
  if (msg->range > msg->max_range || msg->range < msg->min_range)
  {
    msg->range = NAN;
  }
}