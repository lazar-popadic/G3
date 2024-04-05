/*
 * movement.c
 *
 *  Created on: Jan 2, 2024
 *      Author: lazar
 */

#include "movement.h"
#include "../regulation/position/position.h"
#include "../regulation/regulation.h"
#include "../odometry/odometry.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include "../timer/timer.h"
#include "../tactics/task_modules.h"

#define W_LIMIT		0.0001
#define V_LIMIT		0.00005

// meri
extern volatile position robot_position;
extern volatile float V_m_s;
extern volatile float w_rad_s;
// zadajem
volatile position target_position =
  { 0, 0, 0 };
//izracuna
volatile position error =
  { 0, 0, 0 };
volatile float theta_to_pos_target = 0;
volatile float theta_to_pos = 0;
volatile float distance;
volatile float theta_to_angle = 0;

volatile bool movement_init = false;
volatile static position pos_init =
  { 0, 0, 0 };

int8_t init_rot_dir = 0, final_rot_dir = 0, tran_dir = 1;
extern volatile uint8_t regulation_phase;
extern volatile float V_limit, w_limit;
volatile float transition_factor = 1.0;
target offset;
uint8_t hold_position_state = 0;
extern volatile bool robot_moving;
extern volatile uint8_t brake;

void
calculate_movement ()
{
//  switch (hold_position_state)
//    {
//    case 0:
  error.x_mm = target_position.x_mm - robot_position.x_mm;		// [mm]
  error.y_mm = target_position.y_mm - robot_position.y_mm;		// [mm]
  theta_to_pos_target = atan2 (error.y_mm, error.x_mm) + tran_dir * M_PI;

  error.theta_rad = target_position.theta_rad - robot_position.theta_rad; // [rad]

  theta_to_pos = simple_normalize (
      theta_to_pos_target - robot_position.theta_rad);	// [rad]
  distance = (tran_dir * (-2) + 1)
      * sqrt (error.x_mm * error.x_mm + error.y_mm * error.y_mm);	// [mm]
  theta_to_angle = simple_normalize (error.theta_rad);		// [rad]
//      break;
//    case 1:
//      theta_to_pos = 0;
//      distance = 0;
//      theta_to_angle = 0;
//      break;
//    }
}

bool
no_movement ()
{
  if (fabs (w_rad_s) < W_LIMIT * transition_factor
      && fabs (V_m_s) < V_LIMIT * transition_factor)
    return true;
  return false;
}

bool
movement_finished ()
{
  if (fabs (distance) < EPSILON_DISTANCE
      && fabs (theta_to_angle) < EPSILON_THETA_SMALL && !robot_moving)
    {
      regulation_rotation_finished ();
      regulation_translation_finished ();
      movement_init = false;
      init_rot_dir = 0;
      final_rot_dir = 0;
      tran_dir = 1;
      return true;
    }
  return false;
}

void
move_full (float x, float y, float theta_degrees, int8_t translation_direction)
{
  tran_dir = translation_direction;
  target_position.x_mm = x;
  target_position.y_mm = y;
  target_position.theta_rad = theta_degrees * M_PI / 180.0;
}

void
move_to_xy (float x, float y, int8_t translation_direction)
{
  move_full (x, y, robot_position.theta_rad / M_PI * 180.0,
	     translation_direction);
}

void
move_to_angle (float theta_degrees)
{
  move_full (robot_position.x_mm, robot_position.y_mm, theta_degrees, 0);
}

void
move_to_angle_2 (float theta_degrees)
{
  move_full (robot_position.x_mm, robot_position.y_mm, theta_degrees, 0);
}

void
turn_to_pos (float x, float y, int8_t translation_direction)
{
  move_full (
      robot_position.x_mm,
      robot_position.y_mm,
      (atan2 (y - robot_position.y_mm, x - robot_position.x_mm)
	  + translation_direction * M_PI) * 180.0 / M_PI,
      0);
}

