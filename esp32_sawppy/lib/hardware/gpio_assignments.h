/*

ESP32 GPIO pin assignments for rover chassis configurations.

Reference: https://randomnerdtutorials.com/esp32-pinout-reference-gpios/

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

#ifndef INC_GPIO_ASSIGNMENTS_H
#define INC_GPIO_ASSIGNMENTS_H

#include "hal/gpio_types.h"

#define MSB3

#ifdef CARDBOARD_BOX_TESTBED
#define USE_JOY_ADC
#endif

#ifdef MSB3
#define USE_WIFI
#endif

static const gpio_num_t __boot_pwm = GPIO_NUM_0; // PWM
// static const gpio_num_t __uart_tx0 = GPIO_NUM_1;
static const gpio_num_t drv8833_back_right_b = GPIO_NUM_2;
// static const gpio_num_t __uart_rx0 = GPIO_NUM_3;
static const gpio_num_t drv8833_back_right_a = GPIO_NUM_4;
static const gpio_num_t servo_front_right = GPIO_NUM_5; // PWM
// static const gpio_num_t __flash_ck = GPIO_NUM_6;
// static const gpio_num_t __flash_d0 = GPIO_NUM_7;
// static const gpio_num_t __flash_d1 = GPIO_NUM_8;
// static const gpio_num_t __flash_d2 = GPIO_NUM_9;
// static const gpio_num_t __flash_d3 = GPIO_NUM_10;
// static const gpio_num_t __flash_cmd = GPIO_NUM_11;
static const gpio_num_t drv8833_back_left_b = GPIO_NUM_12;
static const gpio_num_t servo_back_left = GPIO_NUM_13;
static const gpio_num_t servo_front_left = GPIO_NUM_14; // PWM
static const gpio_num_t servo_back_right = GPIO_NUM_15; // PWM
static const gpio_num_t drv8833_mid_right_b = GPIO_NUM_16;
static const gpio_num_t drv8833_mid_right_a = GPIO_NUM_17;
static const gpio_num_t drv8833_front_right_b = GPIO_NUM_18;
static const gpio_num_t drv8833_front_right_a = GPIO_NUM_19;
// static const gpio_num_t __unavailable_20 = GPIO_NUM_20;
static const gpio_num_t __i2c_sda = GPIO_NUM_21;
static const gpio_num_t __i2c_scl = GPIO_NUM_22;
static const gpio_num_t __free_io_23 = GPIO_NUM_23;
// GPIO_NUM_24 not defined in gpio_types.h
static const gpio_num_t drv8833_mid_left_a = GPIO_NUM_25;
static const gpio_num_t drv8833_mid_left_b = GPIO_NUM_26;
static const gpio_num_t drv8833_back_left_a = GPIO_NUM_27;
// static const gpio_num_t __unavailable_28 = GPIO_NUM_28;
// static const gpio_num_t __unavailable_29 = GPIO_NUM_29;
// static const gpio_num_t __unavailable_30 = GPIO_NUM_30;
// static const gpio_num_t __unavailable_31 = GPIO_NUM_31;
static const gpio_num_t drv8833_front_left_a = GPIO_NUM_32;
static const gpio_num_t drv8833_front_left_b = GPIO_NUM_33;
#ifdef USE_JOY_ADC
static const gpio_num_t joystick_button = GPIO_NUM_34; // Needs external pull-up resistor
#endif
#ifdef USE_JOY_RMT_RC
static const gpio_num_t joy_rmt_rc_aux = GPIO_NUM_34;
#endif
static const gpio_num_t adc_volt = GPIO_NUM_35;

#ifdef USE_JOY_ADC
static const gpio_num_t adc_joystick_x = GPIO_NUM_36; // Indirectly used via ADC1_GPIO36_CHANNEL
#endif
#ifdef USE_JOY_RMT_RC
static const gpio_num_t joy_rmt_rc_steer = GPIO_NUM_36;
#endif

// static const gpio_num_t __unavailable_37 = GPIO_NUM_37;
// static const gpio_num_t __unavailable_38 = GPIO_NUM_38;

#ifdef USE_JOY_ADC
static const gpio_num_t adc_joystick_y = GPIO_NUM_39; // Indirectly used via ADC1_GPIO39_CHANNEL
#endif
#ifdef USE_JOY_RMT_RC
static const gpio_num_t joy_rmt_rc_speed = GPIO_NUM_39;
#endif

#endif // #ifndef INC_GPIO_ASSIGNMENTS_H
