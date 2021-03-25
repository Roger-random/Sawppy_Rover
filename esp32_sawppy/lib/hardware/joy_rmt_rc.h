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

static rmt_channel_t rmt_channel_steer = RMT_CHANNEL_0;
static rmt_channel_t rmt_channel_throttle = RMT_CHANNEL_1;
static rmt_channel_t rmt_channel_aux = RMT_CHANNEL_2;

/*
 * @brief FreeRTOS task which reads servo control signals sent by radio
 * remote control receiver and translate them to joystick message.
 * @param pvParam QueueHandle to joy_msg mailbox.
 */

void joy_rmt_rc_read_task(void* pvParam);

#endif // #ifdef USE_JOY_RMT_RC
#endif // #ifndef INC_JOY_RMT_RC_H
