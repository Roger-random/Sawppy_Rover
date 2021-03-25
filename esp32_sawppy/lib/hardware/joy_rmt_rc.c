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

  // Configure RMT peripheral for each RC channel
  for (int i = 0; i < axis_count; i++)
  {
    rmt_config_t rmt_rx_config = {
      .rmt_mode = RMT_MODE_RX,
      .channel = rc_channels[i].channel,
      .gpio_num = rc_channels[i].pin,
      .clk_div = rmt_clock_divider,
      .mem_block_num = 1,
      .rx_config.filter_en = true,
      .rx_config.filter_ticks_thresh = rmt_filter_threshold,
      .rx_config.idle_threshold = rmt_idle_threshold,
    };
    rmt_config(&rmt_rx_config);
    rmt_driver_install(rc_channels[i].channel, 1024, 0);
    rmt_rx_start(rc_channels[i].channel, true);
  }

  // Read loop
  while(true)
  {
    message.timeStamp = xTaskGetTickCount();
    message.axes[axis_steer] = 0.0;
    message.axes[axis_speed] = 0.0;
    message.axes[axis_aux]   = 0.0;

    xQueueOverwrite(xJoystickQueue, &message);
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}

#endif // #ifdef USE_JOY_RMT_RC
