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

extern Mail<ImuDriver::ImuMeasurement, 10> imu_sensor_mail_box;

const char* imu_sensor_type_string[] = { "BNO055_ADDR_A", "BNO055_ADDR_B", "MPU9250", "MPU9255", "BHI260", "UNKNOWN" };
char imu_description_string[64] = "";

volatile uint8_t err_msg;

sensor_msgs__msg__Imu imu_msg;
sensor_msgs__msg__Range range_msgs[RANGES_COUNT];

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
    SensorsMeasurement* message = (SensorsMeasurement*)evt1.value.p;
    if (message->status == MultiDistanceSensor::ERR_I2C_FAILURE)
    {
      err_msg++;
      if (distance_sensor_commands.empty() && err_msg == 3)
      {
        uint8_t* data = distance_sensor_commands.alloc();
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
        range_msgs[i].range = message->range[i];
      }
    }
    distance_sensor_mail_box.free(message);
  }
}

void publish_range_sensors(rcl_timer_t* timer, int64_t last_call_time)
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
    ImuDriver::ImuMeasurement* message = (ImuDriver::ImuMeasurement*)evt2.value.p;
    fill_imu_msg(&imu_msg);
    imu_msg.orientation.y = message->orientation[1];
    imu_msg.orientation.z = message->orientation[2];
    imu_msg.orientation.x = message->orientation[0];
    imu_msg.orientation.w = message->orientation[3];

    imu_msg.angular_velocity.x = message->angular_velocity[0];
    imu_msg.angular_velocity.y = message->angular_velocity[1];
    imu_msg.angular_velocity.z = message->angular_velocity[2];

    imu_msg.linear_acceleration.x = message->linear_acceleration[0];
    imu_msg.linear_acceleration.y = message->linear_acceleration[1];
    imu_msg.linear_acceleration.z = message->linear_acceleration[2];
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


void servos_command_callback(const void* msgin)
{
  const std_msgs__msg__UInt32MultiArray* msg = (const std_msgs__msg__UInt32MultiArray*)msgin;
  if (msg != NULL and msg->data.size == SERVOS_COUNT)
  {
    for (auto i = 0u; i < SERVOS_COUNT; ++i)
    {
      servo_manager.setWidth(i, msg->data.data[i]);
    }
  }
}

void timer_callback(rcl_timer_t* timer, int64_t last_call_time)
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

bool on_parameter_changed(const Parameter* old_param, const Parameter* new_param, void* context)
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
  sens_power = 1;  // sensors power on
  ThisThread::sleep_for(100);
  init_battery();
  init_wheels();
  init_button_and_attach_to_callbacks(&button1, button1_rise_callback, button1_fall_callback);
  init_button_and_attach_to_callbacks(&button2, button2_rise_callback, button2_fall_callback);

  MultiDistanceSensor& distance_sensors = MultiDistanceSensor::getInstance();

  bool distance_sensors_init_flag = false;
  bool imu_init_flag = false;

  int num_sens_init;
  if ((num_sens_init = distance_sensors.init()) > 0)
  {
    distance_sensors_init_flag = true;
  }

  I2C* i2c_ptr = new I2C(IMU_I2C_SDA, IMU_I2C_SCL);
  i2c_ptr->frequency(IMU_I2C_FREQUENCY);

  ImuDriver::Type type = ImuDriver::getType(i2c_ptr, 2);
  sprintf(imu_description_string, "Detected sensor: %s\r\n", imu_sensor_type_string[type]);

  if (type != ImuDriver::UNKNOWN)
  {
    imu_driver_ptr = new ImuDriver(i2c_ptr, type);
    imu_driver_ptr->init();
    imu_driver_ptr->start();
    imu_init_flag = true;
  }

  if (imu_init_flag)
  {
    imu_driver_ptr->start();
  }

  if (distance_sensors_init_flag)
  {
    uint8_t* data = distance_sensor_commands.alloc();
    *data = 1;
    distance_sensor_commands.put(data);
    distance_sensors_enabled = true;
  }

  set_microros_serial_transports(&microros_serial);
  while (not rmw_uros_ping_agent(100, 1) == RMW_RET_OK)
  {
    ThisThread::sleep_for(100);
  }

  if (not microros_init())
  {
    microros_deinit();
    ThisThread::sleep_for(2000);

    NVIC_SystemReset();
  }

  fill_imu_msg(&imu_msg);
  fill_battery_msg(&battery_msg);
  fill_wheels_state_msg(&wheels_state_msg);

  for (auto i = 0u; i < RANGES_COUNT; ++i)
  {
    fill_range_msg(&range_msgs[i], i);
  }

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