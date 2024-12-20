/*

Using ESP32 LEDC to generate PWM control signals for RC hobby servos
responsible for individually steering rover wheels.
Reads steering angle commands from a queue of wheel_msg.

Reference: https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/ledc.html

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
#include "servo_steer_ledc.h"

static const char *TAG = "servo_steer_ledc";

void servo_steer_ledc_task(void* pvParam)
{
  wheel_msg message;

  // Retrieve input queue
  if (NULL == pvParam)
  {
    ESP_LOGE(TAG, "servo_steer_ledc_task parameter is null. Expected handle to queue of wheel commands.");
    vTaskDelete(NULL); // Delete self.
  }
  QueueHandle_t xWheelQueue = (QueueHandle_t)pvParam;

  // Configure LEDC peripheral
  ledc_timer_config_t ledc_timer = {
    .duty_resolution = LEDC_TIMER_15_BIT,
    .freq_hz = 50,
    .speed_mode = LEDC_LOW_SPEED_MODE,
    .timer_num = LEDC_TIMER_0,
    .clk_cfg = LEDC_AUTO_CLK,
  };
  ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

  for (int wheel = 0; wheel < wheel_count; wheel++)
  {
    if (GPIO_NUM_NC == steer_control[wheel].gpio)
    {
      // No servo for this wheel, ignore.
      continue;
    }
    ledc_channel_config_t ledc_channel = {
      .channel = steer_control[wheel].channel,
      .duty = steer_control[wheel].mid,
      .gpio_num = steer_control[wheel].gpio,
      .speed_mode = LEDC_LOW_SPEED_MODE,
      .hpoint = 0,
      .timer_sel = LEDC_TIMER_0,
    };
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));
  }

  // Processing loop
  uint32_t current_duty = steer_duty_mid;
  float steer_radian = 0;
  float scale_factor = 1.0;
  while(true)
  {
    // Wait for next joystick message
    if (pdTRUE == xQueuePeek(xWheelQueue, &message, portMAX_DELAY))
    {
      for (int wheel = 0; wheel < wheel_count; wheel++)
      {
        // Ignore wheels without a designated steering control pin.
        if (GPIO_NUM_NC == steer_control[wheel].gpio)
        {
          continue;
        }

        // Retrieve steering angle in radians
        steer_radian = message.steer[wheel];
        if (steer_control[wheel].invert)
        {
          steer_radian *= -1;
        }

        // Convert to duty cycle. Positive steering angle is scaled between
        // mid and max. Negative angle scaled between mid and min.
        scale_factor = steer_radian / M_PI_2;
        if (steer_radian > 0)
        {
          current_duty = steer_control[wheel].max - steer_control[wheel].mid;
        }
        else
        {
          current_duty = steer_control[wheel].mid - steer_control[wheel].min;
        }
        current_duty *= scale_factor;
        current_duty += steer_control[wheel].mid;

        // Update LEDC peripheral with results
        ESP_ERROR_CHECK(ledc_set_duty(
          LEDC_LOW_SPEED_MODE,
          steer_control[wheel].channel,
          current_duty));
        ESP_ERROR_CHECK(ledc_update_duty(
          LEDC_LOW_SPEED_MODE,
          steer_control[wheel].channel));
      }
    }
    else
    {
      ESP_LOGE(TAG, "Failed to read from mailbox.");
    }

    vTaskDelay(pdMS_TO_TICKS(10));
  }
}
