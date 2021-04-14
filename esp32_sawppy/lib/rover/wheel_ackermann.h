/*

Rover wheel ackermann calculations

Reference: https://en.wikipedia.org/wiki/Ackermann_steering_geometry

Also see Jupyter notebook: https://github.com/Roger-random/Sawppy_Rover/blob/main/jupyter/Sawppy%20Ackermann%20Math.ipynb

Lengths in meters, angles in radians, coordinates as per REP103
https://www.ros.org/reps/rep-0103.html

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
#ifndef INC_WHEEL_ACKERMANN_H
#define INC_WHEEL_ACKERMANN_H

// C language headers
#include <math.h>

// FreeRTOS headers
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

// ESP32 headers
#include "esp_log.h"

// Headers from project libraries
#include <twist_msg.h>
#include <wheel_msg.h>

// Local headers
#include "wheel_parameter.h"

/*
 * @brief Parameter for wheel_ackermann_task
 */
typedef struct wheel_ackermann_task_parameters
{
  xQueueHandle xCmdVelQueue;
  xQueueHandle xWheelQueue;
} wheel_ackermann_task_parameters;

/*
 * @brief Ticks to wait for command evlocity message before error message
 */
static const TickType_t twist_msg_timeout_interval = pdMS_TO_TICKS(3000);

/*
 * @brief FreeRTOS task which reads twist_msg and generates wheel_msg
 * @param Instance of wheel_ackermann_task_parameters filled with queue handles
 */
void wheel_ackermann_task(void*);

#endif // #ifndef INC_WHEEL_ACKERMANN_H
