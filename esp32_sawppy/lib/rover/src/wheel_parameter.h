/*
 * @brief Rover chassis wheel parameters
 * Lengths in meters, angles in radians, coordinates as per REP103
 * https://www.ros.org/reps/rep-0103.html
 */

#ifndef INC_WHEEL_PARAMETER_H
#define INC_WHEEL_PARAMETER_H

// Enumeration for indexing into wheel information array
enum wheel_name {
  front_left,
  front_right,
  mid_left,
  mid_right,
  back_left,
  back_right,
  wheel_count,
};

// Distance from front wheel to middle wheel.
static const float wheelbase_front  = 0.14;

// Distance from back wheel to mid wheel.
static const float wheelbase_back   = 0.14;

// Distance between left and right front wheels
static const float track_front = 0.2;

// Distance between left and right mid wheels
static const float track_mid   = 0.27;

// Distance between left and right back wheels
static const float track_back  = 0.2;

// X and Y coordinate of a wheel relative to rover center
typedef struct wheel_position
{
  float x; // One of wheelbase_* values
  float y; // One of track_* values, divided by two
} wheel_position;

// Array of wheel positions relative to rover center
static const wheel_position wheel_positions[wheel_count] = {
  // front_left
  {
    wheelbase_front,
    track_front/2,
  },
  // front_right
  {
    wheelbase_front,
    -track_front/2,
  },
  // mid_left
  {
    0,
    track_mid/2,
  },
  // mid_right
  {
    0,
    -track_mid/2,
  },
  // back_left
  {
    -wheelbase_back,
    track_back/2,
  },
  // back_right
  {
    -wheelbase_back,
    -track_back/2,
  },
};

// Rover max speed (meters/second) dictated by wheel motors
// Achieved when going straight forwards or backwards. This might not be exact
// due to variations from motor to motor, but close enough to serve as limit.
static const float velocity_linear_max = 0.5;

// Rover max turning speed achieved by running the motors at max speed
// while turning in place. (Not a very practical motion but it's the max.)
// Calculated from circle with diameter of track_mid, the distance on its
// circumference covered by velocity_linear_max gives us the angular velocity
// in radians/second.
//
//   velocity_linear_max         velocity_angular_max
//  ---------------------   =   ----------------------
//     track_mid * Pi                  2 * Pi
//
// Again this does not need to be exact, just close enough to use as limit.
static const float velocity_angular_max = velocity_linear_max * 2 / track_mid;

#endif // #ifndef INC_WHEEL_PARAMETER_H
