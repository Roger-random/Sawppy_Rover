/*
 *
 * Message definition to represent wheel steering and driving command
 * in a FreeRTOS queue
 * 
 * Copyright (c) Roger Cheng
 * Released AS-IS under MIT license
 * 
 */

#ifndef INC_WHEEL_MSG_H
#define INC_WHEEL_MSG_H

#include "freertos/FreeRTOS.h"

// Wheel count is available in rover/wheel_parameter.h but architecturally
// the message definitions should not rely on higher level concepts. Is a
// layer inversion better or worse than #define our own here?
#define WHEEL_MSG_DATA_COUNT 6

typedef struct wheel_msg
{
  TickType_t  timeStamp;

  // Desired steering angle for this wheel, in radians.
  float steer[WHEEL_MSG_DATA_COUNT];

  // Desired speed for this wheel, in meters per second.
  float speed[WHEEL_MSG_DATA_COUNT];
} wheel_msg;

#endif // #ifndef INC_WHEEL_MSG_H
