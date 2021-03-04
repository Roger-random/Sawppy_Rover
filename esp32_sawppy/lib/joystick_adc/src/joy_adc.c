#include "joy_adc.h"

// FreeRTOS task which will read joystick data every joystick_read_period and
// posts to the given queue of type joy_msg.
void joy_adc_read_task(void* pvParameter)
{
  // Get ready to use caller-allocated queue for communicating joystick data
  QueueHandle_t xJoystickQueue;
  if (NULL == pvParameter)
  {
    printf("ERROR: joy_adc_read_task parameter is null. Expected handle to joystick data queue.");
    vTaskDelete(NULL); // Delete self.
  }
  xJoystickQueue = (QueueHandle_t)pvParameter;

  // Configure a pin for GPIO input to see if joystick button is pressed
  gpio_config_t io_conf = {
      .mode = GPIO_MODE_INPUT,
      .intr_type = GPIO_INTR_DISABLE,
      .pin_bit_mask = (1ULL<<joystick_button),
  };
  gpio_config(&io_conf);

  // Configure ADC to read joystick X and Y positions.
  adc1_config_width(joystick_precision);
  adc1_config_channel_atten(joystick_x, joystick_attenuation);
  adc1_config_channel_atten(joystick_y, joystick_attenuation);

  // Read loop
  uint32_t uiX;
  uint32_t uiY;
  bool     bButtonUp;
  joy_msg  message;
  while(true)
  {
    uiX = adc1_get_raw(joystick_x);
    uiY = adc1_get_raw(joystick_y);
    bButtonUp = gpio_get_level(joystick_button); // 1 == button is up.

    message.timeStamp = xTaskGetTickCount();
    message.buttons[axis_steer] = ((float)uiX/256.0) - 1.0;
    message.buttons[axis_speed] = ((float)uiY/256.0) - 1.0;
    message.buttons[button_mode] = !bButtonUp; // 1 == button is pressed.

    xQueueOverwrite(xJoystickQueue, &message);
    vTaskDelay(pdMS_TO_TICKS(joystick_read_period));
  }
}