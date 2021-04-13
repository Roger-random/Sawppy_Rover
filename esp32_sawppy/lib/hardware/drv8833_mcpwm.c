#include "drv8833_mcpwm.h"

static const char *TAG = "drv8833_mcpwm";

/*
 * @brief Updates MCPWM duty cycles to deliver given velocity
 * @param velocity Target velocity in meters per second
 * @param MC Specify which MCPWM peripheral to update
 */
void update_motor_speed(float velocity, mcpwm_motor_control MC)
{
  float duty_cycle;

  if (fabs(velocity) < wheel_speed_epsilon)
  {
    // Close enough to be zero to be treated as zero.
    duty_cycle = 0.0;
  }
  else if (fabs(velocity) < wheel_speed_min)
  {
    // Below the level our DC motor can sustain running by itself. However
    // it might have help from other wheels so we feed it enough power to
    // barely overcome geartrain friction
    duty_cycle = duty_cycle_min;
  }
  else
  {
    duty_cycle = duty_cycle_min + (fabs(velocity)/wheel_speed_max) * (duty_cycle_max - duty_cycle_min);
  }

  if (duty_cycle > 100.0)
  {
    // Velocity should not exceed wheel_speed_max. If it happens, that's a bug
    // elsewhere in this code. But if it happens anyway, clamp value and emit
    // warning message.
    duty_cycle = 100.0;
    ESP_LOGW(TAG, "Motor speed velocity %+.2f exceeded specified maximum %+.2f.", velocity, wheel_speed_max);
  }

  if (velocity > 0)
  {
    ESP_ERROR_CHECK(mcpwm_set_duty(MC.unit, MC.timer, MCPWM_GEN_A, duty_cycle));
    ESP_ERROR_CHECK(mcpwm_set_duty(MC.unit, MC.timer, MCPWM_GEN_B, 0));
  }
  else if (velocity < 0)
  {
    ESP_ERROR_CHECK(mcpwm_set_duty(MC.unit, MC.timer, MCPWM_GEN_A, 0));
    ESP_ERROR_CHECK(mcpwm_set_duty(MC.unit, MC.timer, MCPWM_GEN_B, duty_cycle));
  }
  else
  {
    ESP_ERROR_CHECK(mcpwm_set_duty(MC.unit, MC.timer, MCPWM_GEN_A, 0));
    ESP_ERROR_CHECK(mcpwm_set_duty(MC.unit, MC.timer, MCPWM_GEN_B, 0));
  }
}

void drv8833_mcpwm_task(void* pvParam)
{
  wheel_msg message;
  float current_speed[wheel_count] = {
    0.0, 0.0, 0.0, 0.0, 0.0, 0.0
  };

  // Retrieve input queue
  if (NULL == pvParam)
  {
    ESP_LOGE(TAG, "drv8833_mcpwm_task parameter is null. Expected handle to queue of wheel commands.");
    vTaskDelete(NULL); // Delete self.
  }
  QueueHandle_t xWheelQueue = (QueueHandle_t)pvParam;

  // Configure MCPWM peripheral
  mcpwm_config_t mcpwm_config = {
    .frequency = pwm_freq,
    .cmpr_a = 0,
    .cmpr_b = 0,
    .counter_mode = MCPWM_UP_COUNTER,
    .duty_mode = MCPWM_DUTY_MODE_0,
  };
  for (int wheel = 0; wheel < wheel_count; wheel++)
  {
    ESP_ERROR_CHECK(mcpwm_init(
      speed_control[wheel].unit,
      speed_control[wheel].timer,
      &mcpwm_config));
    ESP_ERROR_CHECK(mcpwm_gpio_init(
      speed_control[wheel].unit,
      speed_control[wheel].signalA,
      speed_control[wheel].gpioA));
    ESP_ERROR_CHECK(mcpwm_gpio_init(
      speed_control[wheel].unit,
      speed_control[wheel].signalB,
      speed_control[wheel].gpioB));
    current_speed[wheel] = 0.0;
    update_motor_speed(current_speed[wheel], speed_control[wheel]);
  }

  bool haltNotify = true;
  while(true)
  {
    // Wait for next joystick message
    if (pdTRUE == xQueueReceive(xWheelQueue, &message, wheel_msg_timeout_interval))
    {
      haltNotify = true;
      for (int wheel = 0; wheel < wheel_count; wheel++)
      {
        float currentSpeed = current_speed[wheel];
        float targetSpeed = message.speed[wheel];
        float newSpeed;

        // newSpeed speed moves towards targetSpeed, limited by acceleration cap
        if (targetSpeed > currentSpeed + wheel_accel_max)
        {
          newSpeed = currentSpeed + wheel_accel_max;
        }
        else if (targetSpeed < currentSpeed - wheel_accel_max)
        {
          newSpeed = currentSpeed - wheel_accel_max;
        }
        else
        {
          newSpeed = targetSpeed;
        }

        // If we are switching directions, minimum stopped deadtime of one update cycle.
        if ((currentSpeed < 0 && newSpeed > 0) || (currentSpeed > 0 && newSpeed < 0))
        {
          newSpeed = 0;
        }

        // If starting from below minimum, give motor a startup kick.
        if (currentSpeed < wheel_speed_min && newSpeed > wheel_speed_min)
        {
          newSpeed = wheel_speed_startup;
        }
        else if (currentSpeed > -wheel_speed_min && newSpeed < -wheel_speed_min)
        {
          newSpeed = -wheel_speed_startup;
        }

        current_speed[wheel] = newSpeed;
      }

      // Calculated velocities has been stored in current_speed[]. But due to
      // rover geometry, it is possible for some wheels to end up with velocity
      // higher than wheel_speed_max. If so, scale every value of the range
      // [wheel_speed_startup, calc_max] to [wheel_speed_startup, wheel_speed_max]
      float calc_max = 0;
      for (int wheel = 0; wheel < wheel_count; wheel++)
      {
        if (fabs(current_speed[wheel]) > calc_max)
        {
          calc_max = fabs(current_speed[wheel]);
        }
      }

      if (calc_max > wheel_speed_max)
      {
        float scale = (wheel_speed_max-wheel_speed_startup) /
                      (calc_max-wheel_speed_startup);
        float magnitude = 0;
        for (int wheel = 0; wheel < wheel_count; wheel++)
        {
          magnitude = fabs(current_speed[wheel]);
          if (magnitude > wheel_speed_startup)
          {
            magnitude -= wheel_speed_startup;
            magnitude *= scale;
            magnitude += wheel_speed_startup;

            current_speed[wheel] = copysignf(magnitude, current_speed[wheel]);
          }
        }
      }

      // All speed adjustments complete, send results to motor controllers.
      for (int wheel = 0; wheel < wheel_count; wheel++)
      {
        update_motor_speed(current_speed[wheel], speed_control[wheel]);
      }
    }
    else
    {
      if (haltNotify) {
        haltNotify = false; // Once per timeout is enough
        ESP_LOGE(TAG, "No wheel message received within timeout, stopping all motors.");
      }
      // In case of timeout, ramping is ignored. We go to zero immediately.
      for (int wheel = 0; wheel < wheel_count; wheel++)
      {
        current_speed[wheel] = 0.0;
        update_motor_speed(current_speed[wheel], speed_control[wheel]);
      }
    }
  }
}
