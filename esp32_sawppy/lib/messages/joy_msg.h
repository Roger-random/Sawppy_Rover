/*

Message definition to represent joystick input in a FreeRTOS queue
Inspired by ROS joystick message http://wiki.ros.org/joy

Copyright (c) Roger Cheng

MIT License

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

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