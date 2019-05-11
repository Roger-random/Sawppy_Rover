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

#ifndef joydrive_h
#define joydrive_h

#include "arduino.h"

#define ANALOG_MID 512
#define DEAD_ZONE 3

#define INVERT_STEERING false
#define INVERT_VELOCITY false

class JoyDrive
{
  public:
    JoyDrive(int steeringPin, int velocityPin);

    int getSteering();
    int getVelocity();

  private:
    int normalized(int raw, bool invert);

    int _steeringPin;
    int _velocityPin;
};
#endif // joydrive_h
