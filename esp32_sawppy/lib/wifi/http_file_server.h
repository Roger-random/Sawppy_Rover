#ifndef INC_HTTP_FILE_SERVER_H
#define INC_HTTP_FILE_SERVER_H
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"

#include "esp_event.h"
#include "esp_http_server.h"
#include "esp_log.h"
#include "esp_spiffs.h"
#include "esp_system.h"
#include "esp_wifi.h"

#include "cJSON.h"

/* Flags used in event group */
#define WIFI_CONNECTED_BIT BIT0

void http_file_server_task(void* pvParameters);

#endif // #ifndef INC_HTTP_FILE_SERVER_H
