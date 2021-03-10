#include "msg_print.h"

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
    if (pdTRUE == xQueuePeek(xJoystickQueue, &message, portMAX_DELAY))
    {
      printf("joy_msg_print_task - time %d - steer %+.2f - speed %+.2f - button %d\n", 
        message.timeStamp,
        message.axes[axis_steer],
        message.axes[axis_speed],
        message.buttons[button_mode]);
    }
    else
    {
      // Since timeout is set to portMAX_DELAY, not sure when this would possibly happen.
      printf("joy_msg_print_task failed to peek joystick queue data.\n");
    }

    // Wait before we perform the next queue peek
    vTaskDelay(print_interval);
  }
}


void twist_msg_print_task(void* pvParameter)
{
  twist_msg message;
  QueueHandle_t xTwistQueue;
  if (NULL == pvParameter)
  {
    printf("ERROR: twist_msg_print_task parameter is null. Expected handle to twist message queue\n");
    vTaskDelete(NULL); // Delete self.
  }
  xTwistQueue = (QueueHandle_t)pvParameter;

  while(true)
  {
    if (pdTRUE == xQueuePeek(xTwistQueue, &message, portMAX_DELAY))
    {
      printf("twist_msg_print_task - time %d - linear %+.2f,%+.2f,%+.2f - angular %+.2f,%+.2f,%+.2f\n",
        message.timeStamp,
        message.linear.x,
        message.linear.y,
        message.linear.z,
        message.angular.x,
        message.angular.y,
        message.angular.z);
    }
    else
    {
      // Since timeout is set to portMAX_DELAY, not sure when this would possibly happen.
      printf("twist_msg_print_task failed to peek twist queue data.\n");
    }

    // Wait before we perform the next queue peek
    vTaskDelay(print_interval);
  }
}

void wheel_msg_print_task(void* pvParameter)
{
  wheel_msg message;
  QueueHandle_t xWheelQueue;
  if (NULL == pvParameter)
  {
    printf("ERROR: wheel_msg_print_task parameter is null. Expected handle to wheel message queue\n");
    vTaskDelete(NULL); // Delete self.
  }
  xWheelQueue = (QueueHandle_t)pvParameter;

  while(true)
  {
    if (pdTRUE == xQueuePeek(xWheelQueue, &message, portMAX_DELAY))
    {
      printf("wheel_msg_print_task - time %d",
        message.timeStamp);
      for (int wheel = 0; wheel < WHEEL_MSG_DATA_COUNT; wheel++)
      {
        printf(" %+.2f rad %.2f m/s", message.steer[wheel], message.speed[wheel]);
      }
      printf("\n");
    }
    else
    {
      // Since timeout is set to portMAX_DELAY, not sure when this would possibly happen.
      printf("wheel_msg_print_task failed to peek wheel queue data.\n");
    }

    // Wait before we perform the next queue peek
    vTaskDelay(print_interval);
  }
}
