/*
 * @brief Rover wheel Ackermann calculations
 * https://en.wikipedia.org/wiki/Ackermann_steering_geometry
 *
 * Lengths in meters, angles in radians, coordinates as per REP103
 * https://www.ros.org/reps/rep-0103.html
 *
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
