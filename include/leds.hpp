#pragma once
#include <mbed.h>
#include <std_msgs/msg/bool.h>

enum LEDs
{
  led_left,
  led_right,
  LED_COUNT
};

static DigitalOut led2(LED2, 0);
static DigitalOut led3(LED3, 0);

void led1_callback(const void* message);
void led2_callback(const void* message);
