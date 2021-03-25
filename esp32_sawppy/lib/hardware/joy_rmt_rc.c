#include "joy_rmt_rc.h"

#ifdef USE_JOY_RMT_RC

void joy_rmt_rc_read_task(void* pvParameter)
{
  joy_msg  message;

  // Get ready to use caller-allocated queue for communicating joystick data
  QueueHandle_t xJoystickQueue;
  if (NULL == pvParameter)
  {
    printf("ERROR: joy_rmt_rc_read_task parameter is null. Expected handle to joystick data queue.\n");
    vTaskDelete(NULL); // Delete self.
  }
  xJoystickQueue = (QueueHandle_t)pvParameter;

  // Configure RMT peripheral

  // Read loop
  while(true)
  {
    vTaskDelay(pdMS_TO_TICKS(10));
  }
}

#endif // #ifdef USE_JOY_RMT_RC
