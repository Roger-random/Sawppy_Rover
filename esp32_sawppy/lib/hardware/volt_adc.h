/*
 *
 * Using ESP32 ADC to read battery voltage reduced by a 10:1 divider
 *
 * Publishes result to a queue of battery_msg struct.
 *
 * Copyright (c) Roger Cheng
 * Released AS-IS under MIT license
 *
 */

#ifndef INC_VOLT_ADC_H
#define INC_VOLT_ADC_H

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

#include "esp_err.h"
#include "esp_log.h"
#include "driver/adc.h"
#include "soc/adc_channel.h"

#include "gpio_assignments.h"

// https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/adc.html
//
// ESP32 analog read configuration isn't "Set GPIO [X] to analog input mode"
// but indirectly configured via ADC channels which correspond to certain
// pins that we have to look up in documentation. Fortunately we have GPIO
// lookup macros to help.
static const adc1_channel_t voltage_channel = ADC1_GPIO35_CHANNEL;

// Read voltage to 12 bits resolution, or 0-4095
static const adc_bits_width_t voltage_precision = ADC_WIDTH_BIT_12;

// 2.5dB attentuation lets us read up to approx. 1100 mV on the pin. Given
// the 1:10 voltage divider, that is approximately 1.1 * 11 = 12.1 volts.
static const adc_atten_t voltage_attenuation = ADC_ATTEN_DB_2_5;

// Time to wait between taking voltage measurements
static const TickType_t voltage_period = pdMS_TO_TICKS(500);
/*
 * @brief FreeRTOS task to read battery input voltage
 * @param pvParameter QueueHandle_t of battery_msg
 */
void volt_adc_task(void* pvParameter);

#endif //#ifndef INC_VOLT_ADC_H