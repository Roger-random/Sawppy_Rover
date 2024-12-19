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

#include "power_msg.h"

// https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/adc.html
//
// ESP32 analog read configuration isn't "Set GPIO [X] to analog input mode"
// but indirectly configured via ADC channels which correspond to certain
// pins that we have to look up in documentation. Fortunately we have GPIO
// lookup macros to help.
static const adc1_channel_t voltage_channel = ADC1_GPIO35_CHANNEL;

// Read voltage to 12 bits resolution, or 0-4095
static const adc_bits_width_t voltage_precision = ADC_WIDTH_BIT_12;

// No ADC input attenuation
static const adc_atten_t voltage_attenuation = ADC_ATTEN_DB_0;

// Conversion coefficient calculated with help of Fluke multimeter and ESP32
// dev module. Will vary between different ESP32 modules and actual resistance
// values of voltage-dividing resistors. To recalculate this coefficient,
// measure input voltage and see its corresponding raw ADC value.
//
//                   Input voltage reported by trusted meter
//  Coefficient =  ------------------------------------------
//                     Raw ADC value as reported by ESP32
//
// This mechanism is simple and quick but has proven to be nonlinear. Therefore
// voltage measurement error will vary across the input voltage range. If better
// precision is required, consult Espressif documentation on how to utilize
// factory ADC calibration values.
static const float voltage_conversion_coefficient = 0.002872;

// Time to wait between taking voltage measurements
static const TickType_t voltage_period = pdMS_TO_TICKS(50);
/*
 * @brief FreeRTOS task to read battery input voltage
 * @param pvParameter QueueHandle_t of battery_msg
 */
void volt_adc_task(void* pvParameter);

#endif //#ifndef INC_VOLT_ADC_H