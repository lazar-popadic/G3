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

#define W_LIMIT		0.0001
#define V_LIMIT		0.00001

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

void
calculate_movement ()
{
  error.x_mm = target_position.x_mm - robot_position.x_mm;		// [mm]
  error.y_mm = target_position.y_mm - robot_position.y_mm;		// [mm]
  theta_to_pos_target = atan2 (error.y_mm, error.x_mm);

  // TODO: 	smisli drugacije ovo biranje smerova, bolje mozda u move_full
  //		ali ovako ne mogu samo da prebacim tamo,
  //		bilo bi dobro da tamo mogu da zadam uglove, a ovde samo greske da izracuna
  //		al opet pravi problem init_rot_dir, koji mozda i ne mora da postoji

  switch (tran_dir)
    {
    default:
      break;
    case BACKWARD:
      theta_to_pos_target += M_PI;
      break;
    }

  // ovo sam isto mogao da dobijem i samo normalizacijom greske i robota
//  theta_to_pos_target = float_normalize_angle (theta_to_pos_target,
//					       robot_position.theta_rad);
//  target_position.theta_rad = float_normalize_angle (target_position.theta_rad,
//						     robot_position.theta_rad);

//  switch (init_rot_dir)
//    {
//    default:
//      break;
//    case CCW:	// pozitivan smer
//      if (theta_to_pos_target
//	  < robot_position.theta_rad&& regulation_phase != TRAN_WITH_ROT)
//	{
//	  theta_to_pos_target += 2 * M_PI;
//	}
//      break;
//    case CW:	// negativan smer
//      if (theta_to_pos_target
//	  > robot_position.theta_rad&& regulation_phase != TRAN_WITH_ROT)
//	{
//	  theta_to_pos_target -= 2 * M_PI;
//	}
//      break;
//    }
//
//  switch (final_rot_dir)
//    {
//    default:
//      break;
//    case CCW:	// pozitivan smer
//      if (target_position.theta_rad < robot_position.theta_rad)
//	{
//	  target_position.theta_rad += 2 * M_PI;
//	}
//      break;
//    case CW:	// negativan smer
//      if (target_position.theta_rad > robot_position.theta_rad)
//	{
//	  target_position.theta_rad -= 2 * M_PI;
//	}
//      break;
//    }

  error.theta_rad = target_position.theta_rad - robot_position.theta_rad; // [rad]

  theta_to_pos = float_normalize_angle (
      theta_to_pos_target - robot_position.theta_rad, 0);	// [rad]
  distance = sign (tran_dir)
      * sqrt (error.x_mm * error.x_mm + error.y_mm * error.y_mm);	// [mm]
  theta_to_angle = float_normalize_angle (error.theta_rad, 0);		// [rad]
}

bool
no_movement ()
{
  if (w_rad_s < W_LIMIT || V_m_s < V_LIMIT)
    return true;
  return false;
}

bool
movement_finished ()
{
  if (fabs (distance) < EPSILON_DISTANCE
      && fabs (theta_to_angle) < EPSILON_THETA_SMALL && no_movement ())
    {
      regulation_rotation_finished ();//TODO: vidi da li je ovo sad ispravljeno
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
move_full (float x, float y, float theta, int8_t translation_direction,
	   int8_t initial_rotation_direction, int8_t final_rotation_direction)
{
  tran_dir = translation_direction;
  init_rot_dir = initial_rotation_direction;
  final_rot_dir = final_rotation_direction;
  target_position.x_mm = x;
  target_position.y_mm = y;
  target_position.theta_rad = theta;
}

void
move_to_xy (float x, float y, int8_t translation_direction,
	    int8_t rotation_direction)
{
  move_full (x, y, robot_position.theta_rad, translation_direction,
	     rotation_direction, DEFAULT);
}

void
move_to_angle (float theta_degrees, int8_t rotation_direction)
{
  if (!movement_init)
    {
      movement_init = true;
      pos_init.x_mm = robot_position.x_mm;
      pos_init.y_mm = robot_position.y_mm;
      pos_init.theta_rad = theta_degrees * M_PI / 180;
    }
  move_full (pos_init.x_mm, pos_init.y_mm, pos_init.theta_rad, DEFAULT, DEFAULT,
	     rotation_direction);
}

void
move_on_direction (float distance, int8_t direction)
{
  if (!movement_init)
    {
      movement_init = true;
      pos_init.x_mm = robot_position.x_mm
	  + direction * distance * cos (robot_position.theta_rad);
      pos_init.y_mm = robot_position.y_mm
	  + direction * distance * sin (robot_position.theta_rad);
    }
  move_full (pos_init.x_mm, pos_init.y_mm, robot_position.theta_rad, direction,
  DEFAULT,
	     DEFAULT);
}

void
move_relative_angle (float angle_degrees)
{
  if (!movement_init)
    {
      movement_init = true;
      pos_init.x_mm = robot_position.x_mm;
      pos_init.y_mm = robot_position.y_mm;
      pos_init.theta_rad = robot_position.theta_rad
	  + angle_degrees * M_PI / 180;
    }
  move_full (pos_init.x_mm, pos_init.y_mm, pos_init.theta_rad, DEFAULT, DEFAULT,
  DEFAULT);
}

void
set_starting_position (float starting_x, float starting_y,
		       float starting_theta_degrees)
{
  robot_position.x_mm = starting_x;
  robot_position.y_mm = starting_y;
  robot_position.theta_rad = starting_theta_degrees * M_PI / 180;
}

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
