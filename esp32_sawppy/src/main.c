#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

#include <joy_msg.h>
#include <twist_msg.h>
#include <wheel_msg.h>
#include <msg_print.h>

#include "gpio_assignments.h"

#ifdef USE_JOY_ADC
#include <joy_adc.h>
#endif
#ifdef USE_JOY_RMT_RC
#include <joy_rmt_rc.h>
#endif

#include <joy_steer.h>
#include <wheel_ackermann.h>
#include <servo_steer_ledc.h>
#include <drv8833_mcpwm.h>

void app_main()
{
  QueueHandle_t xJoystickQueue = xQueueCreate(1, sizeof(joy_msg));
  QueueHandle_t xCmdVelQueue = xQueueCreate(1, sizeof(twist_msg));
  QueueHandle_t xWheelQueue = xQueueCreate(1, sizeof(wheel_msg));

  joy_steer_task_parameters joy_steer_params = {
    .xJoyMsgQueue = xJoystickQueue,
    .xCmdVelQueue = xCmdVelQueue,
  };

  wheel_ackermann_task_parameters wheel_ackermann_params = {
    .xCmdVelQueue = xCmdVelQueue,
    .xWheelQueue = xWheelQueue,
  };

  if (NULL == xJoystickQueue || NULL == xCmdVelQueue || NULL == xWheelQueue)
  {
    printf("ERROR: Queue allocation failed.");
  }
  else
  {
#ifdef USE_JOY_ADC
    xTaskCreate(joy_adc_read_task, "joy_adc_read_task", 2048, xJoystickQueue, 20, NULL);
#endif
#ifdef USE_JOY_RMT_RC
    xTaskCreate(joy_rmt_rc_read_task, "joy_rmt_rc_read_task", 2048, xJoystickQueue, 20, NULL);
#endif
    //xTaskCreate(joy_msg_print_task, "joy_msg_print_task", 2048, xJoystickQueue, 25, NULL);
    xTaskCreate(joy_steer_task, "joy_steer_task", 2048, &joy_steer_params, 15, NULL);
    //xTaskCreate(twist_msg_print_task, "twist_msg_print_task", 2048, xCmdVelQueue, 25, NULL);
    xTaskCreate(wheel_ackermann_task, "wheel_ackermann_task", 2048, &wheel_ackermann_params, 14, NULL);
    //xTaskCreate(wheel_msg_print_task, "wheel_msg_print_task", 2048, xWheelQueue, 25, NULL);
    xTaskCreate(servo_steer_ledc_task, "servo_steer_ledc_task", 2048, xWheelQueue, 13, NULL);
    xTaskCreate(drv8833_mcpwm_task, "drv8833_mcpwm_task", 2048, xWheelQueue, 12, NULL);
  }
}
