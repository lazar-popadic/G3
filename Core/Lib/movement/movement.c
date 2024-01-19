/*
 * movement.c
 *
 *  Created on: Jan 2, 2024
 *      Author: lazar
 */

#include "movement.h"
#include "../regulation/position/position.h"
#include "../odometry/odometry.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#define W_LIMIT		10
#define V_LIMIT		10

// meri
extern volatile position robot_position;
extern volatile float V;
extern volatile float w;
// zadajem
volatile position target_position =
  { 0, 0, 0 };
//izracuna
volatile position error =
  { 0, 0, 0 };
volatile float theta_to_pos = 0;
volatile float distance;
volatile float theta_to_angle = 0;

volatile bool movement_init = false;
volatile static position pos_init =
  { 0, 0, 0 };

void
calculate_movement ()
{
  error.x_mm = target_position.x_mm - robot_position.x_mm;		// [mm]
  error.y_mm = target_position.y_mm - robot_position.y_mm;		// [mm]
  error.theta_rad = target_position.theta_rad - robot_position.theta_rad;// [rad]

  theta_to_pos = atan2 (error.y_mm, error.x_mm) - robot_position.theta_rad;// [rad]
  distance = sqrt (error.x_mm * error.x_mm + error.y_mm * error.y_mm);	// [mm]
  theta_to_angle = error.theta_rad;					// [rad]
}

void
set_starting_position (float starting_x, float starting_y,
		       float starting_theta_degrees)
{
  robot_position.x_mm = starting_x;
  robot_position.y_mm = starting_y;
  robot_position.theta_rad = starting_theta_degrees * M_PI / 180;
}

bool
no_movement ()
{
  if (w < W_LIMIT || V < V_LIMIT)
    return true;
  return false;
}

bool
movement_finished ()
{
  if (fabs (distance) < EPSILON_DISTANCE
      && fabs (theta_to_angle) < EPSILON_THETA_SMALL && no_movement ())
    {
      movement_init = false;
      return true;
    }
  return false;
}

void
move_full (float x, float y, float theta)
{
  target_position.x_mm = x;			// od	0 	do	3000
  target_position.y_mm = y;			// od	0 	do	2000
  target_position.theta_rad = float_normalize (theta, -M_PI, M_PI);// od	-3.14	do	3.14
}

void
move_to_xy (float x, float y)
{
  move_full (robot_position.x_mm, robot_position.y_mm,
	     robot_position.theta_rad);	// TODO: ne moze ovako, ovo bi vazilo kada bi stalno prolazio kroz ovo
}

void
move_to_angle (float theta_degrees)
{
  if (!movement_init)
    {
      movement_init = true;
      pos_init.x_mm = robot_position.x_mm;
      pos_init.y_mm = robot_position.y_mm;
      pos_init.theta_rad = theta_degrees * M_PI / 180;
    }
  move_full (pos_init.x_mm, pos_init.y_mm, pos_init.theta_rad);
}

void
move_on_direction (float distance)
{
  if (!movement_init)
    {
      movement_init = true;
      pos_init.x_mm = robot_position.x_mm
	  + distance * cos (robot_position.theta_rad);
      pos_init.y_mm = robot_position.y_mm
	  + distance * sin (robot_position.theta_rad);
    }
  move_full (pos_init.x_mm, pos_init.y_mm, robot_position.theta_rad);
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
  move_full (pos_init.x_mm, pos_init.y_mm, pos_init.theta_rad);
}
