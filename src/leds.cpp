#include "leds.hpp"

void led1_callback(const void* msgin)
{
  const std_msgs__msg__Bool* msg = (const std_msgs__msg__Bool*)msgin;
  if (msg != NULL)
  {
    led2 = msg->data;
  }
}

void led2_callback(const void* msgin)
{
  const std_msgs__msg__Bool* msg = (const std_msgs__msg__Bool*)msgin;
  if (msg != NULL)
  {
    led3 = msg->data;
  }
}