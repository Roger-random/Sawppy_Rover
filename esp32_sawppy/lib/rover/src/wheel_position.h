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

// Half of distance between left and right front wheels
static const float track_half_front = 0.1;

// Half of distance between left and right mid wheels
static const float track_half_mid   = 0.135;

// Half of distance between left and right back wheels
static const float track_half_back  = 0.1;

// X and Y coordinate of a wheel relative to rover center
typedef struct wheel_position
{
  float x; // One of wheelbase_* values
  float y; // One of track_half_* values
} wheel_position;

// Array of wheel positions relative to rover center
const wheel_position wheel_positions[wheel_count] = {
  // front_left
  {
    wheelbase_front,
    track_half_front,
  },
  // front_right
  {
    wheelbase_front,
    -track_half_front,
  },
  // mid_left
  {
    0,
    track_half_mid,
  },
  // mid_right
  {
    0,
    -track_half_mid,
  },
  // back_left
  {
    -wheelbase_back,
    track_half_back,
  },
  // back_right
  {
    -wheelbase_back,
    -track_half_back,
  },
};

#endif // #ifndef INC_WHEEL_POSITION_H
