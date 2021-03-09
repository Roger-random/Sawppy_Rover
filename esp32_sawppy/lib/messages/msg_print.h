/*
 *
 * Debug tool: a task to peek a message queue and print current value
 * 
 * Copyright (c) Roger Cheng
 * Released AS-IS under MIT license
 * 
 */

#ifndef INC_MSG_PRINT_H
#define INC_MSG_PRINT_H

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

#include <joy_msg.h>
#include <twist_msg.h>

// Ticks to wait between prints. NEVER SET TO ZERO.
// xQueuePeek does not block() and setting this to zero would consume all
// CPU, starving other tasks.
static const TickType_t print_interval = pdMS_TO_TICKS(1000);

// FreeRTOS task which will read joystick message queue
void joy_msg_print_task(void*);

// FreeRTOS task which will read twist message queue
void twist_msg_print_task(void*);

#endif // #ifndef INC_MSG_PRINT_H
