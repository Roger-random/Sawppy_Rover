/*

Debug tool: Tasks to peek a message queue and print current value

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
#ifndef INC_MSG_PRINT_H
#define INC_MSG_PRINT_H

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

#include "esp_log.h"

#include <joy_msg.h>
#include <twist_msg.h>
#include <wheel_msg.h>

// Ticks to wait between prints. NEVER SET TO ZERO.
// xQueuePeek does not block() and setting this to zero would consume all
// CPU, starving other tasks.
static const TickType_t print_interval = pdMS_TO_TICKS(1000);

// FreeRTOS task which will read joystick message queue
void joy_msg_print_task(void*);

// FreeRTOS task which will read twist message queue
void twist_msg_print_task(void*);

// FreeRTOS task which will read wheel control message
void wheel_msg_print_task(void*);

#endif // #ifndef INC_MSG_PRINT_H
