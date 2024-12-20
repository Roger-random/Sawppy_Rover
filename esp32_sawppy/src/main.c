/*

Main startup for Sawppy Rover ESP32 controller

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
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

#include <joy_msg.h>
#include <twist_msg.h>
#include <wheel_msg.h>
#include <msg_print.h>

#include "gpio_assignments.h"

#ifdef USE_JOY_ADC
#include <joy_adc.h>
#endif
#ifdef USE_JOY_RMT_RC
#include <joy_rmt_rc.h>
#endif

#include "volt_adc.h"

#ifdef USE_WIFI
#include "softap_start.h"
#include "station_start.h"
#include "http_file_server.h"
#endif

#include <joy_steer.h>
#include <wheel_ackermann.h>
#include <servo_steer_ledc.h>
#include <drv8833_mcpwm.h>

void app_main()
{
  QueueHandle_t xJoystickQueue = xQueueCreate(1, sizeof(joy_msg));
  QueueHandle_t xCmdVelQueue = xQueueCreate(1, sizeof(twist_msg));
  QueueHandle_t xWheelQueue = xQueueCreate(1, sizeof(wheel_msg));
  QueueHandle_t xPowerQueue = xQueueCreate(1, sizeof(power_msg));

#ifdef USE_WIFI
  ESP_ERROR_CHECK(esp_event_loop_create_default());
  http_file_server_task_parameters http_params = {
    .xJoystickQueue = xJoystickQueue,
    .xPowerQueue = xPowerQueue,
  };
#endif

  joy_steer_task_parameters joy_steer_params = {
    .xJoyMsgQueue = xJoystickQueue,
    .xCmdVelQueue = xCmdVelQueue,
  };

  wheel_ackermann_task_parameters wheel_ackermann_params = {
    .xCmdVelQueue = xCmdVelQueue,
    .xWheelQueue = xWheelQueue,
  };

  if (NULL == xJoystickQueue || NULL == xCmdVelQueue || NULL == xWheelQueue || NULL == xPowerQueue)
  {
    printf("ERROR: Queue allocation failed.");
  }
  else
  {
#ifdef USE_JOY_ADC
    xTaskCreate(joy_adc_read_task, "joy_adc_read_task", 2048, xJoystickQueue, 20, NULL);
#endif

#ifdef USE_JOY_RMT_RC
    xTaskCreate(joy_rmt_rc_read_task, "joy_rmt_rc_read_task", 2048, xJoystickQueue, 20, NULL);
#endif

#ifdef USE_WIFI
    xTaskCreate(softap_start_task, "softap_start_task", 1024*3, NULL, 20, NULL);
    //xTaskCreate(station_start_task, "station_start_task", 1024*3, NULL, 20, NULL);
    xTaskCreate(http_file_server_task, "http_file_server_task", 1024*4, &http_params, 19, NULL);

    xTaskCreate(volt_adc_task, "volt_adc_task", 2048, xPowerQueue, 10, NULL);
    //xTaskCreate(power_msg_print_task, "power_msg_print_task", 2048, xPowerQueue, 14, NULL);
#endif

    //xTaskCreate(joy_msg_print_task, "joy_msg_print_task", 2048, xJoystickQueue, 25, NULL);
    xTaskCreate(joy_steer_task, "joy_steer_task", 2048, &joy_steer_params, 15, NULL);
    //xTaskCreate(twist_msg_print_task, "twist_msg_print_task", 2048, xCmdVelQueue, 25, NULL);
    xTaskCreate(wheel_ackermann_task, "wheel_ackermann_task", 2048, &wheel_ackermann_params, 14, NULL);
    //xTaskCreate(wheel_msg_print_task, "wheel_msg_print_task", 2048, xWheelQueue, 25, NULL);
    xTaskCreate(servo_steer_ledc_task, "servo_steer_ledc_task", 2048, xWheelQueue, 13, NULL);
    xTaskCreate(drv8833_mcpwm_task, "drv8833_mcpwm_task", 2048, xWheelQueue, 12, NULL);
  }
}
