## ROS2 API
### Subscribes
- `/cmd_ser` (*std_msgs/msg/UInt32MultiArray[6]*, **/rosbot_stm32_firmware**)
- `/led/left` (*std_msgs/msg/Bool*, **/rosbot_stm32_firmware**)
- `/led/right` (*std_msgs/msg/Bool*, **/rosbot_stm32_firmware**)
- `/_motors_cmd` (*std_msgs/msg/Float32MultiArray[4]*, **/rosbot_stm32_firmware**)

### Publishes
- `/_motors_response` (*sensor_msgs/msg/JointState*, **/rosbot_stm32_firmware**)
- `/_imu/data_raw` (*sensor_msgs/msg/Imu*, **/rosbot_stm32_firmware**)
- `/battery` (*sensor_msgs/BatteryState*, **/rosbot_stm32_firmware**)
- `/range/right_front` (*sensor_msgs/msg/Range*, **/rosbot_stm32_firmware**)
- `/range/left_front` (*sensor_msgs/msg/Range*, **/rosbot_stm32_firmware**)
- `/range/right_rear` (*sensor_msgs/msg/Range*, **/rosbot_stm32_firmware**)
- `/range/left_rear` (*sensor_msgs/msg/Range*, **/rosbot_stm32_firmware**)
- `/button/left` (*std_msgs/msg/Bool*, **/rosbot_stm32_firmware**)
- `/button/right` (*std_msgs/msg/Bool*, **/rosbot_stm32_firmware**)

### Parameters
- `servo_enable_power` (*Bool*, **/rosbot_stm32_firmware**)
- `servo_voltage` (*Double*, **/rosbot_stm32_firmware**):
    - `5.0`V
    - `6.0`V
    - `7.4`V
    - `8.6`V
- `servo[0...5]_enable` (*Bool*, **/rosbot_stm32_firmware**) e.g. `servo2_enable`
- `servo[0...5]_period` (*UInt32*, **/rosbot_stm32_firmware**) e.g. `servo2_period`

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