#include "wheel_ackermann.h"

void wheel_ackermann_task(void* pvParameter)
{
  twist_msg cmdVelData;
  wheel_msg wheelData;

  // Initialize wheel data output message
  wheelData.timeStamp = xTaskGetTickCount();
  for (int wheel = 0; wheel < wheel_count; wheel++)
  {
    wheelData.steer[wheel] = 0;
    wheelData.speed[wheel] = 0;
  }

  // Verify input parameters are not null.
  wheel_ackermann_task_parameters* pTaskParameters = (wheel_ackermann_task_parameters*) pvParameter;
  if (NULL == pTaskParameters ||
      NULL == pTaskParameters->xCmdVelQueue ||
      NULL == pTaskParameters->xWheelQueue)
  {
    printf("ERROR: wheel_ackermann_task parameters were missing, exiting.\n");
    vTaskDelete(NULL); // Delete self.
  }
  QueueHandle_t xCmdVelQueue = pTaskParameters->xCmdVelQueue;
  QueueHandle_t xWheelQueue = pTaskParameters->xWheelQueue;

  while(true)
  {
    // Wait for next velocity command
    if (pdTRUE == xQueueReceive(xCmdVelQueue, &cmdVelData, twist_msg_timeout_interval))
    {

      wheelData.timeStamp = xTaskGetTickCount();
      xQueueOverwrite(xWheelQueue, &wheelData);
    }
    else
    {
      printf("ERROR: wheel_ackermann_task timed out waiting for command velocity message. Continuing to wait...\n");
    }
  }
}
