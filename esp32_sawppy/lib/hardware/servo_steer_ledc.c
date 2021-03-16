#include "servo_steer_ledc.h"

void servo_steer_ledc_task(void* pvParam)
{
  wheel_msg message;

  // Retrieve input queue
  if (NULL == pvParam)
  {
    printf("ERROR: servo_steer_ledc_task parameter is null. Expected handle to queue of wheel commands.\n");
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
  ledc_timer_config(&ledc_timer);

  for (int wheel = 0; wheel < wheel_count; wheel++)
  {
    if (GPIO_NUM_NC == steer_control[wheel].gpio)
    {
      // No servo for this wheel, ignore.
      continue;
    }
    ledc_channel_config_t ledc_channel = {
      .channel = steer_control[wheel].channel,
      .duty = steer_duty_mid,
      .gpio_num = steer_control[wheel].gpio,
      .speed_mode = LEDC_LOW_SPEED_MODE,
      .hpoint = 0,
      .timer_sel = LEDC_TIMER_0,
    };
    ledc_channel_config(&ledc_channel);
  }

  // Processing loop
  uint32_t current_duty = steer_duty_mid;
  bool upwards = true;
  while(true)
  {
    // Wait for next joystick message
    if (pdTRUE == xQueuePeek(xWheelQueue, &message, portMAX_DELAY))
    {
      for (int wheel = 0; wheel < wheel_count; wheel++)
      {
        if (GPIO_NUM_NC == steer_control[wheel].gpio)
        {
          // No servo for this wheel, ignore.
          continue;
        }

        ledc_set_duty(
          LEDC_LOW_SPEED_MODE,
          steer_control[wheel].channel,
          current_duty);
        ledc_update_duty(
          LEDC_LOW_SPEED_MODE,
          steer_control[wheel].channel);
      }

      if (upwards)
      {
        current_duty += 100;
        if (current_duty > steer_duty_max)
        {
          current_duty = steer_duty_max;
          upwards = false;
        }
      }
      else
      {
        current_duty -= 100;
        if (current_duty < steer_duty_min)
        {
          current_duty = steer_duty_min;
          upwards = true;
        }
      }
    }
    else
    {
      printf("servo_steer_ledc_task: failed to read from mailbox.\n");
    }

    vTaskDelay(pdMS_TO_TICKS(10));
  }
}
