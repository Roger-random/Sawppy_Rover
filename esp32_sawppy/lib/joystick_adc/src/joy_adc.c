#include "joy_adc.h"

// Converts raw ADC reading ()
float joystick_axis_conversion(uint32_t center, uint32_t reading)
{
  uint32_t threshold_positive = center + joystick_null;
  uint32_t threshold_negative = center - joystick_null;
  if (reading > threshold_positive)
  {
    return ((float)reading - threshold_positive)/(joystick_max - threshold_positive);
  }
  else if (reading < threshold_negative)
  {
    return ((float)reading - threshold_negative)/threshold_negative;
  }
  else
  {
    return 0.0;
  }
}

// FreeRTOS task which will read joystick data every joystick_read_period and
// posts to the given queue of type joy_msg.
void joy_adc_read_task(void* pvParameter)
{
  uint32_t uiXcenter = 0;
  uint32_t uiYcenter = 0;
  joy_msg  message;
  uint32_t retry;

  // Get ready to use caller-allocated queue for communicating joystick data
  QueueHandle_t xJoystickQueue;
  if (NULL == pvParameter)
  {
    printf("ERROR: joy_adc_read_task parameter is null. Expected handle to joystick data queue.\n");
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

  // Read initial values as center. In case of low quality joysticks that drift, may need
  // to kill and restart this task in order to update these centering values.
  retry = 0;
  while(uiXcenter < joystick_null || uiXcenter > (joystick_max - joystick_null))
  {
    uiXcenter = adc1_get_raw(joystick_x);
    if (retry++ > 10)
    {
      printf("ERROR: unable to retrieve reasonable joystick center X.\n");
      vTaskDelete(NULL); // Delete self.
    }
  }
  retry = 0;
  while(uiYcenter < joystick_null || uiYcenter > (joystick_max - joystick_null))
  {
    uiYcenter = adc1_get_raw(joystick_y);
    if (retry++ > 10)
    {
      printf("ERROR: unable to retrieve reasonable joystick center Y.\n");
      vTaskDelete(NULL); // Delete self.
    }
  }

  // Read loop
  while(true)
  {
    message.timeStamp = xTaskGetTickCount();
    message.axes[axis_speed] = joystick_axis_conversion(uiXcenter, adc1_get_raw(joystick_x));
    message.axes[axis_steer] = joystick_axis_conversion(uiYcenter, adc1_get_raw(joystick_y));
    message.buttons[button_mode] = !gpio_get_level(joystick_button); // nonzero == button is pressed.

    xQueueOverwrite(xJoystickQueue, &message);
    vTaskDelay(pdMS_TO_TICKS(joystick_read_period));
  }
}