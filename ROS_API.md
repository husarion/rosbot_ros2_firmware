Use `micro_ros_agent` to communicate with all firmware functionalities.
```bash
ros2 run micro_ros_agent micro_ros_agent serial -D $SERIAL_PORT serial -b 576000
```
- `rosbot_ros2_firmware` it is a micro-ROS node on CORE2 inside ROSbot 2, 2R, PRO. It is used to publish all the sensor data such as wheels positions, IMU measurements, battery level and buttons states from firmware to ROS2 and also to subscribe command values such as motors speeds, servos periods, servos parameters and LEDs states.
    **Subscribes**
    - `/cmd_ser` (_std_msgs/msg/UInt32MultiArray[6]_)
    - `/led/left` (_std_msgs/msg/Bool_)
    - `/led/right` (_std_msgs/msg/Bool_)
    - `/_motors_cmd` (_std_msgs/msg/Float32MultiArray[4]_)

    **Publishes**
    - `/_motors_response` (_*sensor_msgs/msg/JointState_)
    - `/_imu/data_raw` (_sensor_msgs/msg/Imu_)
    - `/battery` (_sensor_msgs/BatteryState_)
    - `/range/right_front` (_sensor_msgs/msg/Range_)
    - `/range/left_front` (_sensor_msgs/msg/Range_)
    - `/range/right_rear` (_sensor_msgs/msg/Range_)
    - `/range/left_rear` (_sensor_msgs/msg/Range_)
    - `/button/left` (_std_msgs/msg/Bool_)
    - `/button/right` (_std_msgs/msg/Bool_)

    **Parameters**
    - `servo_enable_power` (_Bool_)
    - `servo_voltage` (_Double_):
        - `5.0`V
        - `6.0`V
        - `7.4`V
        - `8.6`V
    - `servo[0...5]_enable` (*Bool_) e.g. `servo2_enable`
    - `servo[0...5]_period` (*UInt32_) e.g. `servo2_period`

## Command line examples
### Motors driving
```bash
# Go forward
ros2 topic pub /_motors_cmd std_msgs/msg/Float32MultiArray "data: [1.0, 1.0, 1.0, 1.0]"
```

### Servos steering
```bash
# Choose power supply voltage for the servos e.g. 5.0V
ros2 param set /rosbot_stm32_firmware servo_voltage 5.0

# Enable power for the servos
ros2 param set /rosbot_stm32_firmware servo_enable_power true

# Set the control period in microseconds e.g. 20 000us for the servo5
ros2 param set /rosbot_stm32_firmware servo5_period 20000

# Enable PWM output for the servo e.g. for the servo5
ros2 param set /rosbot_stm32_firmware servo5_enable true

# Send duty cycle to the servos
ros2 topic  pub /cmd_ser std_msgs/msg/UInt32MultiArray "data: [0, 0, 0, 0, 0, 2000]"

```

### LED blinking
```bash
# Turn on the left LED
ros2 topic pub /led/left std_msgs/msg/Bool "data: true"

# Turn off the left LED
ros2 topic pub /led/left std_msgs/msg/Bool "data: false"
```