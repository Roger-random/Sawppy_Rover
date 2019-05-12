/*
 * Arduino sketch to drive a Sawppy rover using a joystick (two potentiometers)
 * wired to analog pins. After calculations based on rover geometry, resulting
 * angle and velocity information can be sent to different types of outputs.
 *
 * http://sawppy.com
 *
 * LewanSoul serial bus servo control code (in lewansoul.cpp) subject to LewanSoul terms and conditions.
 *
 * Remaining code created by Roger Cheng are released under MIT license.
 */

#include <math.h>
#include "Arduino.h"
#include "joydrive.h"
#include "lewansoul.h"

// Output select - only one of the following should be uncommented
#define LEWANSOUL 1 // Output to LewanSoul serial bus servo commands
//#define PRINTCMD 1 // Output to serial debug monitor
// Output select end

#define STEERING_PIN 1 // Analog pin to control steering angle
#define VELOCITY_PIN 0 // Analog pin to control speed
#define INPLACE_BUTTON 2 // Button to trigger turn-in-place mode.

// Initialize joystick module
JoyDrive jd(STEERING_PIN, VELOCITY_PIN);

#ifdef LEWANSOUL
LewanSoul lss(false);
#endif

// Hold information about wheel arrangement on rover. This information
// is fixed and does not change while the program runs.
typedef struct RoverWheel
{
  float x; // How far this wheel is to the right (positive) or left (negative) of center
  float y; // How far this wheel is to the ahead (positive) or behind (negative) center
  int rollServoId; // Serial bus ID of servo responsible for wheel rolling
  bool rollServoInverted; // Whether to invert wheel rolling front/back direction
  int steerServoId; // Serial bus ID of servo responsible for wheel steering
  float steerTrim; // Adjustment in angle degrees to trim steering center position
} RoverWheel;

// Array of wheels on rover
const RoverWheel Chassis[] = {
  // front left
  {
    -9.125, // x
    11.375, // y
    25,     // roll ID
    false,  // roll inverted
    23,     // steer ID
    -4      // steer trim
  },
  // front right
  {
     9.125, // x
    11.375, // y
    27,     // roll ID
    true,   // roll inverted
    29,     // steer ID
    -4      // steer trim
  },
  // mid left
  {
    -10.375,// x
    0,      // y
    21,     // roll ID
    false,  // roll inverted
    -1,     // steer ID
    0       // steer trim
  },
  // mid right
  {
    10.375, // x
    0,      // y
    22,     // roll ID
    true,   // roll inverted
    -1,     // steer ID
    0       // steer trim
  },
  // rear left
  {
    -9,     // x
    -10,    // y
    20,     // roll ID
    false,  // roll inverted
    24,     // steer ID
    2       // steer trim
  },
  // rear right
  {
     9,     // x
    -10,    // y
    28,     // roll ID
    true,   // roll inverted
    26,     // steer ID
    4       // steer trim
  }
};

// Index into Chassis[] used for calculating steering angles
#define FRONT_LEFT 0
#define FRONT_RIGHT 1
#define MID_LEFT 2

// Using information above, the maximum steering angle for a front wheel
float maxSteering;

// Store calculation of servo angle and speed before we send them as commands
// Also stores turning radius (hypotenuse in trig calculations) which affects speed.
// This information is updated on every loop.
typedef struct ServoCommand {
  float angle;
  float radius;
  float speed;
} ServoCommand;

ServoCommand servoCommands[6];

// Use the servoCommands[].radius values to scale all servoCommands[].speed relative
// to a given velocity.
void speedFromRadius(float maxSpeed)
{
  int wheel; // iterator for wheels on a chassis
  float maxRadius = 0.0; // Turning radius of wheel furthest from turn center

  // Find maximum radius of all wheels
  for (wheel = 0; wheel < 6; wheel++)
  {
    if (abs(servoCommands[wheel].radius) > maxRadius)
    {
      maxRadius = abs(servoCommands[wheel].radius);
    }
  }

  // Max radius found, now scale all wheel speeds so those with max radius spins
  // at commanded velocity and other wheels at slower speed based on radius ratio.
  for (wheel = 0; wheel < 6; wheel++)
  {
    servoCommands[wheel].speed = (servoCommands[wheel].radius/maxRadius)*maxSpeed;
  }
}

// Runs once upon powerup
void setup()
{
  // Configure button pins
  pinMode(INPLACE_BUTTON, INPUT);
  digitalWrite(INPLACE_BUTTON, HIGH); // Button pulls LOW when pressed

#ifdef LEWANSOUL
  // LewanSoul serial servo code is taking over Serial port.
  lss.setup();
#endif

#ifdef PRINTCMD
  // Use serial port to print our calculated commands
  Serial.begin(9600);
#endif

  // Calculate maximum steering angle
  float adjacent = Chassis[MID_LEFT].x - Chassis[FRONT_LEFT].x;
  float opposite = Chassis[FRONT_LEFT].y;
  maxSteering = abs(atan(opposite/adjacent)*180.0/M_PI);
}

