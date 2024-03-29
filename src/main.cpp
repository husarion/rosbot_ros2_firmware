/** @file main.cpp
 * ROSbot firmware.
 *
 * @author Husarion
 * @copyright MIT
 */

#include <main.hpp>

#define MAIN_LOOP_INTERVAL_MS 10
#define IMU_I2C_FREQUENCY 100000L
#define IMU_I2C_SCL SENS2_PIN3
#define IMU_I2C_SDA SENS2_PIN4

volatile uint8_t err_msg;
static volatile uint32_t spin_count;

static mbed::InterruptIn button1(BUTTON1);
static mbed::InterruptIn button2(BUTTON2);

void button1_fall_callback()
{
  buttons_publish_flag[0] = true;
}

void button2_fall_callback()
{
  buttons_publish_flag[1] = true;
}

void button1_rise_callback()
{
  buttons_publish_flag[0] = false;
}

void button2_rise_callback()
{
  buttons_publish_flag[1] = false;
}

void range_sensors_msg_handler()
{
  osEvent evt1 = distance_sensor_mail_box.get(0);
  if (evt1.status == osEventMail)
  {
    SensorsMeasurement *message = (SensorsMeasurement *)evt1.value.p;
    if (message->status == MultiDistanceSensor::ERR_I2C_FAILURE)
    {
      err_msg++;
      if (distance_sensor_commands.empty() && err_msg == 3)
      {
        uint8_t *data = distance_sensor_commands.alloc();
        *data = 2;
        distance_sensor_commands.put(data);
        data = distance_sensor_commands.alloc();
        *data = 1;
        distance_sensor_commands.put(data);
        err_msg = 0;
      }
    }
    else
    {
      err_msg = 0;
      for (auto i = 0u; i < RANGES_COUNT; ++i)
      {
        fill_range_msg_with_measurements(&range_msgs[i], message->range[i]);
      }
    }
    distance_sensor_mail_box.free(message);
  }
}

void publish_range_sensors(rcl_timer_t *timer, int64_t last_call_time)
{
  RCLC_UNUSED(last_call_time);
  if (timer != NULL)
  {
    for (auto i = 0u; i < RANGES_COUNT; ++i)
    {
      fill_range_msg(&range_msgs[i], i);
      publish_range_msg(&range_msgs[i], i);
    }
  }
}

void imu_msg_handler()
{
  osEvent evt2 = imu_sensor_mail_box.get(0);

  if (evt2.status == osEventMail)
  {
    ImuDriver::ImuMeasurement *message = (ImuDriver::ImuMeasurement *)evt2.value.p;
    fill_imu_msg(&imu_msg);
    fill_imu_msg_with_measurements(&imu_msg, message);
    publish_imu_msg(&imu_msg);
    imu_sensor_mail_box.free(message);
  }
}

void battery_msg_handler()
{
  if (spin_count % 40 == 0)
  {
    fill_battery_msg(&battery_msg);
    publish_battery_msg(&battery_msg);
  }
}

void button_msgs_handler()
{
  for (auto i = 0u; i < BUTTONS_COUNT; ++i)
  {
    if (buttons_publish_flag[i] != button_msgs[i].data)
    {
      button_msgs[i].data = buttons_publish_flag[i];
      publish_button_msg(&button_msgs[i], i);
    }
  }
}

void wheels_state_msg_handler()
{
  if (spin_count % 5 == 0)
  {
    fill_wheels_state_msg(&wheels_state_msg);
    publish_wheels_state_msg(&wheels_state_msg);
  }
}

void timer_callback(rcl_timer_t *timer, int64_t last_call_time)
{
  RCLC_UNUSED(last_call_time);
  if (timer != NULL)
  {
    imu_msg_handler();
    wheels_state_msg_handler();
    button_msgs_handler();
    range_sensors_msg_handler();
    battery_msg_handler();
    spin_count++;
  }
}

bool on_parameter_changed(const Parameter *old_param, const Parameter *new_param, void *context)
{
  (void)context;
  if (old_param == NULL && new_param == NULL)
  {
    return false;
  }
  if (old_param != NULL and new_param != NULL)
  {
    std::map<double, uint8_t>::iterator it;
    switch (old_param->value.type)
    {
    case RCLC_PARAMETER_DOUBLE:
      it = servo_voltage_configuration.find(new_param->value.double_value);
      if (it == servo_voltage_configuration.end())
      {
        return false;
      }
      servo_manager.setPowerMode(it->second);
      break;
    case RCLC_PARAMETER_BOOL:
      if (not strcmp(new_param->name.data, "servo_enable_power"))
      {
        servo_manager.enablePower(new_param->value.bool_value);
      }
      else if (isdigit(new_param->name.data[5]))
      {
        servo_manager.enableOutput(new_param->name.data[5] - '0', new_param->value.bool_value);
      }
      else
      {
        return false;
      }
      break;
    case RCLC_PARAMETER_INT:
      if (isdigit(new_param->name.data[5]))
      {
        servo_manager.setPeriod(new_param->name.data[5] - '0', new_param->value.integer_value);
      }
      else
      {
        return false;
      }
      break;
    default:
      break;
    }
  }
  return true;
}

int main()
{
  ThisThread::sleep_for(100);
  sens_power = 1; // sensors power on
  ThisThread::sleep_for(100);

  init_battery();
  init_wheels();
  init_button_and_attach_to_callbacks(&button1, button1_rise_callback, button1_fall_callback);
  init_button_and_attach_to_callbacks(&button2, button2_rise_callback, button2_fall_callback);

  I2C *i2c_ptr = new I2C(IMU_I2C_SDA, IMU_I2C_SCL);
  i2c_ptr->frequency(IMU_I2C_FREQUENCY);
  init_imu(i2c_ptr);
  init_servos();
  init_ranges();

  // if button1 or button2 are pressed, connect through USB
  if (button1.read() == true && button2.read() == true)
  {
    led2=0;
    led3=1;
    set_microros_serial_transports(&microros_serial_rpi);
  }
  else
  {
    led2=1;
    led3=0;
    set_microros_serial_transports(&microros_serial_ftdi);
  }

  uint32_t cnt=0;
  while (not rmw_uros_ping_agent(100, 1) == RMW_RET_OK)
  {
    ThisThread::sleep_for(100);
    cnt++;
    if (cnt == 30) {
       NVIC_SystemReset();
    }
  }

  if (not microros_init())
  {
    microros_deinit();
    ThisThread::sleep_for(2000);
    NVIC_SystemReset();
  }

  led2=0;
  led3=0;

  AgentStates state = AGENT_CONNECTED;
  while (state == AGENT_CONNECTED)
  {
    EXECUTE_EVERY_N_MS(2000,
                       state = (RMW_RET_OK == rmw_uros_ping_agent(200, 5)) ? AGENT_CONNECTED : AGENT_DISCONNECTED;);
    microros_spin();
  }

  for (int i = 0; i < 10; ++i)
  {
    ThisThread::sleep_for(200);
  }
  microros_deinit();
  NVIC_SystemReset();
}