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
#include "http_file_server.h"

/* Handle for our HTTPD instance */
static httpd_handle_t server_handle;

/* Waiting for certain WiFi events before continuing */
static EventGroupHandle_t s_wifi_event_group;

/* Mailbox (queue of 1 item) of joy_msg */
static QueueHandle_t xJoystickQueue;

static const char *TAG = "http file server";

// We can spend effort making code memory-efficient, or we can just blow
// a chunk of RAM to make code trivial.
#define readBufSize 1024*16
static char readBuf[readBufSize];

/*
 * @brief When WiFi is connected, sets event group bits to unblock code waiting for WiFi.
 */
static void wifi_connected_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
  if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
    ESP_LOGI(TAG, "Received IP address in station mode");
    xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
  } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_AP_STACONNECTED) {
    // Unsure if WIFI_EVENT_AP_STACONNECTED is the best event to use
    ESP_LOGI(TAG, "Client connected in access point mode");
    xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
  }
}

/*
 * @brief Wait for WiFi to connect far enough to obtain IP address before returning
 */
void wait_for_wifi_ready()
{
  // Used by WiFi event handler to signal we can continue
  s_wifi_event_group = xEventGroupCreate();

  // Register for WiFi event
  esp_event_handler_instance_t instance_got_ip;
  ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
    IP_EVENT_STA_GOT_IP, &wifi_connected_handler, NULL, &instance_got_ip));
  ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
    WIFI_EVENT_AP_STACONNECTED, &wifi_connected_handler, NULL, &instance_got_ip));

  EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group, WIFI_CONNECTED_BIT, pdFALSE, pdFALSE, portMAX_DELAY);
  assert(bits == WIFI_CONNECTED_BIT); // We only have one bit today.

  // Once we're connected to WiFi, no longer need to listen to WiFi event
  ESP_ERROR_CHECK(esp_event_handler_instance_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP, instance_got_ip));
  ESP_ERROR_CHECK(esp_event_handler_instance_unregister(WIFI_EVENT, WIFI_EVENT_AP_STACONNECTED, instance_got_ip));
  vEventGroupDelete(s_wifi_event_group);
}

/*
 * @brief Initialize and mount SPIFFS partition for future access
 */
esp_err_t spiffs_init()
{
    ESP_LOGI(TAG, "Initializing SPIFFS");

    esp_vfs_spiffs_conf_t conf = {
      .base_path = "/static",
      .partition_label = NULL,
      .max_files = 5,
      .format_if_mount_failed = false
    };

    // Use settings defined above to initialize and mount SPIFFS filesystem.
    // Note: esp_vfs_spiffs_register is an all-in-one convenience function.
    esp_err_t ret = esp_vfs_spiffs_register(&conf);

    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE(TAG, "Failed to mount or format filesystem");
        } else if (ret == ESP_ERR_NOT_FOUND) {
            ESP_LOGE(TAG, "Failed to find SPIFFS partition");
        } else {
            ESP_LOGE(TAG, "Failed to initialize SPIFFS (%s)", esp_err_to_name(ret));
        }
        return ret;
    }

    size_t total = 0, used = 0;
    ret = esp_spiffs_info(conf.partition_label, &total, &used);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to get SPIFFS partition information (%s)", esp_err_to_name(ret));
    } else {
        ESP_LOGI(TAG, "Partition size: total: %d, used: %d", total, used);
    }
    return ret;
}

/*
 * @brief Reads file into memory buffer
 * @param file_name Name of file to read
 * @return size of file read
 */
static size_t read_spiff_buffer(const char *file_name)
{
  size_t readSize = 0;
  FILE* f = fopen(file_name, "r");
  if (f == NULL) {
      ESP_LOGE(TAG, "Failed to open file for reading");
      return 0;
  }

  readSize = fread(readBuf, sizeof(char), readBufSize, f);
  fclose(f);

  // Fail if file is larger than memory buffer, as we don't
  // have code to split across multiple read operations.
  if (readSize >= readBufSize) {
    ESP_LOGE(TAG, "File size exceeds memory buffer");
    ESP_ERROR_CHECK(ESP_FAIL);
  } else {
    // Technically unnecessary but an extra null termination never hurt anyone
    readBuf[readSize]=0;
  }

  return readSize;
}

/*
 * @brief URI handler for HTTP GET /index.html
 */
static esp_err_t index_html_get_handler(httpd_req_t *req)
{
  size_t readSize = 0;
  ESP_ERROR_CHECK(httpd_resp_set_hdr(req, "cache-control", "max-age=1")); // For development
  ESP_ERROR_CHECK(httpd_resp_set_type(req, "text/html"));
  readSize = read_spiff_buffer("/static/index.html");
  ESP_ERROR_CHECK(httpd_resp_send(req, readBuf, readSize));

  return ESP_OK;
}

/*
 * @brief URI handler for HTTP GET /index.html
 */
static const httpd_uri_t index_html = {
  .uri      = "/index.html",
  .method   = HTTP_GET,
  .handler  = index_html_get_handler,
  .user_ctx = NULL
};

/*
 * @brief URI handler for HTTP GET /
 */
