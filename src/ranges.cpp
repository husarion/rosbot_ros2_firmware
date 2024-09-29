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

// #include <math.h>
// #include "sensor_msgs/msg/laser_scan.h"
// #include "micro_ros_utilities/string_utilities.h"
// #include "rmw_uros_time.h"

// // Function to fill in the LaserScan message with basic parameters
// void fill_laserscan_msg(sensor_msgs__msg__LaserScan* msg, uint8_t id)
// {
//   // Set frame ID for the LaserScan message
//   msg->header.frame_id = micro_ros_string_utilities_set(msg->header.frame_id, range_frame_names[id]);

//   // Set timestamp if the epoch is synchronized
//   if (rmw_uros_epoch_synchronized())
//   {
//     msg->header.stamp.sec = (int32_t)(rmw_uros_epoch_nanos() / 1000000000);
//     msg->header.stamp.nanosec = (uint32_t)(rmw_uros_epoch_nanos() % 1000000000);
//   }

//   // Laser scan angle configuration
//   msg->angle_min = -0.13;  // Starting angle of the scan [rad] (example value)
//   msg->angle_max = 0.13;   // Ending angle of the scan [rad] (example value)
//   msg->angle_increment = 0.01;  // Angular distance between measurements [rad]

//   // Time between measurements and total scan time (example values)
//   msg->time_increment = 0.0;  // Time between individual measurements [s]
//   msg->scan_time = 0.1;       // Time between scans [s]

//   // Minimum and maximum range of the sensor
//   msg->range_min = 0.01;
//   msg->range_max = 0.90;

//   // Initialize the ranges and intensities arrays
//   size_t num_readings = (size_t)((msg->angle_max - msg->angle_min) / msg->angle_increment);
//   msg->ranges.data = (float*)malloc(num_readings * sizeof(float));
//   msg->ranges.size = num_readings;
//   msg->ranges.capacity = num_readings;

//   msg->intensities.data = (float*)malloc(num_readings * sizeof(float));
//   msg->intensities.size = num_readings;
//   msg->intensities.capacity = num_readings;

//   // Initialize range and intensity values with default values
//   for (size_t i = 0; i < num_readings; i++)
//   {
//     msg->ranges.data[i] = NAN;  // Set range to NAN to indicate no detection
//     msg->intensities.data[i] = 0.0;  // Set intensity to 0.0 as default
//   }
// }

// // Function to fill in the LaserScan message with actual range measurements
// void fill_laserscan_msg_with_measurements(sensor_msgs__msg__LaserScan* msg, float range)
// {
//   // Determine the index of the range array to fill based on the provided range
//   size_t num_readings = msg->ranges.size;

//   // Fill each range measurement with the given value (can modify logic for actual scans)
//   for (size_t i = 0; i < num_readings; i++)
//   {
//     msg->ranges.data[i] = range;

//     // If the range is outside the sensor's valid range, set it to NAN
//     if (msg->ranges.data[i] > msg->range_max || msg->ranges.data[i] < msg->range_min)
//     {
//       msg->ranges.data[i] = NAN;
//     }

//     // Set intensity (optional)
//     msg->intensities.data[i] = 100.0;  // Example value, adjust as needed
//   }
// }
