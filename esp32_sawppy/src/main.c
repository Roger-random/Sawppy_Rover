#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

#include <joy_msg.h>
#include <twist_msg.h>
#include <wheel_msg.h>
#include <msg_print.h>

#include <joy_adc.h>
#include <joy_steer.h>

void app_main()
{
  QueueHandle_t xJoystickQueue = xQueueCreate(1, sizeof(joy_msg));
  QueueHandle_t xCmdVelQueue = xQueueCreate(1, sizeof(twist_msg));
  QueueHandle_t xWheelQueue = xQueueCreate(1, sizeof(wheel_msg));

  joy_steer_task_parameters joy_steer_params = {
    .xCmdVelQueue = xCmdVelQueue,
    .xJoyMsgQueue = xJoystickQueue,
  };

  if (NULL == xJoystickQueue || NULL == xCmdVelQueue)
  {
    printf("ERROR: Queue allocation failed.");
  }
  else
  {
    xTaskCreate(joy_adc_read_task, "joy_adc_read_task", 2048, xJoystickQueue, 20, NULL);
    //xTaskCreate(joy_msg_print_task, "joy_msg_print_task", 2048, &xJoystickQueue, 25, NULL);
    xTaskCreate(joy_steer_task, "joy_steer_task", 2048, &joy_steer_params, 15, NULL);
    //xTaskCreate(twist_msg_print_task, "twist_msg_print_task", 2048, xCmdVelQueue, 25, NULL);
    xTaskCreate(wheel_msg_print_task, "wheel_msg_print_task", 2048, xWheelQueue, 25, NULL);
  }
}