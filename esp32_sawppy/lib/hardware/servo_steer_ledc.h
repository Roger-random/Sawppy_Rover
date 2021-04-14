/*

Using ESP32 LEDC to generate PWM control signals for RC hobby servos
responsible for individually steering rover wheels.
Reads steering angle commands from a queue of wheel_msg.

Reference: https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/ledc.html

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
#ifndef INC_SERVO_STEER_LEDC_H
#define INC_SERVO_STEER_LEDC_H

#include <math.h>

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

#include "driver/ledc.h"

#include "esp_err.h"
#include "esp_log.h"

#include <wheel_parameter.h>
#include <wheel_msg.h>

#include "gpio_assignments.h"

// 15-bit PWM duty cycle resolution. 2^15 = 32768
// 50 Hz PWM frequency (once every 20ms)
// 32768 / 20 = 1638 representing 1ms
// 819 = 0.5ms, usually 90 degree rotation on way
static const uint32_t steer_duty_min = 819;

// 2460 = 1.5ms, representing central position.
static const uint32_t steer_duty_mid = 2460;

// 4096 = 2.5ms, usually 90 degrees rotation other way.
static const uint32_t steer_duty_max = 4096;

/*
 * @brief LEDC peripheral parameters to steer one wheel
 */
typedef struct ledc_servo_control {
  ledc_channel_t channel;
  gpio_num_t gpio; // GPIO_NUM_NC to indicate not connected
  bool invert;
  uint32_t min;
  uint32_t mid;
  uint32_t max;
} ledc_servo_control;

/*
 * @brief LEDC peripheral parameters for all wheels
 */
static const ledc_servo_control steer_control[wheel_count] = {
  // front left
  {
    LEDC_CHANNEL_0,
    servo_front_left,
    true,
    steer_duty_min,
    steer_duty_mid,
    steer_duty_max,
  },
  // front right
  {
    LEDC_CHANNEL_1,
    servo_front_right,
    true,
    steer_duty_min,
    steer_duty_mid,
    steer_duty_max,
  },
  // mid left
  {
    LEDC_CHANNEL_MAX,
    GPIO_NUM_NC,
    true,
    steer_duty_min,
    steer_duty_mid,
    steer_duty_max,
  },
  // mid right
  {
    LEDC_CHANNEL_MAX,
    GPIO_NUM_NC,
    true,
    steer_duty_min,
    steer_duty_mid,
    steer_duty_max,
  },
  // back left
  {
    LEDC_CHANNEL_2,
    servo_back_left,
    true,
    steer_duty_min,
    steer_duty_mid,
    steer_duty_max,
  },
  // back right
  {
    LEDC_CHANNEL_3,
    servo_back_right,
    true,
    steer_duty_min,
    steer_duty_mid,
    steer_duty_max,
  },
};

/*
 * @brief FreeRTOS task to send PWM command to steering servos
 * @param pvParam QueueHandle to wheel_msg mailbox.
 */
void servo_steer_ledc_task(void* pvParam);

#endif // INC_SERVO_STEER_LEDC_H
