#pragma once

#include <mbed.h>

#include <std_msgs/msg/bool.h>

enum Buttons
{
  button_left,
  button_right,
  BUTTONS_COUNT
};



static volatile bool buttons_publish_flag[BUTTONS_COUNT] = { false, false };
static std_msgs__msg__Bool button_msgs[BUTTONS_COUNT];
void init_button_and_attach_to_callbacks(mbed::InterruptIn *interrupt, void (*rise)(), void (*fall)());
