#include <microros.hpp>

rclc_executor_t executor;
rclc_support_t support;
rcl_allocator_t rcl_allocator;
rcl_node_t node;
rcl_timer_t timer;
rcl_timer_t range_timer;


rcl_publisher_t imu_pub;
rcl_publisher_t wheels_state_pub;
rcl_publisher_t battery_pub;
rcl_publisher_t range_pubs[RANGES_COUNT];
rcl_publisher_t buttons_pubs[BUTTONS_COUNT];

rcl_subscription_t wheels_command_sub;
rcl_subscription_t servos_command_sub;
rcl_subscription_t led_subs[LED_COUNT];

rclc_parameter_server_t param_server;

std_msgs__msg__Float32MultiArray wheels_command_msg;
std_msgs__msg__UInt32MultiArray servos_command_msg;
std_msgs__msg__Bool led_msg;

Parameters microros_parameters;

const char *range_frame_names[] = {"range_right_front", "range_left_front", "range_right_rear", "range_left_rear"};
const char *range_pub_names[] = {"range/right_front", "range/left_front", "range/right_rear", "range/left_rear"};
const char *buttons_pub_names[] = {"button/left", "button/right"};
const char *led_subs_names[] = {"led/left", "led/right"};

extern void timer_callback(rcl_timer_t *timer, int64_t last_call_time);
extern void wheels_command_callback(const void *msgin);
extern void servos_command_callback(const void *msgin);
extern void led1_callback(const void *msgin);
extern void led2_callback(const void *msgin);

extern void publish_range_sensors(rcl_timer_t *timer, int64_t last_call_time);
extern bool on_parameter_changed(const Parameter * old_param, const Parameter * new_param, void * context);


bool microros_init() {
    fill_wheels_command_msg(&wheels_command_msg);
    fill_servos_command_msg(&servos_command_msg);

    rcl_allocator = rcl_get_default_allocator();
    RCCHECK(rclc_support_init(&support, 0, NULL, &rcl_allocator));
    RCCHECK(rclc_node_init_default(&node, NODE_NAME, "", &support));
    if (not init_wheels_command_subscriber() or
        not init_servos_command_subscriber() or
        not init_wheels_state_publisher() or
        not init_imu_publisher() or
        not init_battery_publisher() or
        not init_range_publishers() or
        not init_button_publishers() or
        not init_led_subscribers() or
        not init_param_server() or
        not init_parameters()) {
        return false;
    }

    RCCHECK(rclc_timer_init_default(&timer, &support, RCL_MS_TO_NS(10),
                                    timer_callback));
    RCCHECK(rclc_timer_init_default(&range_timer, &support, RCL_MS_TO_NS( 200  ),
                                    publish_range_sensors));

    RCCHECK(rclc_executor_init(&executor, &support.context, 12, &rcl_allocator));
    RCCHECK(rclc_executor_add_timer(&executor, &timer));
    RCCHECK(rclc_executor_add_timer(&executor, &range_timer));

    RCCHECK(rclc_executor_add_subscription(&executor, &wheels_command_sub, &wheels_command_msg,
                                           &wheels_command_callback, ON_NEW_DATA));
    RCCHECK(rclc_executor_add_subscription(&executor, &servos_command_sub, &servos_command_msg,
                                           &servos_command_callback, ON_NEW_DATA));
    RCCHECK(rclc_executor_add_subscription(&executor, &led_subs[0], &led_msg,
                                           &led1_callback, ON_NEW_DATA));
    RCCHECK(rclc_executor_add_subscription(&executor, &led_subs[1], &led_msg,
                                           &led2_callback, ON_NEW_DATA));
    RCCHECK(rclc_executor_add_parameter_server(&executor, &param_server, on_parameter_changed));

    RCCHECK(rclc_executor_prepare(&executor));
    RCCHECK(rmw_uros_sync_session(1000));
    return true;
}

bool microros_deinit() {
    rmw_context_t *rmw_context = rcl_context_get_rmw_context(&support.context);
    (void)rmw_uros_set_context_entity_destroy_session_timeout(rmw_context, 0);

    RCCHECK(rcl_subscription_fini(&wheels_command_sub, &node));
    RCCHECK(rcl_publisher_fini(&wheels_state_pub, &node));
    RCCHECK(rcl_publisher_fini(&imu_pub, &node));
    RCCHECK(rcl_publisher_fini(&battery_pub, &node));
    for (auto i = 0u; i < RANGES_COUNT; ++i) {
        RCCHECK(rcl_publisher_fini(&range_pubs[i], &node));
    }

    for (auto i = 0u; i < BUTTONS_COUNT; ++i) {
        RCCHECK(rcl_publisher_fini(&buttons_pubs[i], &node));
    }

    RCCHECK(rclc_parameter_server_fini(&param_server, &node));
    RCCHECK(rclc_executor_fini(&executor));
    RCCHECK(rcl_node_fini(&node));
    return true;
}

