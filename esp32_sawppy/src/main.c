#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

#include <joy_msg.h>
#include <joy_adc.h>
#include <joy_msg_print.h>

void app_main()
{
  QueueHandle_t xJoystickQueue = NULL;

  xJoystickQueue = xQueueCreate(1, sizeof(joy_msg));
  if (NULL == xJoystickQueue)
  {
    printf("ERROR: Joystick queue allocation failed.");
  }
  else
  {
    xTaskCreate(joy_adc_read_task, "joy_adc_read_task", 2048, xJoystickQueue, 20, NULL);
    xTaskCreate(joy_msg_print_task, "joy_msg_print_task", 2048, xJoystickQueue, 15, NULL);
  }
}