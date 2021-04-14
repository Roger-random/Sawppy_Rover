/*
 * Modified from ESP32 WiFi SoftAP example
 * https://github.com/espressif/esp-idf/blob/master/examples/wifi/getting_started/softAP/main/softap_example_main.c
 */

#ifndef INC_SOFTAP_START_H
#define INC_SOFTAP_START_H
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "lwip/err.h"
#include "lwip/sys.h"

#include "secrets.h"

/*
 * @brief FreeRTOS task to initialize ESP32 WiFi in access point mode
 * @param pvParameter Currently unused
 */
void softap_start_task(void* pvParameter);

#endif // #ifndef INC_SOFTAP_START_H
