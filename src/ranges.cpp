#include <ranges.hpp>

#define SENSOR_COUNT 4  // Number of sensors

// Function to initialize separate LaserScan messages for each sensor
void init_ranges()
{
    MultiDistanceSensor& distance_sensors = MultiDistanceSensor::getInstance();
    if (distance_sensors.init() > 0)
    {
        uint8_t* data = distance_sensor_commands.alloc();
        *data = 1;
        distance_sensor_commands.put(data);
    }

    // Initialize a separate LaserScan message for each sensor
    for (auto i = 0u; i < SENSOR_COUNT; ++i)
    {
        fill_range_msg(&range_msgs[i], i);
    }
}

// Static variables to store a single range and intensity measurement for each sensor
static float range_data_buf[SENSOR_COUNT] = {NAN, NAN, NAN, NAN};  // Initialize with NAN


// Function to fill in the LaserScan message with basic parameters for a specific sensor
void fill_range_msg(sensor_msgs__msg__LaserScan* msg, uint8_t id)
{
    // Set frame ID for the LaserScan message
    msg->header.frame_id = micro_ros_string_utilities_set(msg->header.frame_id, range_frame_names[id]);

    // Set timestamp if the epoch is synchronized
    if (rmw_uros_epoch_synchronized())
    {
        msg->header.stamp.sec = (int32_t)(rmw_uros_epoch_nanos() / 1000000000);
        msg->header.stamp.nanosec = (uint32_t)(rmw_uros_epoch_nanos() % 1000000000);
    }

    // Laser scan angle configuration (for single measurement)
    msg->angle_min = -0.13;         // Set to 0.0 as this is a single point measurement
    msg->angle_max = 0.13;         // Set to 0.0 as this is a single point measurement
    msg->angle_increment = 0.0;   // No increment as it's a single point

    // Time configuration
    msg->time_increment = 0.1;    // Time between individual measurements [s]
    msg->scan_time = 0.1;         // Time between scans [s]

    // Minimum and maximum range of the sensor
    msg->range_min = 0.01;
    msg->range_max = 0.90;

    // Initialize the ranges and intensities arrays for a single measurement
    msg->ranges.data = &range_data_buf[id];     // Point to the specific index for this sensor
    msg->ranges.size = 1;
    msg->ranges.capacity = 1;
}

// Function to fill in the LaserScan message with actual range measurements for a specific sensor
void fill_range_msg_with_measurements(sensor_msgs__msg__LaserScan* msg, float range)
{
  if(msg->ranges.data!=NULL) {
    msg->ranges.data[0] = range;        // Update the single range value for the given sensor
    if (msg->ranges.data[0] > msg->range_max || msg->ranges.data[0] < msg->range_min)
    {
      msg->ranges.data[0] = NAN;
    }
  }
}
