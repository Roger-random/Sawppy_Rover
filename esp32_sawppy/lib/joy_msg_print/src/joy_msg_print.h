/*
 *
 * Debug tool: a task to peek joystick queue and print current value
 * 
 * Copyright (c) Roger Cheng
 * Released AS-IS under MIT license
 * 
 */

#ifndef INC_JOY_MSG_PRINT_H
#define INC_JOY_MSG_PRINT_H

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

#include <joy_msg.h>

// Milliseconds to wait between prints
static const TickType_t print_interval = pdMS_TO_TICKS(1000);

// FreeRTOS task which will read joystick message queue
void joy_msg_print_task(void*);

#endif // #ifndef INC_JOY_MSG_PRINT_H
