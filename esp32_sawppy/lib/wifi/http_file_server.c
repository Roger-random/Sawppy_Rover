#include "http_file_server.h"

static httpd_handle_t server_handle;

/* Waiting for certain WiFi events before continuing */
static EventGroupHandle_t s_wifi_event_group;

static QueueHandle_t xJoystickQueue;

static const char *TAG = "http file server";

// We can spend effort making code memory-efficient, or we can just blow
// a chunk of RAM to make code trivial.
#define readBufSize 1024*16
static char readBuf[readBufSize];

static void wifi_connected_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
  if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
    xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
  }
}

void wait_for_wifi_ready()
{
  // Used by WiFi event handler to signal we can continue
  s_wifi_event_group = xEventGroupCreate();

  // Register for WiFi event
  esp_event_handler_instance_t instance_got_ip;
  ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
    IP_EVENT_STA_GOT_IP, &wifi_connected_handler, NULL, &instance_got_ip));

  EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group, WIFI_CONNECTED_BIT, pdFALSE, pdFALSE, portMAX_DELAY);
  assert(bits == WIFI_CONNECTED_BIT); // We only have one bit today.

  // No longer need to listen to WiFi event
  ESP_ERROR_CHECK(esp_event_handler_instance_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP, instance_got_ip));
  vEventGroupDelete(s_wifi_event_group);
}

void spiffs_init()
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
        return;
    }

    size_t total = 0, used = 0;
    ret = esp_spiffs_info(conf.partition_label, &total, &used);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to get SPIFFS partition information (%s)", esp_err_to_name(ret));
    } else {
        ESP_LOGI(TAG, "Partition size: total: %d, used: %d", total, used);
    }
}

static uint32_t read_spiff_buffer(const char *file_name)
{
  uint32_t readSize = 0;
  FILE* f = fopen(file_name, "r");
  if (f == NULL) {
      ESP_LOGE(TAG, "Failed to open file for reading");
      return 0;
  }

  readSize = fread(readBuf, sizeof(char), readBufSize, f);
  fclose(f);

  return readSize;
}

static esp_err_t index_html_get_handler(httpd_req_t *req)
{
  uint32_t readSize = 0;
  ESP_ERROR_CHECK(httpd_resp_set_hdr(req, "cache-control", "max-age=1")); // For development
  ESP_ERROR_CHECK(httpd_resp_set_type(req, "text/html"));
  readSize = read_spiff_buffer("/static/index.html");
  ESP_ERROR_CHECK(httpd_resp_send(req, readBuf, readSize));

  return ESP_OK;
}

static const httpd_uri_t index_html = {
  .uri      = "/index.html",
  .method   = HTTP_GET,
  .handler  = index_html_get_handler,
  .user_ctx = NULL
};

static const httpd_uri_t root = {
  .uri      = "/",
  .method   = HTTP_GET,
  .handler  = index_html_get_handler,
  .user_ctx = NULL
};

static esp_err_t joystick_css_get_handler(httpd_req_t *req)
{
  uint32_t readSize = 0;
  ESP_ERROR_CHECK(httpd_resp_set_hdr(req, "cache-control", "max-age=1")); // For development
  ESP_ERROR_CHECK(httpd_resp_set_type(req, "text/css"));
  readSize = read_spiff_buffer("/static/joystick.css");
  ESP_ERROR_CHECK(httpd_resp_send(req, readBuf, readSize));

  return ESP_OK;
}

static const httpd_uri_t joystick_css = {
  .uri      = "/joystick.css",
  .method   = HTTP_GET,
  .handler  = joystick_css_get_handler,
  .user_ctx = NULL
};

static esp_err_t joystick_js_get_handler(httpd_req_t *req)
{
  uint32_t readSize = 0;
  ESP_ERROR_CHECK(httpd_resp_set_hdr(req, "cache-control", "max-age=1")); // For development
  ESP_ERROR_CHECK(httpd_resp_set_type(req, "application/javascript"));
  readSize = read_spiff_buffer("/static/joystick.js");
  ESP_ERROR_CHECK(httpd_resp_send(req, readBuf, readSize));

  return ESP_OK;
}

static const httpd_uri_t joystick_js = {
  .uri      = "/joystick.js",
  .method   = HTTP_GET,
  .handler  = joystick_js_get_handler,
  .user_ctx = NULL
};

esp_err_t sendJoyMsg(float steer, float speed) {
  joy_msg message;

  memset(&message, 0, sizeof(joy_msg));
  message.axes[0] = steer;
  message.axes[1] = speed;

  message.timeStamp = xTaskGetTickCount();

  xQueueOverwrite(xJoystickQueue, &message);

  return ESP_OK;
}

struct websock_instance {
  httpd_handle_t handle;
  int descriptor;
};

static struct websock_instance sJoyMsgSender;
static TickType_t tWebsockTimeoutPeriod = pdMS_TO_TICKS(500);
static TimerHandle_t xWebsockTimeout;

void socket_close_cleanup(void* context)
{
  ESP_LOGI(TAG, "Lost our joy_msg handler.");
  sJoyMsgSender.handle = NULL;
  sJoyMsgSender.descriptor = 0;
  if (pdPASS != xTimerStop(xWebsockTimeout, 0)) {
    ESP_LOGE(TAG, "Failed to stop websock timeout timer");
  }
}

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

static void vWebsockTimeoutCallback( TimerHandle_t xTimer)
{
  ESP_ERROR_CHECK(httpd_queue_work(sJoyMsgSender.handle, vWebsockClose, NULL));
}

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
    cJSON *root = cJSON_Parse(readBuf);
    cJSON *axes = cJSON_GetObjectItem(root,"axes");
    cJSON *axes0 = cJSON_GetArrayItem(axes, 0);
    cJSON *axes1 = cJSON_GetArrayItem(axes, 1);
    ESP_ERROR_CHECK(sendJoyMsg(axes0->valuedouble, axes1->valuedouble));
    cJSON_Delete(root);
  } else {
    ESP_LOGE(TAG, "Ignoring oversized WebSocket packet");
    return ESP_FAIL;
  }

  if (pdPASS != xTimerReset(xWebsockTimeout, 0)) {
    ESP_LOGE(TAG, "Failed to reset websock timeout timer");
    return ESP_FAIL;
  }

  return ESP_OK;
}

static const httpd_uri_t websocket_joy_msg = {
  .uri      = "/joy_msg",
  .method   = HTTP_GET,
  .handler  = websocket_joy_msg_handler,
  .user_ctx = NULL,
  .is_websocket = true
};

void http_file_server_task(void* pvParameters)
{
  xJoystickQueue = (QueueHandle_t)pvParameters;

  sJoyMsgSender.handle = NULL;
  sJoyMsgSender.descriptor = 0;

  xWebsockTimeout = xTimerCreate("Websock Timeout", tWebsockTimeoutPeriod,
    pdFALSE, 0, vWebsockTimeoutCallback);

  if (NULL==xWebsockTimeout) {
    ESP_LOGE(TAG, "Failed to create websock timeout timer");
    vTaskDelete(NULL);
  }

  spiffs_init();

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
