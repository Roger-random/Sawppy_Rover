#include "wheel_ackermann.h"

static const char *TAG = "wheel_ackermann";

/*
 * @brief set all steering angle and speeds to zero
 */
void wheel_msg_reset(wheel_msg* pwheel_msg)
{
  for (int wheel = 0; wheel < WHEEL_MSG_DATA_COUNT; wheel++)
  {
    pwheel_msg->steer[wheel] = 0;
    pwheel_msg->speed[wheel] = 0;
  }
}

void wheel_ackermann_task(void* pvParameter)
{
  twist_msg cmdVelData;
  wheel_msg wheelData;

  // Initialize wheel data output message
  wheelData.timeStamp = xTaskGetTickCount();
  wheel_msg_reset(&wheelData);

  // Verify input parameters are not null.
  wheel_ackermann_task_parameters* pTaskParameters = (wheel_ackermann_task_parameters*) pvParameter;
  if (NULL == pTaskParameters ||
      NULL == pTaskParameters->xCmdVelQueue ||
      NULL == pTaskParameters->xWheelQueue)
  {
    ESP_LOGE(TAG, "Task parameters were missing, exiting.");
    vTaskDelete(NULL); // Delete self.
  }
  QueueHandle_t xCmdVelQueue = pTaskParameters->xCmdVelQueue;
  QueueHandle_t xWheelQueue = pTaskParameters->xWheelQueue;

  bool timeoutNotify = true;
  while(true)
  {
    // Wait for next velocity command
    if (pdTRUE == xQueueReceive(xCmdVelQueue, &cmdVelData, twist_msg_timeout_interval))
    {
      timeoutNotify = true;

      if (0.0f != cmdVelData.angular.x ||
          0.0f != cmdVelData.angular.y ||
          0.0f != cmdVelData.linear.y ||
          0.0f != cmdVelData.linear.z )
      {
        ESP_LOGW(TAG, "Velocity commanded along unsupported axes are ignored.");
      }

      float velocityLinear = cmdVelData.linear.x;
      float velocityAngular = cmdVelData.angular.z;

      wheel_msg_reset(&wheelData);

      if (0 == cmdVelData.angular.z)
      {
        // Straight forward/back. All steering angles were set to zero by wheel_msg_reset
        // so we only need to copy velocity numbers to all six wheels
        for (int wheel = 0; wheel < wheel_count; wheel++)
        {
          wheelData.speed[wheel] = velocityLinear;
        }
      }
      else
      {
        // Travel in an arc. Calculate center of the turn
        float turnCenter = velocityLinear / velocityAngular;

        // Compute speed and steering angle for each wheel
        for (int wheel = 0; wheel < wheel_count; wheel++)
        {
          // Dimensions of a triangle representing a wheel relative to center of turn
          float opposite = wheel_positions[wheel].x;
          float adjacent = turnCenter - wheel_positions[wheel].y;
          float hypotenuse = sqrt(pow(opposite, 2) + pow(adjacent, 2));

          // Ackermann steering angle
          if ( 0 == opposite )
          {
            // Wheels aligned with turning axis do not need to steer
            wheelData.steer[wheel] = 0;
          }
          else
          {
            wheelData.steer[wheel] = atan(opposite / adjacent);
          }

          wheelData.speed[wheel] = velocityAngular * hypotenuse;
          if ( 0 != turnCenter )
          {
            // When not turning in place, we need to fix up the sign because
            // we lost the negative sign on wheel X/Y when squaring those
            // values to calculate hypotenuse.
            wheelData.speed[wheel] = copysign(wheelData.speed[wheel], velocityLinear);
          }

          // If the center of turn is inside the wheel, reverse motor speed.
          if ((turnCenter > 0 && wheel_positions[wheel].y > 0 && wheel_positions[wheel].y > turnCenter) ||
              (turnCenter < 0 && wheel_positions[wheel].y < 0 && wheel_positions[wheel].y < turnCenter))
          {
            wheelData.speed[wheel] *= -1;
          }
        }
      }

      wheelData.timeStamp = xTaskGetTickCount();
      xQueueOverwrite(xWheelQueue, &wheelData);
    }
    else
    {
      if (timeoutNotify) {
        timeoutNotify = false; // Once is enough
        ESP_LOGE(TAG, "Timed out waiting for command velocity message. Continuing to wait...");
      }
    }
  }
}
