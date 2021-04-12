#include "volt_adc.h"

static const char *TAG = "volt_adc";

void volt_adc_task(void* pvParameter)
{
  int adc_raw;
  float adc_converted;

  // Configure ADC1 to read battery voltage
  ESP_ERROR_CHECK(adc1_config_width(voltage_precision));
  ESP_ERROR_CHECK(adc1_config_channel_atten(voltage_channel, voltage_attenuation));

  while(true) {
    adc_raw = adc1_get_raw(voltage_channel);
    adc_converted = 11*1.1*((float)adc_raw)/4096;
    ESP_LOGI(TAG, "Raw %d Converted %.2fV", adc_raw, adc_converted);
    vTaskDelay(voltage_period);
  }
}