bool microros_spin() {
    RCCHECK(rclc_executor_spin_some(&executor, RCL_MS_TO_NS(100)));
    return true;
}

bool init_imu_publisher() {
    RCCHECK(rclc_publisher_init_best_effort(
        &imu_pub,
        &node,
        ROSIDL_GET_MSG_TYPE_SUPPORT(sensor_msgs, msg, Imu),
        IMU_TOPIC_NAME));
    return true;
}

bool init_wheels_state_publisher() {
    RCCHECK(rclc_publisher_init_best_effort(
        &wheels_state_pub,
        &node,
        ROSIDL_GET_MSG_TYPE_SUPPORT(sensor_msgs, msg, JointState),
        WHEELS_STATE_TOPIC_NAME));
    return true;
}

bool init_battery_publisher() {
    RCCHECK(rclc_publisher_init_default(
        &battery_pub,
        &node,
        ROSIDL_GET_MSG_TYPE_SUPPORT(sensor_msgs, msg, BatteryState),
        BATTERY_TOPIC_NAME));
    return true;
}

bool init_range_publishers() {
    for (auto i = 0u; i < RANGES_COUNT; ++i) {
        RCCHECK(rclc_publisher_init_best_effort(
            &range_pubs[i],
            &node,
            ROSIDL_GET_MSG_TYPE_SUPPORT(sensor_msgs, msg, Range),
            range_pub_names[i]));
    }
    return true;
}

bool init_button_publishers() {
    for (auto i = 0u; i < BUTTONS_COUNT; ++i) {
        RCCHECK(rclc_publisher_init_best_effort(
            &buttons_pubs[i],
            &node,
            ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Bool),
            buttons_pub_names[i]));
    }
    return true;
}

bool init_wheels_command_subscriber() {
    RCCHECK(rclc_subscription_init_best_effort(
        &wheels_command_sub, &node, ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Float32MultiArray),
        WHEELS_COMMAND_TOPIC_NAME));
    return true;
}

bool init_servos_command_subscriber(){
    RCCHECK(rclc_subscription_init_best_effort(
        &servos_command_sub, &node, ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, UInt32MultiArray),
        SERVOS_COMMAND_TOPIC_NAME));
    return true;
}

bool init_led_subscribers(){
    for(auto i = 0u; i < LED_COUNT; ++i){
        RCCHECK(rclc_subscription_init_best_effort(
            &led_subs[i], &node, ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Bool),
            led_subs_names[i]));
    }
    return true;
}

bool init_param_server(){
    const rclc_parameter_options_t options = {
        .notify_changed_over_dds = true,
        .max_params = 15
    };
    RCCHECK(rclc_parameter_server_init_with_option(&param_server, &node, &options));
    return true;
}

bool init_parameters(){
    RCCHECK(rclc_add_parameter(&param_server, "servo_voltage", RCLC_PARAMETER_DOUBLE));
    RCCHECK(rclc_parameter_set_double(&param_server, "servo_voltage", 5.0));

    RCCHECK(rclc_add_parameter(&param_server, "servo_enable_power", RCLC_PARAMETER_BOOL));
    RCCHECK(rclc_parameter_set_bool(&param_server, "servo_enable_power", false));
    constexpr int BUFF = 20;
    char param_name[BUFF];
    for(auto i = 0u; i < SERVOS_COUNT; ++i){
        snprintf(param_name, BUFF, "servo%d_enable", i);
        RCCHECK(rclc_add_parameter(&param_server, param_name, RCLC_PARAMETER_BOOL));
        RCCHECK(rclc_parameter_set_bool(&param_server, param_name, false));

        snprintf(param_name, BUFF, "servo%d_period", i);
        RCCHECK(rclc_add_parameter(&param_server, param_name, RCLC_PARAMETER_INT));
        RCCHECK(rclc_parameter_set_int(&param_server, param_name, 0));
    }
    return true;
}

bool publish_wheels_state_msg(sensor_msgs__msg__JointState *msg) {
    RCCHECK(rcl_publish(&wheels_state_pub, msg, NULL));
    return true;
}

bool publish_imu_msg(sensor_msgs__msg__Imu *msg) {
    RCCHECK(rcl_publish(&imu_pub, msg, NULL));
    return true;
}

bool publish_battery_msg(sensor_msgs__msg__BatteryState *msg) {
    RCCHECK(rcl_publish(&battery_pub, msg, NULL));
    return true;
}

