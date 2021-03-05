#include "joy_msg_print.h"

void joy_msg_print_task(void* pvParameter)
{
  joy_msg message;
  QueueHandle_t xJoystickQueue;
  if (NULL == pvParameter)
  {
    printf("ERROR: joy_msg_print_task parameter is null. Expected handle to joystick data queue.\n");
    vTaskDelete(NULL); // Delete self.
  }
  xJoystickQueue = (QueueHandle_t)pvParameter;

  while(true)
  {
    if (pdTRUE == xQueuePeek(xJoystickQueue, &message, 1000))
    {
      printf("joy_msg_print_task - time %d - steer %+.2f - speed %+.2f - button %d\n", 
        message.timeStamp,
        message.axes[axis_steer],
        message.axes[axis_speed],
        message.buttons[button_mode]);
    }
    else
    {
      printf("joy_msg_print_task failed to peek joystick queue data.\n");
    }
    vTaskDelay(print_interval);
  }
}
