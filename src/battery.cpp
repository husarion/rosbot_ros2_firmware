#include <battery.hpp>

sensor_msgs__msg__BatteryState battery_msg;

void init_battery()
{
  read_battery_voltage_and_show_status_thead.start(read_battery_voltage_and_show_status_task);
  fill_battery_msg(&battery_msg);
}

void read_battery_voltage_and_show_status_task()
{
  while (true)
  {
    battery_msg.voltage = read_battery_voltage_and_show_status();
    ThisThread::sleep_for(100);
  }
}

static void toggle_battery_led()
{
  battery_led = !battery_led;
}

float read_battery_voltage_and_show_status()
{
  static int index = 0;
  battery_data.voltage =
      3.3f * VIN_MEAS_CORRECTION * (UPPER_RESISTOR + LOWER_RESISTOR) / LOWER_RESISTOR * battery_adc.read();
  if (battery_data.threshold > battery_data.voltage && index < MEASUREMENT_SERIES)  // low level
    index++;
  else if (battery_data.threshold < battery_data.voltage && index > 0)
    index--;

  if (battery_data.status == BATTERY_OK && index == MEASUREMENT_SERIES)
  {
    battery_data.status = BATTERY_LOW;
    battery_led = 0;
    battery_led_flipper.attach(callback(toggle_battery_led), 0.4);
  }
  else if (battery_data.status == BATTERY_LOW && index == 0)
  {
    battery_data.status = BATTERY_OK;
    battery_led_flipper.detach();
    battery_led = 1;
  }
  return battery_data.voltage;
}

void fill_battery_msg(sensor_msgs__msg__BatteryState *msg) {
    msg->header.frame_id = micro_ros_string_utilities_set(msg->header.frame_id, "battery");

    if (rmw_uros_epoch_synchronized()) {
        msg->header.stamp.sec = (int32_t)(rmw_uros_epoch_nanos() / 1000000000);
        msg->header.stamp.nanosec = (uint32_t)(rmw_uros_epoch_nanos() % 1000000000);
    }

    msg->power_supply_technology = sensor_msgs__msg__BatteryState__POWER_SUPPLY_TECHNOLOGY_LION;
}