// Runs regularly as long as there is power to Arduino
void loop()
{
  int steering; // -100 to 100, retrieved by JoyDrive::getSteering()
  int velocity; // -100 to 100, retrieved by JoyDrive::getVelocity()
  int wheel; // Iterator index
  float turnCenterX; // Position of center of turn, which lies on the X axis
  float inRadians; // Angle calculation in radians

  int invert; // Multiplier for implementing RoverWheel.rollServoInverted
  int referenceWheel; // Wheel used to calculate turnCenterX

  bool turnInPlace = (digitalRead(INPLACE_BUTTON) == LOW); // Read button that commands turn-in-place mode.
  delay(100);
  steering = jd.getSteering(); // Read steering potentiometer
  delay(100);
  velocity = jd.getVelocity(); // Read velocity potentiometer

#if PRINTCMD
  Serial.print("S");
  Serial.print(steering);
  Serial.print(" V");
  Serial.print(velocity);
  Serial.print(" ");
#endif

  if (turnInPlace)
  {
    // Turn in place mode: point all wheels at center so we can rotate about center.
    for (wheel = 0; wheel < 6; wheel++)
    {
      // Calculate angles to point at center
      if (Chassis[wheel].y == 0)
      {
        servoCommands[wheel].angle = 0;
        servoCommands[wheel].radius = abs(Chassis[wheel].x);
      }
      else
      {
        servoCommands[wheel].angle = -atan(Chassis[wheel].y/Chassis[wheel].x)*180.0/M_PI;
        servoCommands[wheel].radius = sqrt(pow(Chassis[wheel].x,2)+pow(Chassis[wheel].y,2));
      }

      // When turning in place, one side wheels turn opposite the other. Change direction of
      // this inequality comparison to reverse turn-in-place direction relative to joystick.
      if (Chassis[wheel].x > 0)
      {
        servoCommands[wheel].radius *= -1;
      }
    }

    // Calculate speed from relative radii
    speedFromRadius(steering);
  }
  else
  {
    // Choose a reference wheel and, from there, calculate turn center X
    if (steering == 0)
    {
      // Rolling straight ahead or behind
      referenceWheel = -1;
      turnCenterX = 0;
    }
    else
    {
      if (steering > 0)
      {
        referenceWheel = FRONT_RIGHT;
      }
      else
      {
        referenceWheel = FRONT_LEFT;
      }

      // Rover motion is based on joystick commanding angle of reference wheel
      servoCommands[referenceWheel].angle = maxSteering * steering / 100.0;

      // From there, calculate the center of rotation we'll use to calculate remaining wheels
      inRadians = servoCommands[referenceWheel].angle * M_PI / 180.0;
      turnCenterX = Chassis[referenceWheel].x + (Chassis[referenceWheel].y / tan(inRadians));

      // Store length of hypotenuse for later speed calculation
      servoCommands[referenceWheel].radius = abs(Chassis[referenceWheel].y / sin(inRadians));
    }

    // Calculate other wheel angles based on turn center X
    for (wheel = 0; wheel < 6; wheel++)
    {
      if (turnCenterX == 0)
      {
        // Rolling straight ahead or behind
        servoCommands[wheel].angle = 0;
        servoCommands[wheel].speed = velocity;
      }
      else if (wheel != referenceWheel)
      {
        // The X-axis distance between this wheel and turn center
        float wheelToCenter = turnCenterX - Chassis[wheel].x;

        if (Chassis[wheel].steerServoId != -1)
        {
          // Calculate wheel angle
          inRadians = atan(Chassis[wheel].y/wheelToCenter);

          // Convert to degrees and store command
          servoCommands[wheel].angle = inRadians*180.0/M_PI;

          // Store hypotenuse (turning radius) in speed for later calculation.
          servoCommands[wheel].radius = abs(Chassis[wheel].y/sin(inRadians));
        }
        else
        {
          // No steering servo, but good to blank out data anyway.
          servoCommands[wheel].angle = 0;

          // This wheel lacks steering servo. This should only happen on the wheels
          // aligned with turn axis. (wheel coordinate y of zero.) So their turning
          // radius ("hypotenuse" in all the other trig calculations) becomes their
          // X relative to turn center.
          servoCommands[wheel].radius = abs(wheelToCenter);
        }
      }
    }

    if (abs(turnCenterX) > 0)
    {
      // Calculate speed from relative radii
      speedFromRadius(velocity);
    }
  }
  
#ifdef LEWANSOUL
  // All angles and speeds calculated, send the commands accounting
  // for steering trim offset and inverting speed where needed
  for (wheel = 0; wheel < 6; wheel++)
  {
    if (Chassis[wheel].steerServoId != -1)
    {
      lss.moveTo(Chassis[wheel].steerServoId, servoCommands[wheel].angle + Chassis[wheel].steerTrim);
    }

    if (Chassis[wheel].rollServoInverted)
    {
      invert = -1;
    }
    else
    {
      invert = 1;
    }
    lss.spinAt(Chassis[wheel].rollServoId, servoCommands[wheel].speed * invert);
  }
#endif

#ifdef PRINTCMD
  for (wheel = 0; wheel < 6; wheel++)
  {
    Serial.print(" W");
    Serial.print(wheel);
    Serial.print(" ");
    if (Chassis[wheel].steerServoId != -1)
    {
      Serial.print(servoCommands[wheel].angle + Chassis[wheel].steerTrim);
      Serial.print("deg ");
    }

    if (Chassis[wheel].rollServoInverted)
    {
      invert = -1;
    }
    else
    {
      invert = 1;
    }
    Serial.print(servoCommands[wheel].speed * invert);
    Serial.print("pct ");
  }
  Serial.println();
#endif

}
