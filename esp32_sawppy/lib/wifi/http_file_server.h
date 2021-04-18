/*

Sawppy Rover HTML-based joystick control, server side.

Portions copied from Espressif sample code
https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/protocols/esp_http_server.html
https://github.com/espressif/esp-idf/tree/1cb31e5/examples/protocols/http_server/ws_echo_server
https://github.com/espressif/esp-idf/tree/master/examples/storage/spiffs
Which they have placed in the public domain, or CC0
https://creativecommons.org/share-your-work/public-domain/cc0/

Uses cJSON parser, which Espressif incorporated into ESP-IDF
https://github.com/DaveGamble/cJSON

Remainder Copyright (c) Roger Cheng

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
#ifndef INC_HTTP_FILE_SERVER_H
#define INC_HTTP_FILE_SERVER_H
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "esp_event.h"
#include "esp_http_server.h"
#include "esp_log.h"
#include "esp_spiffs.h"
#include "esp_system.h"
#include "esp_wifi.h"

#include "cJSON.h"

#include "joy_msg.h"
#include "power_msg.h"

/* Flags used in event group */
#define WIFI_CONNECTED_BIT BIT0

/* Time period between sending power updates */
static const TickType_t powerUpdatePeriod = pdMS_TO_TICKS(100);

/*
 * @brief Parameter for http_file_server_task
 */
typedef struct http_file_server_task_parameters
{
  xQueueHandle xJoystickQueue;
  xQueueHandle xPowerQueue;
} http_file_server_task_parameters;

/*
 * @brief FreeRTOS task to launch web server for Sawppy web control panel over WiFI
 * @param pvParameters QueueHandle_t mailbox of joy_msg
 */
void http_file_server_task(void* pvParameters);

#endif // #ifndef INC_HTTP_FILE_SERVER_H