void
move_to_xy_offset (float x, float y, int8_t translation_direction,
		   float dist_offset)
{
  if (!movement_init)
    {
      float theta_target_rad = atan2 (
	  y - robot_position.y_mm,
	  x - robot_position.x_mm) + translation_direction * M_PI;
      offset.x = dist_offset * cos (theta_target_rad);
      offset.y = dist_offset * sin (theta_target_rad);
      movement_init = true;
    }
  move_full (x - offset.x, y - offset.y,
	     robot_position.theta_rad * 180.0 / M_PI, translation_direction);
}

float
return_x_offset (float x, float y, int8_t translation_direction,
		 float dist_offset)
{
  float theta_target_rad = atan2 (
      y - robot_position.y_mm,
      x - robot_position.x_mm) + translation_direction * M_PI;
  return x - dist_offset * cos (theta_target_rad);
}

float
return_y_offset (float x, float y, int8_t translation_direction,
		 float dist_offset)
{
  float theta_target_rad = atan2 (
      y - robot_position.y_mm,
      x - robot_position.x_mm) + translation_direction * M_PI;
  return y - dist_offset * sin (theta_target_rad);
}

float
return_theta_offset (float x, float y, int8_t translation_direction,
		     float dist_offset)
{
  return atan2 (y - robot_position.y_mm, x - robot_position.x_mm)
      + translation_direction * M_PI;
}

void
move_on_direction (float distance, int8_t direction)
{
  if (!movement_init)
    {
      movement_init = true;
      pos_init.x_mm = robot_position.x_mm
	  + (direction * (-2) + 1) * distance * cos (robot_position.theta_rad);
      pos_init.y_mm = robot_position.y_mm
	  + (direction * (-2) + 1) * distance * sin (robot_position.theta_rad);
    }
  move_full (pos_init.x_mm, pos_init.y_mm,
	     robot_position.theta_rad * 180.0 / M_PI, direction);
}

void
move_on_direction_2 (float distance, int8_t direction)
{
  move_full (
      robot_position.x_mm
	  + (direction * (-2) + 1) * distance * cos (robot_position.theta_rad),
      robot_position.y_mm
	  + (direction * (-2) + 1) * distance * sin (robot_position.theta_rad),
      robot_position.theta_rad * 180.0 / M_PI, direction);
}

void
set_starting_position (float starting_x, float starting_y,
		       float starting_theta_degrees)
{
  robot_position.x_mm = starting_x;
  robot_position.y_mm = starting_y;
  robot_position.theta_rad = starting_theta_degrees * M_PI / 180;
}

//void
//hold_position ()
//{
//  hold_position_state = 1;
//}
//
//void
//continue_movement ()
//{
//  hold_position_state = 0;
//}

void
set_translation_speed_limit (float perc)
{
  V_limit = perc * V_REF_LIMIT_DEFAULT;
}

void
set_rotation_speed_limit (float perc)
{
  w_limit = perc * W_REF_LIMIT_DEFAULT;
}

void
set_transition_factor (float factor)
{
  transition_factor = factor;
}

void
reset_movement ()
{
//  target_position.x_mm = robot_position.x_mm;
//  target_position.y_mm = robot_position.y_mm;
//  target_position.theta_rad = robot_position.theta_rad;
//  regulation_phase = ROT_TO_ANGLE;
//  movement_init = false;
  brake = 1;
}

void
reset_x_coord_close ()
{
  robot_position.x_mm = 85;
  robot_position.theta_rad = M_PI;
}

void
reset_y_coord_close ()
{
  robot_position.y_mm = 85;
  robot_position.theta_rad = -M_PI * 0.5;
}

void
reset_x_coord_far ()
{
  robot_position.x_mm = 3000 - 85;
  robot_position.theta_rad = 0;
}

void
reset_y_coord_far ()
{
  robot_position.y_mm = 2000 - 85;
  robot_position.theta_rad = M_PI * 0.5;
}
