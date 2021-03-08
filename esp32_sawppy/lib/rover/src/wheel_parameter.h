/*
 * @brief Rover chassis geometry - wheel position
 * Units are in meters
 */

#ifndef INC_WHEEL_POSITION_H
#define INC_WHEEL_POSITION_H

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
const wheel_position wheel_positions[wheel_count] = {
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

#endif // #ifndef INC_WHEEL_POSITION_H
