#pragma once

#include <map>
#include <mbed.h>
#include <std_msgs/msg/u_int32_multi_array.h>

static std::map<double, uint8_t> servo_voltage_configuration{
    {5.0, 0},
    {6.0, 1},
    {7.4, 2},
    {8.6, 3}
};

enum Servos{
    servo0,
    servo1,
    servo2,
    servo3,
    servo4,
    servo5,
    SERVOS_COUNT
};

extern std_msgs__msg__UInt32MultiArray servos_command_msg;

void init_servos();
void fill_servos_command_msg(std_msgs__msg__UInt32MultiArray *msg);
void servos_command_callback(const void* message);

namespace rosbot_sensors {

class ServoManger : NonCopyable<ServoManger>
{
public:
    enum : int
    {
        SERVO_OUTPUT_1 = 0,
        SERVO_OUTPUT_2 = 1,
        SERVO_OUTPUT_3 = 2,
        SERVO_OUTPUT_4 = 3,
        SERVO_OUTPUT_5 = 4,
        SERVO_OUTPUT_6 = 5,
        VOLTAGE_5V = 0,
        VOLTAGE_6V = 1,
        VOLTAGE_7_4V = 2,
        VOLTAGE_8_6V = 3,
    };

    ServoManger();

    void enablePower(bool en = true);
    int getEnabledOutputs();
    PwmOut* getOutput(int output);
    bool setWidth(int output, int width_us);
    bool setPeriod(int output, int period_us);
    void enableOutput(int output, bool en = true);
    void setPowerMode(int mode);

private:
    PwmOut* _servo[6];
    int _voltage_mode;
    int _enabled_outputs;
    DigitalOut _servo_sel1;
    DigitalOut _servo_sel2;
    DigitalOut _servo_power;
};

} // namespace rosbot_sensors

extern rosbot_sensors::ServoManger servo_manager;