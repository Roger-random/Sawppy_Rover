/*
 *
 * Message definition to represent joystick input in a FreeRTOS queue
 * Inspired by ROS joystick message http://wiki.ros.org/joy
 * 
 * Copyright (c) Roger Cheng
 * Released AS-IS under MIT license
 * 
 */

#ifndef INC_JOY_MSG_H
#define INC_JOY_MSG_H

#include "freertos/FreeRTOS.h"

static const uint32_t axis_steer = 0;
static const uint32_t axis_speed = 1;
#define axis_count 2

static const uint32_t button_mode = 0;
#define button_count 1

typedef struct xJoystickMessage
{
  TickType_t  timeStamp;
  float       axes[axis_count];
  uint32_t    buttons[button_count];
} joy_msg;

#endif // #ifndef INC_JOY_MSG_H