static const httpd_uri_t root = {
  .uri      = "/",
  .method   = HTTP_GET,
  .handler  = index_html_get_handler,
  .user_ctx = NULL
};

/*
 * @brief URI handler for HTTP GET /joystick.css
 */
static esp_err_t joystick_css_get_handler(httpd_req_t *req)
{
  size_t readSize = 0;
  ESP_ERROR_CHECK(httpd_resp_set_hdr(req, "cache-control", "max-age=1")); // For development
  ESP_ERROR_CHECK(httpd_resp_set_type(req, "text/css"));
  readSize = read_spiff_buffer("/static/joystick.css");
  ESP_ERROR_CHECK(httpd_resp_send(req, readBuf, readSize));

  return ESP_OK;
}

/*
 * @brief URI handler for HTTP GET /joystick.css
 */
static const httpd_uri_t joystick_css = {
  .uri      = "/joystick.css",
  .method   = HTTP_GET,
  .handler  = joystick_css_get_handler,
  .user_ctx = NULL
};

/*
 * @brief URI handler for HTTP GET /joystick.js
 */
static esp_err_t joystick_js_get_handler(httpd_req_t *req)
{
  size_t readSize = 0;
  ESP_ERROR_CHECK(httpd_resp_set_hdr(req, "cache-control", "max-age=1")); // For development
  ESP_ERROR_CHECK(httpd_resp_set_type(req, "application/javascript"));
  readSize = read_spiff_buffer("/static/joystick.js");
  ESP_ERROR_CHECK(httpd_resp_send(req, readBuf, readSize));

  return ESP_OK;
}

/*
 * @brief URI handler for HTTP GET /joystick.js
 */
static const httpd_uri_t joystick_js = {
  .uri      = "/joystick.js",
  .method   = HTTP_GET,
  .handler  = joystick_js_get_handler,
  .user_ctx = NULL
};

/*
 * @brief Struct with information to uniquely identify a websocket
 */
struct websock_instance {
  httpd_handle_t handle;
  int descriptor;
};

// Websocket instance for browser joystick client
static struct websock_instance sJoyMsgSender;

// Amount of time to wait before declaring sJoyMsgSender inactive
static TickType_t tWebsockTimeoutPeriod = pdMS_TO_TICKS(500);

// Timer watching for sJoyMsgSender inactivity
static TimerHandle_t xWebsockTimeout;

/*
 * @brief WebSocket has been closed, clean up associated data structures
 * @param context Unused here, but had to be nonzero for this callback to be called upon socket close
 */
void socket_close_cleanup(void* context)
{
  ESP_LOGI(TAG, "Lost our joy_msg handler.");
  sJoyMsgSender.handle = NULL;
  sJoyMsgSender.descriptor = 0;
  if (pdPASS != xTimerStop(xWebsockTimeout, 0)) {
    ESP_LOGE(TAG, "Failed to stop websock timeout timer");
  }
}

/*
 * @brief Send a control frame to close joystick client WebSocket
 * @param arg Unused
 */
static void vWebsockClose(void *arg)
{
  httpd_ws_frame_t ws_frame = {
    .final = true,
    .fragmented = false,
    .type = HTTPD_WS_TYPE_CLOSE,
    .payload = NULL,
    .len = 0
  };

  ESP_ERROR_CHECK(httpd_ws_send_frame_async(
    sJoyMsgSender.handle, sJoyMsgSender.descriptor, &ws_frame ));
}

/*
 * @brief Queues action to close joystick client websocket when inactivity timer triggers
 */
static void vWebsockTimeoutCallback( TimerHandle_t xTimer)
{
  ESP_ERROR_CHECK(httpd_queue_work(sJoyMsgSender.handle, vWebsockClose, NULL));
}

/*
 * @brief Parse JSON in readBuf[] into joy_msg posted to xJoystickQueue
 */
esp_err_t parseSendJoyMsg() {
  esp_err_t ret = ESP_FAIL;
  joy_msg message;

  // cJSON failure mode is to return NULL. If NULL is passed in as first
  // parameter, NULL will be returned. So we don't really need to check
  // intermediate values for NULL, checking leaf objects are sufficient.
  cJSON *root = cJSON_Parse(readBuf);
  cJSON *axes = cJSON_GetObjectItem(root,"axes");
  cJSON *axes0 = cJSON_GetArrayItem(axes, 0);
  cJSON *axes1 = cJSON_GetArrayItem(axes, 1);
  if (NULL == axes0 || NULL == axes1) {
    ESP_LOGE(TAG, "JSON from browser client is malformed");
    ret = ESP_FAIL;
  } else {
    // Copy parsed steer and speed values into joy_msg
    memset(&message, 0, sizeof(joy_msg));
    message.axes[0] = axes0->valuedouble;
    message.axes[1] = axes1->valuedouble;
    message.timeStamp = xTaskGetTickCount();

    // Post joy_msg to mailbox
    if(pdPASS == xQueueOverwrite(xJoystickQueue, &message)) {
      ret = ESP_OK;
    } else {
      ESP_LOGE(TAG, "Failed to post to joy_msg mailbox");
      ret = ESP_FAIL;
    }
  }

  // Clean up all cJSON data structures
  cJSON_Delete(root);

  return ret;
}

