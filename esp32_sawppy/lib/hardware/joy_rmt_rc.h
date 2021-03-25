/*
 *
 * Using ESP32 RMT to read servo pulses sent by radio remote control receiver
 * 
 * Publishes result to a queue of joy_msg struct.
 * 
 * Copyright (c) Roger Cheng
 * Released AS-IS under MIT license
 * 
 */

#ifndef INC_JOY_RMT_RC_H
#define INC_JOY_RMT_RC_H

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

#include "driver/rmt.h"

#include <joy_msg.h>

#include "gpio_assignments.h"

#ifdef USE_JOY_RMT_RC

/*
 * @brief Structure to hold RMT configuration information specific to a channel
 */
typedef struct rmt_rc_channel
{
  rmt_channel_t channel;
  gpio_num_t pin;
} rmt_rc_channel;

/*
 * @brief Array of channel-specific RMT configuration parameters
 */
static const rmt_rc_channel rc_channels[axis_count] = {
  // axis_steer
  {
    RMT_CHANNEL_0,
    joy_rmt_rc_steer,
  },
  // axis_speed
  {
    RMT_CHANNEL_1,
    joy_rmt_rc_speed,
  },
  // axis_aux
  {
    RMT_CHANNEL_2,
    joy_rmt_rc_aux,
  }
};

/*
 * @brief Divide RMT 80MHz by 80 = 1MHz = Each tick is 1us (microsecond)
 */
static const uint8_t rmt_clock_divider = 80;

/*
 * @brief Go into idle after inaction longer than 7ms = 7000us
 */
static const uint16_t rmt_idle_threshold = 7000;

/*
 * @brief Signal shorter than 0.25ms = 250us will be ignored as noise
 */
static const uint8_t rmt_filter_threshold = 250;

/*
 * @brief RC receiver usually sends 1ms = 1000us on the low end
 */
static const uint16_t rc_receive_min = 1000;

/*
 * @brief RC receiver usually sends 1.5ms = 1500us as center position
 */
static const uint16_t rc_receive_mid = 1500;

/*
 * @brief RC receiver usually sends 2ms = 2000us on the high end
 */
static const uint16_t rc_receive_max = 2000;

/*
 * @brief FreeRTOS task which reads servo control signals sent by radio
 * remote control receiver and translate them to joystick message.
 * @param pvParam QueueHandle to joy_msg mailbox.
 */

void joy_rmt_rc_read_task(void* pvParam);

#endif // #ifdef USE_JOY_RMT_RC
#endif // #ifndef INC_JOY_RMT_RC_H
