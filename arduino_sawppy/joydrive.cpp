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
}

int JoyDrive::getSteering()
{
  // analogRead returns between 0 and 1023
  int vRaw = analogRead(_steeringPin);

  return normalized(vRaw, INVERT_STEERING);
}

int JoyDrive::getVelocity()
{
  // analogRead returns between 0 and 1023
  int vRaw = analogRead(_velocityPin);

  return normalized(vRaw, INVERT_VELOCITY);
}

int JoyDrive::normalized(int raw, bool invert)
{
  float range = ANALOG_MID - DEAD_ZONE;
  float sign = 0.0;
  int subset = 0;
  
  if (raw > ANALOG_MID + DEAD_ZONE)
  {
    sign = 100.0;
    subset = raw - ANALOG_MID - DEAD_ZONE;
  }
  else if (raw < ANALOG_MID - DEAD_ZONE)
  {
    sign = -100.0;
    subset = ANALOG_MID - DEAD_ZONE - raw;
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

  return sign * subset / range;
}
