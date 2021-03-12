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

// Wheel speed acceleration cap.
// Wheel velocity will not change by more than this amount per update.
static const float wheel_accel_max = 0.1;

// Frequency for MCPWM control signals. DRV8833 can handle up to megahertz
// and still stay within its minimum required rise and fall times. Running
// slower than range of human hearing (~20kHz) will result in audible whine
// during operation. Running at higher speeds will make the rover less
// audibly noisy. However switching losses also increase at higher frequencies,
// and motor response curve (duty cycle vs. motor speed) will also change.
static const uint32_t pwm_freq = 20000;

// Duty cycle limit: restrict duty cycle to be no greater than this value.
// Necessary when the power supply exceeds voltage rating of motor.
// TT gearmotors are typically quoted for 3-6V operation.
// Fully charged 2S LiPo is 8.4V. 6/8.4 ~= 72%.
// Feeling adventurous? Nominal 2S LiPo is 7.4V. 7.4/8.4 ~= 88%
// 4 AA batteries (max 6V) = 100%.
static const float duty_cycle_max = 88;

// Approximate speed, in meters per second, of wheel on duty_cycle_max
static const float wheel_speed_max = 0.75;

// TT gear motors couldn't spin at arbitrarily low speeds. This is the
// lowest speed our wheel can sustain, anything lower just stops.
static const float duty_cycle_min = 60;
static const float wheel_speed_min = 0.15;

// Starting from a stop, the DC gear motor needs extra power to overcome
// static friction.
static const float wheel_speed_startup = 0.30;

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