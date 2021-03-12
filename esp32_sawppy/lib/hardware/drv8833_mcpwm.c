#include "drv8833_mcpwm.h"

void update_motor_speed(float velocity, mcpwm_motor_control MC)
{
  float duty_cycle;

  if (fabs(velocity) < wheel_speed_min)
  {
    // Below minimum. Don't bother sending power as it'd just
    // turn into heat with stalled motor.
    duty_cycle = 0.0;
  }
  else
  {
    duty_cycle = duty_cycle_min + (fabs(velocity)/wheel_speed_max) * (duty_cycle_max - duty_cycle_min);
  }

  if (duty_cycle > duty_cycle_max)
  {
    // Possible to exceed max in certain travel geometries.
    // Exampel: outermost wheel moves faster than rover body
    duty_cycle = duty_cycle_max;
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

        update_motor_speed(newSpeed, speed_control[wheel]);
        current_speed[wheel] = newSpeed;
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
