#include "wheels.hpp"

std_msgs__msg__Float32MultiArray wheels_command_msg;

void init_wheels()
{
  odom_watchdog_timer.start();
  last_wheels_speed_calc_time = odom_watchdog_timer.read();
  RosbotWheel custom_wheel_params = { .radius = WHEEL_RADIUS,
                                      .diameter_modificator = DIAMETER_MODIFICATOR,
                                      .tyre_deflation = TYRE_DEFLATION,
                                      .gear_ratio = GEAR_RATIO,
                                      .encoder_cpr = ENCODER_CPR,
                                      .polarity = POLARITY };

  RosbotDrive& drive = RosbotDrive::getInstance();
  drive.setupMotorSequence(MOTOR_FR, MOTOR_FL, MOTOR_RR, MOTOR_RL);
  drive.init(custom_wheel_params, RosbotDrive::DEFAULT_REGULATOR_PARAMS);
  drive.enable(true);
  drive.enablePidReg(true);

  fill_wheels_state_msg(&wheels_state_msg);
  fill_wheels_command_msg(&wheels_command_msg);
  wheels_state_read_thread.start(wheels_state_read_and_watchdog_task);
}

void update_wheels_states()
{
  RosbotDrive& drive = RosbotDrive::getInstance();

  float current_position[MOTORS_COUNT];
  current_position[motor_left_front] = drive.getAngularPos(MOTOR_FL);
  current_position[motor_right_front] = drive.getAngularPos(MOTOR_FR);
  current_position[motor_left_rear] = drive.getAngularPos(MOTOR_RL);
  current_position[motor_right_rear] = drive.getAngularPos(MOTOR_RR);

  const float current_time = odom_watchdog_timer.read();
  const float dt = current_time - last_wheels_speed_calc_time;
  last_wheels_speed_calc_time = current_time;

  for (auto i = 0u; i < MOTORS_COUNT; ++i)
  {
    wheels_state_msg.velocity.data[i] = (current_position[i] - wheels_state_msg.position.data[i]) / dt;
    wheels_state_msg.position.data[i] = current_position[i];
  }
}

void wheels_command_callback(const void* message)
{
  const std_msgs__msg__Float32MultiArray* msg = (const std_msgs__msg__Float32MultiArray*)message;
  if (msg != NULL and msg->data.size == MOTORS_COUNT)
  {
    RosbotDrive& drive = RosbotDrive::getInstance();
    NewTargetSpeed new_speed = {
            .speed = {
                msg->data.data[motor_right_front] * WHEEL_RADIUS,
                msg->data.data[motor_right_rear] * WHEEL_RADIUS,
                msg->data.data[motor_left_rear] * WHEEL_RADIUS,
                msg->data.data[motor_left_front] * WHEEL_RADIUS,
            },
            .mode = MPS
        };

    for (auto i = 0u; i < MOTORS_COUNT; ++i)
    {
      if (isnan(new_speed.speed[i]))
      {
        float zero_speeds[MOTORS_COUNT] = { 0, 0, 0, 0 };
        memcpy(new_speed.speed, zero_speeds, sizeof(new_speed.speed));
        break;
      }
    }

    drive.updateTargetSpeed(new_speed);
    last_speed_command_time = odom_watchdog_timer.read_ms();
    is_speed_watchdog_active = false;
  }
}

void check_speed_watchdog()
{
  if (!is_speed_watchdog_active && (odom_watchdog_timer.read_ms() - last_speed_command_time) > speed_watchdog_interval)
  {
    RosbotDrive& drive = RosbotDrive::getInstance();
    NewTargetSpeed new_speed = { .speed = { 0.0, 0.0, 0.0, 0.0 }, .mode = MPS };
    drive.updateTargetSpeed(new_speed);
    is_speed_watchdog_active = true;
  }
}

void wheels_state_read_and_watchdog_task()
{
  while (true)
  {
    check_speed_watchdog();
    update_wheels_states();
    ThisThread::sleep_for(10);
  }
}

void fill_wheels_state_msg(sensor_msgs__msg__JointState* msg)
{
  static double msg_data_tab[MOTORS_STATE_COUNT][MOTORS_COUNT];
  static rosidl_runtime_c__String msg_name_tab[MOTORS_COUNT];
  msg->header.frame_id = micro_ros_string_utilities_set(msg->header.frame_id, "wheels_state");

  msg->position.data = msg_data_tab[motor_state_position];
  msg->position.capacity = msg->position.size = MOTORS_COUNT;
  msg->velocity.data = msg_data_tab[motor_state_velocity];
  msg->velocity.capacity = msg->velocity.size = MOTORS_COUNT;
  msg->effort.data = msg_data_tab[motor_state_effort];
  msg->effort.capacity = msg->effort.size = MOTORS_COUNT;

  msg_name_tab->capacity = msg_name_tab->size = MOTORS_COUNT;
  msg_name_tab[motor_left_front].data = (char*)FRONT_LEFT_MOTOR_NAME;
  msg_name_tab[motor_right_front].data = (char*)FRONT_RIGHT_MOTOR_NAME;
  msg_name_tab[motor_left_rear].data = (char*)REAR_LEFT_MOTOR_NAME;
  msg_name_tab[motor_right_rear].data = (char*)REAR_RIGHT_MOTOR_NAME;
  for (uint8_t i = 0; i < MOTORS_COUNT; i++)
  {
    msg_name_tab[i].capacity = msg_name_tab[i].size = strlen(msg_name_tab[i].data);
  }
  msg->name.capacity = msg->name.size = MOTORS_COUNT;
  msg->name.data = msg_name_tab;

  if (rmw_uros_epoch_synchronized())
  {
    msg->header.stamp.sec = (int32_t)(rmw_uros_epoch_nanos() / 1000000000);
    msg->header.stamp.nanosec = (uint32_t)(rmw_uros_epoch_nanos() % 1000000000);
  }
}

void fill_wheels_command_msg(std_msgs__msg__Float32MultiArray* msg)
{
  static float data[MOTORS_COUNT] = { 0, 0, 0, 0 };
  msg->data.capacity = MOTORS_COUNT;
  msg->data.size = 0;
  msg->data.data = (float*)data;
}