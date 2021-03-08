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

/*
 * @brief FreeRTOS task which reads joy_msg and generates velocity command
 * @param Instance of joy_steer_task_parameters filled with queue handles
 */
void joy_steer_task(void*);

#endif // #ifndef INC_JOY_STEER_H