/*
 * @brief URI handler for WebSocket /joy_msg
 */
static esp_err_t websocket_joy_msg_handler(httpd_req_t *req)
{
  if (req->method == HTTP_GET) {
    // This clause was copied from example, but I never see this message in logs
    ESP_LOGI(TAG, "Handshake done, the new connection was opened");
    return ESP_OK;
  }

  if (sJoyMsgSender.handle == NULL) {
    // We didn't have a joy_msg sender before, now we do. Set up notification so
    // we know when it goes away.
    ESP_LOGI(TAG, "Have a new joy_msg sender.");
    sJoyMsgSender.handle = req->handle;
    sJoyMsgSender.descriptor = httpd_req_to_sockfd(req);
    req->sess_ctx = (void*)1; // Set to nonzero otherwise free_ctx won't get called.
    req->free_ctx = socket_close_cleanup;

    if (pdPASS != xTimerStart(xWebsockTimeout, 0)) {
      ESP_LOGE(TAG, "Failed to start websock timeout timer");
      return ESP_FAIL;
    }
  } else if (sJoyMsgSender.handle != req->handle ||
             sJoyMsgSender.descriptor != httpd_req_to_sockfd(req)) {
    ESP_LOGI(TAG, "Already have joy_msg sender, reject connection attempt.");
    return ESP_FAIL;
  }

  // Read websocket data
  httpd_ws_frame_t ws_pkt;
  memset(&ws_pkt, 0, sizeof(httpd_ws_frame_t));
  ws_pkt.type = HTTPD_WS_TYPE_TEXT;
  ws_pkt.payload = (uint8_t*)readBuf;
  ESP_ERROR_CHECK(httpd_ws_recv_frame(req, &ws_pkt, readBufSize));
  if (ws_pkt.len < readBufSize) {
    readBuf[ws_pkt.len] = 0; // null termination
    if (ESP_OK == parseSendJoyMsg()) {
      if (pdPASS != xTimerReset(xWebsockTimeout, 0)) {
        ESP_LOGE(TAG, "Failed to reset websock timeout timer");
        return ESP_FAIL;
      }
    } else {
      // Failed JSON parsing or joy_msg mailbox posting is not a dealbreaker,
      // merely ignored for xWebsockTimeout purposes. No ESP_LOGE here as
      // parseSendJoyMsg() has already done so.
    }
  } else {
    ESP_LOGE(TAG, "Ignoring oversized WebSocket packet");
  }

  return ESP_OK;
}

/*
 * @brief URI handler for WebSocket /joy_msg
 */
static const httpd_uri_t websocket_joy_msg = {
  .uri      = "/joy_msg",
  .method   = HTTP_GET,
  .handler  = websocket_joy_msg_handler,
  .user_ctx = NULL,
  .is_websocket = true
};

void http_file_server_task(void* pvParameters)
{
  // Get ready to use caller-allocated queue for communicating joystick data
  if (NULL == pvParameters)
  {
    ESP_LOGE(TAG, "Task parameter is null. Expected handle to joystick data queue.");
    vTaskDelete(NULL); // Delete self.
  }
  xJoystickQueue = (QueueHandle_t)pvParameters;

  // Initialize structure tracking websocket to client
  sJoyMsgSender.handle = NULL;
  sJoyMsgSender.descriptor = 0;

  // Create timer watching for inactive websocket
  xWebsockTimeout = xTimerCreate("Websock Timeout", tWebsockTimeoutPeriod,
    pdFALSE, 0, vWebsockTimeoutCallback);
  if (NULL==xWebsockTimeout) {
    ESP_LOGE(TAG, "Failed to create websock timeout timer");
    vTaskDelete(NULL);
  }

  // Initialize SPIFFS which holds the HTML/CSS/JS files we serve to client browser
  ESP_ERROR_CHECK(spiffs_init());

  wait_for_wifi_ready();

  httpd_config_t config = HTTPD_DEFAULT_CONFIG();
  config.lru_purge_enable = true;

  // Start the httpd server
  ESP_LOGI(TAG, "Starting server on port: '%d'", config.server_port);
  if (httpd_start(&server_handle, &config) == ESP_OK) {
      // Set URI handlers
      ESP_LOGI(TAG, "Registering URI handlers");
      ESP_ERROR_CHECK(httpd_register_uri_handler(server_handle, &root));
      ESP_ERROR_CHECK(httpd_register_uri_handler(server_handle, &index_html));
      ESP_ERROR_CHECK(httpd_register_uri_handler(server_handle, &joystick_css));
      ESP_ERROR_CHECK(httpd_register_uri_handler(server_handle, &joystick_js));
      ESP_ERROR_CHECK(httpd_register_uri_handler(server_handle, &websocket_joy_msg));
      // TODO: Wait for something to shut down server. Right now we just spin
      while(true) {
        vTaskDelay(portMAX_DELAY);
      }
  }

  ESP_LOGI(TAG, "Error starting server!");
  vTaskDelete(NULL);
}
