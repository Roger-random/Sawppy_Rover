/*
 * @brief Rover steering by joystick
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
  xQueueHandle xJoyMsgQueue;
  xQueueHandle xCmdVelQueue;
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
