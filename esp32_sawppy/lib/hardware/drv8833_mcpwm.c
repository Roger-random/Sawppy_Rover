#include "drv8833_mcpwm.h"

void update_motor_speed(float velocity, mcpwm_motor_control MC)
{
  float duty_cycle = fabs(duty_cycle_max * (velocity / velocity_linear_max));

  if (duty_cycle > 100.0)
  {
    printf("WARNING: Duty cycle capped at 100 percent.\n");
    duty_cycle = 100.0;
  }

  if (velocity > 0)
  {
    mcpwm_set_duty(MC.unit, MC.timer, MCPWM_GEN_A, duty_cycle);
    mcpwm_set_duty(MC.unit, MC.timer, MCPWM_GEN_B, 0);
  }
  else if (velocity < 0)
  {
    mcpwm_set_duty(MC.unit, MC.timer, MCPWM_GEN_A, 0);
    mcpwm_set_duty(MC.unit, MC.timer, MCPWM_GEN_B, duty_cycle);
  }
  else
  {
    mcpwm_set_duty(MC.unit, MC.timer, MCPWM_GEN_A, 0);
    mcpwm_set_duty(MC.unit, MC.timer, MCPWM_GEN_B, 0);
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
    printf("ERROR: drv8833_mcpwm_task parameter is null. Expected handle to queue of wheel commands.\n");
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
    mcpwm_init(
      speed_control[wheel].unit,
      speed_control[wheel].timer,
      &mcpwm_config);
    mcpwm_gpio_init(
      speed_control[wheel].unit, 
      speed_control[wheel].signalA, 
      speed_control[wheel].gpioA);
    mcpwm_gpio_init(
      speed_control[wheel].unit, 
      speed_control[wheel].signalB, 
      speed_control[wheel].gpioB);
    current_speed[wheel] = 0.0;
    update_motor_speed(current_speed[wheel], speed_control[wheel]);
  }

  while(true)
  {
    // Wait for next joystick message
    if (pdTRUE == xQueueReceive(xWheelQueue, &message, wheel_msg_timeout_interval))
    {
      for (int wheel = 0; wheel < wheel_count; wheel++)
      {
        current_speed[wheel] = 0.3;
        update_motor_speed(current_speed[wheel], speed_control[wheel]);
      }
    }
    else
    {
      printf("drv8833_mcpwm: No wheel message received within timeout, stopping all motors.\n");
      // In case of timeout, ramping is ignored. We go to zero immediately.
      for (int wheel = 0; wheel < wheel_count; wheel++)
      {
        current_speed[wheel] = 0.0;
        update_motor_speed(current_speed[wheel], speed_control[wheel]);
      }
    }
  }
}
