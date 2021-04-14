/*

Message definition to represent wheel steering and driving command
in a FreeRTOS queue

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
