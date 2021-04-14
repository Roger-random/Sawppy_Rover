/*

Using ESP32 ADC to read position of a joystick composed of two potentiometers
and a button. Publishes result to a queue of joy_msg.

Reference: https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/adc.html

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
#ifndef INC_JOY_ADC_H
#define INC_JOY_ADC_H

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

#include "driver/adc.h"
#include "soc/adc_channel.h"

#include <joy_msg.h>

#include "gpio_assignments.h"

#ifdef USE_JOY_ADC

// https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/adc.html
//
// ESP32 analog read configuration isn't "Set GPIO [X] to analog input mode"
// but indirectly configured via ADC channels which correspond to certain
// pins that we have to look up in documentation. Fortunately we have GPIO
// lookup macros to help.
//
// ADC2 is shared with WiFi and few other hardware peripherals. Given its
// constraints, usage is usually avoided.
//
// ADC1 does not have those constraints, and the input-only GPIO 34, 35, 36,
// and 39 correspond to ADC1. So those are our first choices for joystick input.
//
// "GPIO34-39 can only be set as input mode and do not have software pullup or pulldown functions."
// https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/gpio.html
// so GPIO34 with the button will need an external pull-up resistor.

static const adc1_channel_t joystick_x = ADC1_GPIO36_CHANNEL;
static const adc1_channel_t joystick_y = ADC1_GPIO39_CHANNEL;

// Unknown: does setting resolution to lower resolution gain any benefit?
// Could a read operation be faster for fewer bits of precision?
// Available widths are 9 through 12. 2^9 = 512 and plenty for this scenario.
static const adc_bits_width_t joystick_precision = ADC_WIDTH_BIT_9;

// ADC range treated as center null zone. Center +/- this value is still center.
static const uint32_t joystick_nullzone = 15;

// ADC measures up to ~800mV. But we can add attenuation to extend range
// of measurement. DB11 gets us up to ~2600mV. Since the joysticks are
// potentiometers between 3.3V and GND, we lose the range between 2.6 and 3.3.
// It doesn't matter much on these crappy lowest-bidder joysticks with barely
// any analog proportionality, so ignore until we need that precision.
static const adc_atten_t    joystick_attenuation = ADC_ATTEN_DB_11;

// Number of ticks to wait between reads while getting joystick range
// This should be faster than joystick_read_period because user will likely flick
// through full range rapidly and we need to catch that.
static const TickType_t     joystick_ranging_read_period = pdMS_TO_TICKS(10);

// Number of ticks to wait between normal read operations.
static const TickType_t     joystick_read_period = pdMS_TO_TICKS(100);

// Historically lowest value of X axis
uint32_t uiXlow;

// X axis value treated as center
uint32_t uiXcenter;

// Historically highest value of X axis
uint32_t uiXhigh;

// Current value of X axis
uint32_t uiXnow;

// Historically lowest value of Y axis
uint32_t uiYlow;

// Y axis value treated as center
uint32_t uiYcenter;

// Historically highest value of Y axis
uint32_t uiYhigh;

// Current value of Y axis
uint32_t uiYnow;

// Above values "High" and "Low" is relative to raw ADC values. However
// depending on how a joystick is wired, we may want to invert one or both
// axis at the end, just before sending out the joystick message.
static const bool invert_steer = true;
static const bool invert_speed = false;

// FreeRTOS task which will read joystick data every joystick_read_period and
// posts to the given queue of type joy_msg.
void joy_adc_read_task(void*);

#endif // #ifdef USE_JOY_ADC

#endif // #ifndef INC_JOY_ADC_H
