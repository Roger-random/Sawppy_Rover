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

typedef enum {
  axis_steer,
  axis_speed,
  axis_aux,
  axis_count,
} joy_msg_axes;

typedef enum {
  button_mode,
  button_count,
} joy_msg_buttons;

typedef struct xJoystickMessage
{
  TickType_t  timeStamp;
  float       axes[axis_count];
  uint32_t    buttons[button_count];
} joy_msg;

#endif // #ifndef INC_JOY_MSG_H