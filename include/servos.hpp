#pragma once

#include <mbed.h>

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
