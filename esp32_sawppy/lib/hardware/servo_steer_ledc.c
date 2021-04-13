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

        // Ignore wheels that are not moving.
        if (fabs(message.speed[wheel]) < 0.01)
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
