/*
 *
 * Using ESP32 LEDC to generate PWM control signals for RC hobby servos
 * responsible for individually steering rover wheels.
 *
 * Reads steering angle from a queue of wheel_msg.
 *
 * Copyright (c) Roger Cheng
 * Released AS-IS under MIT license
 *
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
