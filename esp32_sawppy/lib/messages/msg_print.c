/*

Debug tool: Tasks to peek a message queue and print current value

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
#include "msg_print.h"

static const char *TAG_JOY = "joy_msg";
static const char *TAG_TWIST = "twist_msg";
static const char *TAG_WHEEL = "wheel_msg";

void joy_msg_print_task(void* pvParameter)
{
  joy_msg message;
  QueueHandle_t xJoystickQueue;
  if (NULL == pvParameter)
  {
    ESP_LOGE(TAG_JOY, "Task parameter is null. Expected handle to joystick data queue.");
    vTaskDelete(NULL); // Delete self.
  }
  xJoystickQueue = (QueueHandle_t)pvParameter;

  while(true)
  {
    if (pdTRUE == xQueuePeek(xJoystickQueue, &message, portMAX_DELAY))
    {
      ESP_LOGI(TAG_JOY, "%d | steer %+.2f | speed %+.2f | button %d",
        message.timeStamp,
        message.axes[axis_steer],
        message.axes[axis_speed],
        message.buttons[button_mode]);
    }
    else
    {
      // Since timeout is set to portMAX_DELAY, not sure when this would possibly happen.
      ESP_LOGE(TAG_JOY, "Failed to peek joystick queue data.");
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
    ESP_LOGE(TAG_TWIST, "Task parameter is null. Expected handle to twist message queue");
    vTaskDelete(NULL); // Delete self.
  }
  xTwistQueue = (QueueHandle_t)pvParameter;

  while(true)
  {
    if (pdTRUE == xQueuePeek(xTwistQueue, &message, portMAX_DELAY))
    {
      ESP_LOGI(TAG_TWIST, "%d | linear %+.2f,%+.2f,%+.2f | angular %+.2f,%+.2f,%+.2f",
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
      ESP_LOGE(TAG_TWIST, "Failed to peek twist queue data.");
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
    ESP_LOGE(TAG_WHEEL, "Task parameter is null. Expected handle to wheel message queue");
    vTaskDelete(NULL); // Delete self.
  }
  xWheelQueue = (QueueHandle_t)pvParameter;

  while(true)
  {
    if (pdTRUE == xQueuePeek(xWheelQueue, &message, portMAX_DELAY))
    {
      printf("wheel_msg %d",
        message.timeStamp);
      for (int wheel = 0; wheel < WHEEL_MSG_DATA_COUNT; wheel++)
      {
        printf(" | %+.2f rad %+.2f m/s", message.steer[wheel], message.speed[wheel]);
      }
      printf("\n");
    }
    else
    {
      // Since timeout is set to portMAX_DELAY, not sure when this would possibly happen.
      ESP_LOGE(TAG_WHEEL, "Task failed to peek wheel queue data.\n");
    }

    // Wait before we perform the next queue peek
    vTaskDelay(print_interval);
  }
}
