/*
 *
 * Message definition to represent velocity command in a FreeRTOS queue
 * Inspired by ROS Twist message for /cmd_vel topic
 * 
 * Copyright (c) Roger Cheng
 * Released AS-IS under MIT license
 * 
 */

#ifndef INC_TWIST_MSG_H
#define INC_TWIST_MSG_H

#include "freertos/FreeRTOS.h"

typedef struct vector3
{
  float x;
  float y;
  float z;
} vector3;

typedef struct twist_msg
{
  TickType_t  timeStamp;
  vector3     linear;
  vector3     angular;
} twist_msg;

#endif // #ifndef INC_TWIST_MSG_H