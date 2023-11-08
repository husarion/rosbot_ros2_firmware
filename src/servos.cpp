#include "servos.hpp"
rosbot_sensors::ServoManger servo_manager;
std_msgs__msg__UInt32MultiArray servos_command_msg;

void init_servos(){
    fill_servos_command_msg(&servos_command_msg);
}

void fill_servos_command_msg(std_msgs__msg__UInt32MultiArray* msg)
{
  static uint32_t data[SERVOS_COUNT] = { 0, 0, 0, 0, 0, 0 };
  msg->data.capacity = SERVOS_COUNT;
  msg->data.size = 0;
  msg->data.data = (uint32_t*)data;
}

void servos_command_callback(const void* message)
{
  const std_msgs__msg__UInt32MultiArray* msg = (const std_msgs__msg__UInt32MultiArray*)message;
  if (msg != NULL and msg->data.size == SERVOS_COUNT)
  {
    for (auto i = 0u; i < SERVOS_COUNT; ++i)
    {
      servo_manager.setWidth(i, msg->data.data[i]);
    }
  }
}

namespace rosbot_sensors
{

ServoManger::ServoManger()
  : _servo{ nullptr, nullptr, nullptr, nullptr, nullptr, nullptr }
  , _voltage_mode(0)
  , _enabled_outputs(0)
  , _servo_sel1(SERVO_SEL1, 0)
  , _servo_sel2(SERVO_SEL2, 0)
  , _servo_power(SERVO_POWER_ON, 0)
{
}

void ServoManger::enablePower(bool en)
{
  _servo_power.write(en ? 1 : 0);
}

int ServoManger::getEnabledOutputs()
{
  return _enabled_outputs;
}

PwmOut* ServoManger::getOutput(int output)
{
  if (output < SERVO_OUTPUT_1 || output > SERVO_OUTPUT_6)
    return nullptr;

  return _servo[output];
}

bool ServoManger::setWidth(int output, int width_us)
{
  if (output < SERVO_OUTPUT_1 || output > SERVO_OUTPUT_6)
    return false;

  if (_servo[output] == nullptr)
    return false;

  _servo[output]->pulsewidth_us(width_us);
  return true;
}

bool ServoManger::setPeriod(int output, int period_us)
{
  if (output < SERVO_OUTPUT_1 || output > SERVO_OUTPUT_6)
    return false;

  if (_servo[output] == nullptr)
    return false;

  _servo[output]->period_us(period_us);
  return true;
}

void ServoManger::enableOutput(int output, bool en)
{
  if (output < SERVO_OUTPUT_1 || output > SERVO_OUTPUT_6)
    return;

  if (en && _servo[output] == nullptr)
  {
    switch (output)
    {
      case SERVO_OUTPUT_1:
        _servo[output] = new PwmOut(SERVO1_PWM);
        break;
      case SERVO_OUTPUT_2:
        _servo[output] = new PwmOut(SERVO2_PWM);
        break;
      case SERVO_OUTPUT_3:
        _servo[output] = new PwmOut(SERVO3_PWM);
        break;
      case SERVO_OUTPUT_4:
        _servo[output] = new PwmOut(SERVO4_PWM);
        break;
      case SERVO_OUTPUT_5:
        _servo[output] = new PwmOut(SERVO5_PWM_ALT1);
        break;
      case SERVO_OUTPUT_6:
        _servo[output] = new PwmOut(SERVO6_PWM_ALT1);
        break;
    }
    _enabled_outputs++;
  }
  else if (_servo[output] != nullptr && !en)
  {
    delete _servo[output];
    _servo[output] = nullptr;
    _enabled_outputs--;
  }
}

void ServoManger::setPowerMode(int mode)
{
  _servo_sel1.write(mode & 0x01L);
  _servo_sel2.write(mode & 0x02L);
}

}  // namespace rosbot_sensors
