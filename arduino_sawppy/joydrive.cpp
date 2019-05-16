/* 
  *  JoyDrive = Joystick Drive
  * ------------
  *
  * Reads two analog pins that are connected to potentiometers representing two
  * axis of a joystick. One axis represents steering angle, one axis represents
  * velocity. Each is normalized within a range from -100 to 100, so the sign
  * represents direction and number represents magnitude as a percentage.
  * 
  */

#include "joydrive.h"

JoyDrive::JoyDrive(int steeringPin, int velocityPin)
{
  _steeringPin = steeringPin;
  _velocityPin = velocityPin;
  _previousSteering = 0;
  _previousVelocity = 0;
}

int JoyDrive::getSteering()
{
  // analogRead returns between 0 and 1023
  int steeringRaw = analogRead(_steeringPin);
  int steeringNormalized  = normalized(steeringRaw, STEER_MID, INVERT_STEERING);
  int steeringConstrained = constrained(steeringNormalized, _previousSteering, MAX_DELTA_STEERING);

  _previousSteering = steeringConstrained;

  return steeringConstrained;
}

int JoyDrive::getVelocity()
{
  // analogRead returns between 0 and 1023
  int velocityRaw = analogRead(_velocityPin);
  int velocityNormalized = normalized(velocityRaw, VEL_MID, INVERT_VELOCITY);
  int velocityConstrained = constrained(velocityNormalized, _previousVelocity, MAX_DELTA_VELOCITY);

  _previousVelocity = velocityConstrained;

  return velocityConstrained;
}

// The normalized() function accepts a value in the full range of ADC (0-1023) and
// turns it into a range from -100 to 100 with 0 at the given midpoint.
int JoyDrive::normalized(int raw, int midpoint, bool invert)
{
  float range = ADC_MID - DEAD_ZONE;
  float sign = 0.0;
  int subset = 0;
  int normalized = 0;
  
  if (raw > midpoint + DEAD_ZONE)
  {
    sign = 100.0;
    subset = raw - midpoint - DEAD_ZONE;
  }
  else if (raw < midpoint - DEAD_ZONE)
  {
    sign = -100.0;
    subset = midpoint - DEAD_ZONE - raw;
  }
  else
  {
    sign = 100.0;
    subset = 0;
  }

  if (invert)
  {
    sign = sign * -1;
  }

  normalized = sign * subset / range;

  // Due to how midpoint might vary, cap the maximums.
  if (normalized > 100)
  {
    normalized = 100;
  }

  if (normalized < -100)
  {
    normalized = -100;
  }

  return normalized;
}

// The constrained() function accepts a normalized value and returns a
// value that is not more than maxDelta away from previous value.
int JoyDrive::constrained(int normalized, int previous, int maxDelta)
{
  int constrained = 0;

  if (normalized > previous + maxDelta)
  {
    constrained = previous + maxDelta;
  }
  else if (normalized < previous - maxDelta)
  {
    constrained = previous - maxDelta;
  }
  else
  {
    constrained = normalized;
  }

  return constrained;
}
