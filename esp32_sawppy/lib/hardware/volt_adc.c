/*

Using ESP32 ADC to read battery voltage reduced by a 10:1 divider
Publishes result to a queue of power_msg struct.

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
#include "volt_adc.h"

static const char *TAG = "volt_adc";

void volt_adc_task(void* pvParameter)
{
  int adc_raw;
  float adc_converted;
  power_msg message;

  // Get queue to publish our data to.
  QueueHandle_t xPowerQueue;
  if (NULL == pvParameter)
  {
    ESP_LOGE(TAG, "Task parameter is null. Expected handle to power message queue.");
    vTaskDelete(NULL); // Delete self.
  }
  xPowerQueue = (QueueHandle_t)pvParameter;

  // Configure ADC1 to read battery voltage
  ESP_ERROR_CHECK(adc1_config_width(voltage_precision));
  ESP_ERROR_CHECK(adc1_config_channel_atten(voltage_channel, voltage_attenuation));

  // Continuously read voltage and publish to queue
  while(true) {
    adc_raw = adc1_get_raw(voltage_channel);
    adc_converted = ((float)adc_raw) * voltage_conversion_coefficient;
    // ESP_LOGI(TAG, "Raw %d Converted %.2fV", adc_raw, adc_converted);
    message.voltage = adc_converted;
    message.timeStamp = xTaskGetTickCount();

    xQueueOverwrite(xPowerQueue, &message);

    vTaskDelay(voltage_period);
  }
}
