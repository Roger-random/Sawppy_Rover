#include "joy_steer.h"

void joy_steer_task(void* pvParameter)
{
  joy_msg joyData;
  twist_msg cmdVelData = {
    .timeStamp = xTaskGetTickCount(),
    .linear.x = 0.0,
    .linear.y = 0.0,
    .linear.z = 0.0,
    .angular.x = 0.0,
    .angular.y = 0.0,
    .angular.z = 0.0,
  };

  // Verify input parameters are not null.
  joy_steer_task_parameters* pTaskParameters = (joy_steer_task_parameters*) pvParameter;
  if (NULL == pTaskParameters ||
      NULL == pTaskParameters->xCmdVelQueue ||
      NULL == pTaskParameters->xJoyMsgQueue)
  {
    printf("ERROR: joy_steer_task parameters were missing, exiting.\n");
    vTaskDelete(NULL); // Delete self.
  }
  QueueHandle_t xJoystickQueue = pTaskParameters->xJoyMsgQueue;
  QueueHandle_t xCmdVelQueue = pTaskParameters->xCmdVelQueue;

  // Steering angle, in radians, that will point front wheel axle towards mid wheel,
  // making that mid wheel the center of rotation. This is the minimum turning radius
  // when not in turn-in-place mode.
  float mid_wheel_point = atan2((track_mid - track_front)/2, wheelbase_front);
  printf("joy_steer_task mid_wheel_point= %.3f\n", mid_wheel_point);

  // Setup complete, begin processing loop.
  float velocity_angular = 0;
  float velocity_linear = 0;
  while(true)
  {
    // Wait for next joystick message
    if (pdTRUE == xQueueReceive(xJoystickQueue, &joyData, joy_msg_timeout_interval))
    {
      if (fabs(joyData.axes[axis_speed]) > speed_epsilon)
      {
        velocity_linear = joyData.axes[axis_speed] * velocity_linear_max;
      }
      else
      {
        velocity_linear = 0;
      }

      if (fabs(joyData.axes[axis_steer]) > steering_epsilon)
      {
        // Calculate angular velocity based on speed and steering angle
        velocity_angular = velocity_linear / (tan(joyData.axes[axis_steer] * mid_wheel_point) * wheelbase_front);
      }
      else
      {
        // Going straight forward or back
        velocity_angular = 0;
      }

      // Send results as a Twist message to velocity command queue
      cmdVelData.timeStamp = xTaskGetTickCount();
      cmdVelData.linear.x = velocity_linear;
      cmdVelData.angular.z = velocity_angular;
      xQueueOverwrite(xCmdVelQueue, &cmdVelData);
    }
    else
    {
      printf("ERROR: joy_steer_task timed out waiting for joystick message. Continuing to wait...\n");
    }
  }
}