bool publish_range_msg(sensor_msgs__msg__Range *msg, uint8_t id) {
    RCCHECK(rcl_publish(&range_pubs[id], msg, NULL));
    return true;
}

bool publish_button_msg(std_msgs__msg__Bool *msg, uint8_t id) {
    RCCHECK(rcl_publish(&buttons_pubs[id], msg, NULL));
    return true;
}

void fill_wheels_state_msg(sensor_msgs__msg__JointState *msg) {
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
    msg_name_tab[motor_left_front].data = (char *)FRONT_LEFT_MOTOR_NAME;
    msg_name_tab[motor_right_front].data = (char *)FRONT_RIGHT_MOTOR_NAME;
    msg_name_tab[motor_left_rear].data = (char *)REAR_LEFT_MOTOR_NAME;
    msg_name_tab[motor_right_rear].data = (char *)REAR_RIGHT_MOTOR_NAME;
    for (uint8_t i = 0; i < MOTORS_COUNT; i++) {
        msg_name_tab[i].capacity = msg_name_tab[i].size = strlen(msg_name_tab[i].data);
    }
    msg->name.capacity = msg->name.size = MOTORS_COUNT;
    msg->name.data = msg_name_tab;

    if (rmw_uros_epoch_synchronized()) {
        msg->header.stamp.sec = (int32_t)(rmw_uros_epoch_nanos() / 1000000000);
        msg->header.stamp.nanosec = (uint32_t)(rmw_uros_epoch_nanos() % 1000000000);
    }
}

void fill_imu_msg(sensor_msgs__msg__Imu *msg) {
    msg->header.frame_id = micro_ros_string_utilities_set(msg->header.frame_id, "imu");

    if (rmw_uros_epoch_synchronized()) {
        msg->header.stamp.sec = (int32_t)(rmw_uros_epoch_nanos() / 1000000000);
        msg->header.stamp.nanosec = (uint32_t)(rmw_uros_epoch_nanos() % 1000000000);
    }
    msg->orientation.x = 0;
    msg->orientation.y = 0;
    msg->orientation.z = 0;
    msg->orientation.w = 1;
    msg->angular_velocity.x = 0;
    msg->angular_velocity.y = 0;
    msg->angular_velocity.z = 0;
    msg->linear_acceleration.x = 0;
    msg->linear_acceleration.y = 0;
    msg->linear_acceleration.z = 0;
    for (auto i = 0u; i < 9u; ++i) {
        msg->angular_velocity_covariance[i] = msg->linear_acceleration_covariance[i] = msg->orientation_covariance[i] = 0.0;
    }
    msg->orientation_covariance[9] = 0.0;
    msg->orientation_covariance[10] = 0.0;
    msg->orientation_covariance[11] = 0.0;
}

void fill_battery_msg(sensor_msgs__msg__BatteryState *msg) {
    msg->header.frame_id = micro_ros_string_utilities_set(msg->header.frame_id, "battery");

    if (rmw_uros_epoch_synchronized()) {
        msg->header.stamp.sec = (int32_t)(rmw_uros_epoch_nanos() / 1000000000);
        msg->header.stamp.nanosec = (uint32_t)(rmw_uros_epoch_nanos() % 1000000000);
    }

    msg->power_supply_technology = sensor_msgs__msg__BatteryState__POWER_SUPPLY_TECHNOLOGY_LION;
}

void fill_wheels_command_msg(std_msgs__msg__Float32MultiArray *msg) {
    static float data[MOTORS_COUNT] = {0, 0, 0, 0};
    msg->data.capacity = MOTORS_COUNT;
    msg->data.size = 0;
    msg->data.data = (float *)data;
}

void fill_servos_command_msg(std_msgs__msg__UInt32MultiArray *msg) {
    static uint32_t data[SERVOS_COUNT] = {0, 0, 0, 0, 0, 0};
    msg->data.capacity = SERVOS_COUNT;
    msg->data.size = 0;
    msg->data.data = (uint32_t *)data;
}

void fill_range_msg(sensor_msgs__msg__Range *msg, uint8_t id) {
    msg->header.frame_id = micro_ros_string_utilities_set(msg->header.frame_id, range_frame_names[id]);

    if (rmw_uros_epoch_synchronized()) {
        msg->header.stamp.sec = (int32_t)(rmw_uros_epoch_nanos() / 1000000000);
        msg->header.stamp.nanosec = (uint32_t)(rmw_uros_epoch_nanos() % 1000000000);
    }

    msg->radiation_type = sensor_msgs__msg__Range__INFRARED;
    msg->field_of_view = 0.26;
    msg->min_range = 0.01;
    msg->max_range = 0.90;
}