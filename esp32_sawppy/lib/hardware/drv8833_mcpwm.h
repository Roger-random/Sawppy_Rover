/*
 *
 * Using ESP32 MCPWM to control a DRV8833 DC Motor Control IC
 * (And those compatible with its two PWM per motor control scheme.)
 * 
 * Copyright (c) Roger Cheng
 * Released AS-IS under MIT license
 * 
 */

#ifndef INC_DRV8833_MCPWM_H
#define INC_DRV8833_MCPWM_H

#include <math.h>

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

#include "driver/mcpwm.h"

#include <wheel_parameter.h>
#include <wheel_msg.h>

// Wheel messages must be received at least this often, or else the rover
// will be commanded to stop as a safety measure.
static const TickType_t wheel_msg_timeout_interval = pdMS_TO_TICKS(500);

// Wheel speed update proportion.
// 0.25 means 25% of new speed averaged with 75% of existing speed.
// When set to 1.0, all commands are instantaneous.
// Values less than 1.0 allows gradual ramping of speed.
static const float wheel_accel_max = 0.1;

// 50 kHz to run this above range of human hearing in order to avoid
// audible whine. Well within range of what DRV8833 can handle.
static const uint32_t pwm_freq = 50000;

// Duty cycle limit: restrict duty cycle to be no greater than this value.
// Necessary when the power supply exceeds voltage rating of motor.
// TT gearmotors are typically quoted for 3-6V operation.
// Fully charged 2S LiPo is 8.4V. 6/8.4 ~= 72%.
// Feeling adventurous? Nominal 2S LiPo is 7.4V. 7.4/8.4 ~= 88%
// 4 AA batteries (max 6V) = 100.
static const uint32_t duty_cycle_max = 100;

/*
 * @brief MCPWM peripheral parameters for a particular wheel
 */
typedef struct mcpwm_motor_control {
  mcpwm_unit_t unit;
  mcpwm_timer_t timer;
  mcpwm_io_signals_t signalA;
  gpio_num_t gpioA;
  mcpwm_io_signals_t signalB;
  gpio_num_t gpioB;
} mcpwm_motor_control;

/*
 * @brief MCPWM peripheral parameters for all wheels
 */
static const mcpwm_motor_control speed_control[wheel_count] = {
  // front_left
  {
    MCPWM_UNIT_0,
    MCPWM_TIMER_0,
    MCPWM0A,
    GPIO_NUM_32,
    MCPWM0B,
    GPIO_NUM_33,
  },
  // front_right
  {
    MCPWM_UNIT_1,
    MCPWM_TIMER_0,
    MCPWM0A,
    GPIO_NUM_19,
    MCPWM0B,
    GPIO_NUM_18,
  },
  // mid_left
  {
    MCPWM_UNIT_0,
    MCPWM_TIMER_1,
    MCPWM1A,
    GPIO_NUM_25,
    MCPWM1B,
    GPIO_NUM_26,
  },
  // mid_right
  {
    MCPWM_UNIT_1,
    MCPWM_TIMER_1,
    MCPWM1A,
    GPIO_NUM_17,
    MCPWM1B,
    GPIO_NUM_16,
  },
  // back_left
  {
    MCPWM_UNIT_0,
    MCPWM_TIMER_2,
    MCPWM2A,
    GPIO_NUM_27,
    MCPWM2B,
    GPIO_NUM_12,
  },
  // back_right
  {
    MCPWM_UNIT_1,
    MCPWM_TIMER_2,
    MCPWM2A,
    GPIO_NUM_4,
    MCPWM2B,
    GPIO_NUM_2,
  },
};

/*
 * @brief FreeRTOS task which reads wheel_msg and operates ESP32 MCPWM
 * peripheral to control DRV8833 based on speed commands.
 * @param pvParam QueueHandle to wheel_msg mailbox.
 */
void drv8833_mcpwm_task(void* pvParam);

#endif // #ifndef INC_DRV8833_MCPWM_H