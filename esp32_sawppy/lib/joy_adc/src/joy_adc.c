#include "joy_adc.h"

/**
 * @brief Normalizes ADC value to range from -1 to 1
 * @param low Lowest ADC value seen to date, which will map to -1
 * @param center Center position that will map to 0
 * @param high Highest ADC value seen to date, which will map to 1
 * @param now The current ADC value
 * @return Position repesented by 'now' normalized from (low,center,high) to (-1,0,1)
 */
float joystick_axis_conversion(uint32_t low, uint32_t center, uint32_t high, uint32_t now)
{
  uint32_t threshold_positive = center + joystick_nullzone;
  uint32_t threshold_negative = center - joystick_nullzone;
  if (now > threshold_positive)
  {
    return ((float)now - threshold_positive)/(high - threshold_positive);
  }
  else if (now < threshold_negative)
  {
    return ((float)now - threshold_negative)/(threshold_negative - low);
  }
  else
  {
    return 0.0;
  }
}

/**
 * @brief Read ADC for joystick X and Y values
 *
 * @note  Updates 'now' value, will also update 'low' and 'high' as needed
 */
void joystick_get_x_y()
{
  // Get current joystick X,Y values
  uiXnow = adc1_get_raw(joystick_x);
  uiYnow = adc1_get_raw(joystick_y);

  // If current X is higher/lower than historical values, update accordingly.
  if (uiXlow > uiXnow)
  {
    uiXlow = uiXnow;
  }
  else if (uiXhigh < uiXnow)
  {
    uiXhigh = uiXnow;
  }

  // If current Y is higher/lower than historical values, update accordingly.
  if (uiYlow > uiYnow)
  {
    uiYlow = uiYnow;
  }
  else if (uiYhigh < uiYnow)
  {
    uiYhigh = uiYnow;
  }
}

/**
 * @brief Poll joystick button until it is released
 */
void wait_for_button_release()
{
  while(!gpio_get_level(joystick_button))
  {
    printf("joy_adc waiting for button release\n");
    vTaskDelay(joystick_read_period);
  }
}

/**
 * @brief FreeRTOS task which will read joystick data every joystick_read_period
 * convert to normalized value and overwrites joy_msg at head of a queue.
 * @param pvParameter A QueueHandle_t of type joy_msg
 */
void joy_adc_read_task(void* pvParameter)
{
  joy_msg  message;

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
  uiXlow = uiXcenter = uiXhigh = adc1_get_raw(joystick_x);
  uiYlow = uiYcenter = uiYhigh = adc1_get_raw(joystick_y);

  // If button is currently pressed, wait until it is released.
  wait_for_button_release();

  // Print instructions to output in case anyone is reading.
  printf("joy_adc entering auto-ranging mode. Move joystick to min and max on all axes then press button.\n");

  // Joystick auto-ranging routine: move stick to min and max on both axes
  // then press the button.
  while(gpio_get_level(joystick_button) ||
        uiXlow > uiXcenter - joystick_nullzone ||
        uiXhigh < uiXcenter + joystick_nullzone ||
        uiYlow > uiYcenter - joystick_nullzone ||
        uiYhigh < uiYcenter + joystick_nullzone)
  {
    joystick_get_x_y();
    vTaskDelay(joystick_ranging_read_period);
  }

  // Wait for button to release before entering read loop.
  wait_for_button_release();

  // Report joystick range to serial output.
  printf("joy_adc using range X (%d, %d, %d) Y (%d, %d, %d)\n", uiXlow, uiXcenter, uiXhigh, uiYlow, uiYcenter, uiYhigh);

  // Read loop
  while(true)
  {
    joystick_get_x_y();

    message.timeStamp = xTaskGetTickCount();
    message.axes[axis_speed] = joystick_axis_conversion(uiXlow, uiXcenter, uiXhigh, uiXnow);
    message.axes[axis_steer] = joystick_axis_conversion(uiYlow, uiYcenter, uiYhigh, uiYnow);
    message.buttons[button_mode] = !gpio_get_level(joystick_button); // nonzero == button is pressed.

    xQueueOverwrite(xJoystickQueue, &message);
    vTaskDelay(joystick_read_period);
  }
}