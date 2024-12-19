/*

Convert raw joystick input represented by joy_msg into a rover velocity
command in the form of twist_msg, taking into account rover geometry and
human friendliness. Experience indicated most humans are get confused with
a chassis more maneuverable than most cars, so we keep things constrained.

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
#ifndef INC_JOY_STEER_H
#define INC_JOY_STEER_H
// C language headers
#include <math.h>

// FreeRTOS headers
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

// ESP32 headers
#include "esp_log.h"

// Headers from project libraries
#include <joy_msg.h>
#include <twist_msg.h>

// Local headers
#include "wheel_parameter.h"

/*
 * @brief Parameter for joy_steer_task
 */
typedef struct joy_steer_task_parameters
{
  QueueHandle_t xJoyMsgQueue;
  QueueHandle_t xCmdVelQueue;
} joy_steer_task_parameters;

/*
 * @brief Ticks to wait for joystick message before error message
 */
static const TickType_t joy_msg_timeout_interval = pdMS_TO_TICKS(3000);

/*
 * @brief Joystick steering input minimum. Less than this is treated as zero.
 */
static const float steering_epsilon = 0.01;

/*
 * @brief Joystick speed input minimum. Less than this is treated as stopped.
 */
static const float speed_epsilon = 0.01;

// From a system architecture point of view, it should be the joystick driver's
// responsibility to ensure correct axis alignment. (Positive steer axis is forward
// speed, etc.) In practice we won't necessarily be able to edit joystick
// driver so we'll include a provision at this layer to invert joystick axis.

/*
 * @brief True if positive value in steering axis is negative rotation about Z axis by right-hand rule
 */
static const bool joy_steer_invert = false;

/*
 * @brief True if positive value in speed axis is rover traveling backwards.
 */
static const bool joy_speed_invert = false;

/*
 * @brief FreeRTOS task which reads joy_msg and generates velocity command
 * @param Instance of joy_steer_task_parameters filled with queue handles
 */
void joy_steer_task(void*);

#endif // #ifndef INC_JOY_STEER_H
