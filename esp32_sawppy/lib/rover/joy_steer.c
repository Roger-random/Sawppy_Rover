/*

Convert raw joystick input represented by joy_msg into a rover velocity
command in the form of twist_msg, taking into account rover geometry and
human friendliness. Experience indicated most humans are get confused with
a chassis more maneuverable than most cars, so we keep things constrained.

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
#include "joy_steer.h"

static const char *TAG = "joy_steer";

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
    ESP_LOGE(TAG, "Task parameters were missing, exiting.");
    vTaskDelete(NULL); // Delete self.
  }
  QueueHandle_t xJoystickQueue = pTaskParameters->xJoyMsgQueue;
  QueueHandle_t xCmdVelQueue = pTaskParameters->xCmdVelQueue;

  // Steering angle, in radians, that will point front wheel axle towards mid wheel,
  // making that mid wheel the center of rotation. This is the minimum turning radius
  // when not in turn-in-place mode. Not because of any mechanical limitation but to
  // keep from confusing human pilots unfamiliar with full range of rover maneuverability.
  float mid_wheel_point = atan2(wheelbase_front, (track_mid - track_front)/2);
  ESP_LOGI(TAG, "joy_steer_task mid_wheel_point= %.3f", mid_wheel_point);

  // Setup complete, begin processing loop.
  float velocity_angular = 0;
  float velocity_linear = 0;
  bool timeoutNotify = true;
  while(true)
  {
    // Wait for next joystick message
    if (pdTRUE == xQueueReceive(xJoystickQueue, &joyData, joy_msg_timeout_interval))
    {
      timeoutNotify = true;
      if (fabs(joyData.axes[axis_speed]) > speed_epsilon)
      {
        velocity_linear = joyData.axes[axis_speed] * velocity_linear_max;
        if (joy_speed_invert)
        {
          // Some joysticks are wired positive direction pointing towards rover rear
          velocity_linear *= -1;
        }
      }
      else
      {
        velocity_linear = 0;
      }

      if (fabs(joyData.axes[axis_steer]) > steering_epsilon)
      {
        // Calculate angular velocity based on speed and steering angle

        // Joystick axis dictates steering as percentage of steering angle that gives us
        // our minimum turning radius calculated earlier.
        float steering_angle = joyData.axes[axis_steer] * mid_wheel_point;
        if (joy_steer_invert)
        {
          // Some joysticks are wired positive direction pointing towards rover right,
          // which is negative Z rotation by right-hand rule.
          steering_angle *= -1;
        }

        // Based on steering angle, calculate the center of turn for the arc which will be
        // a point on the Y-axis. The first calculation is relative to front wheel, then
        // we add (or subtract) half of the front wheel track to obtain actual Y coordinate.
        float turn_center = (wheelbase_front / tan(steering_angle));
        if (turn_center > 0)
        {
          turn_center += (track_front / 2.0);
        }
        else
        {
          turn_center -= (track_front / 2.0);
        }

        // Given the desired center of turn and linear velocity, we can calculate
        // an angular velocity that should hit our desired arc center and speed.
        velocity_angular = velocity_linear / turn_center;
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
      if (timeoutNotify) {
        timeoutNotify = false; // Once is enough
        ESP_LOGE(TAG, "joy_steer_task timed out waiting for joystick message. Continuing to wait...");
      }
    }
  }
}
