#include <microros.hpp>
#include <std_srvs/srv/trigger.h>

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

rcl_service_t get_cpu_id_service;

rclc_parameter_server_t param_server;

std_msgs__msg__Bool led_msg;

std_srvs__srv__Trigger_Request get_cpu_id_service_request;
std_srvs__srv__Trigger_Response get_cpu_id_service_response;

const char *range_pub_names[] = {"range/fr", "range/fl", "range/rr", "range/rl"};
const char *buttons_pub_names[] = {"button/left", "button/right"};
const char *led_subs_names[] = {"led/left", "led/right"};

extern void timer_callback(rcl_timer_t *timer, int64_t last_call_time);

extern void publish_range_sensors(rcl_timer_t *timer, int64_t last_call_time);
extern bool on_parameter_changed(const Parameter * old_param, const Parameter * new_param, void * context);

bool microros_init() {
    rcl_allocator = rcl_get_default_allocator();

    rcl_init_options_t init_options = rcl_get_zero_initialized_init_options();
    RCCHECK(rcl_init_options_init(&init_options, rcl_allocator));
    RCCHECK(rcl_init_options_set_domain_id(&init_options, UXR_CLIENT_DOMAIN_ID_TO_OVERRIDE_WITH_ENV));
    RCCHECK(rclc_support_init_with_options(&support, 0, NULL, &init_options, &rcl_allocator));

    RCCHECK(rclc_node_init_default(&node, NODE_NAME, "", &support));

    if (not init_wheels_command_subscriber() or
        not init_wheels_state_publisher() or
        not init_servos_command_subscriber() or
        not init_imu_publisher() or
        not init_battery_publisher() or
        not init_range_publishers() or
        not init_button_publishers() or
        not init_led_subscribers() or
        /*not init_param_server() or
        not init_parameters() or*/
        not init_services()) {
        return false;
    }

    RCCHECK(rclc_timer_init_default(&timer, &support, RCL_MS_TO_NS(10),
                                    timer_callback));
    RCCHECK(rclc_timer_init_default(&range_timer, &support, RCL_MS_TO_NS( 200  ),
                                    publish_range_sensors));

    RCCHECK(rclc_executor_init(&executor, &support.context, 11, &rcl_allocator));
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
    // RCCHECK(rclc_executor_add_parameter_server(&executor, &param_server, on_parameter_changed));
    RCCHECK(rclc_executor_add_service(&executor, &get_cpu_id_service, &get_cpu_id_service_request, &get_cpu_id_service_response, get_cpu_id_service_callback)); 

    RCCHECK(rclc_executor_prepare(&executor));
    RCCHECK(rmw_uros_sync_session(1000));
    return true;
}

bool microros_deinit() {
    rmw_context_t *rmw_context = rcl_context_get_rmw_context(&support.context);
    (void)rmw_uros_set_context_entity_destroy_session_timeout(rmw_context, 0);

    RCCHECK(rcl_subscription_fini(&wheels_command_sub, &node));
    RCCHECK(rcl_subscription_fini(&servos_command_sub, &node));
    for (auto i = 0u; i < LED_COUNT; ++i) {
        RCCHECK(rcl_subscription_fini(&led_subs[i], &node));
    }
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
        RCCHECK(rclc_publisher_init_default(
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
        .max_params = 14
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

bool init_services() {
    std_srvs__srv__Trigger_Request__init(&get_cpu_id_service_request);
    std_srvs__srv__Trigger_Response__init(&get_cpu_id_service_response);

    RCCHECK(rclc_service_init_default(
        &get_cpu_id_service,
        &node,
        ROSIDL_GET_SRV_TYPE_SUPPORT(std_srvs, srv, Trigger),
        GET_CPU_ID_SERVICE_NAME
    ));

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

void get_cpu_id_service_callback(const void *req, void *res) {
    (void)req; // Unused parameter

    const uint32_t ADDRESS = 0x1FFF7A10;
    const uint8_t NUM_BYTES = 12;
    uint8_t buffer[NUM_BYTES];
    memcpy(buffer, (void *)ADDRESS, NUM_BYTES);

    // Prepare the CPU ID in hexadecimal format
    char cpu_id_buffer[NUM_BYTES * 2 + 1] = {0};
    char *hex_ptr = cpu_id_buffer;
    for (uint8_t i = 0; i < NUM_BYTES; ++i) {
        snprintf(hex_ptr, 3, "%02X", buffer[i]);
        hex_ptr += 2;
    }

    // Prepare the final output buffer with "CPU ID: " prefix
    static char out_buffer[100]; // Ensure this is large enough
    snprintf(out_buffer, sizeof(out_buffer), "{\"cpu_id\": \"%s\"}", cpu_id_buffer);

    // Set the response
    std_srvs__srv__Trigger_Response *response = (std_srvs__srv__Trigger_Response *)res;
    response->success = true;
    response->message.data = out_buffer;
    response->message.size = strlen(out_buffer);
}
