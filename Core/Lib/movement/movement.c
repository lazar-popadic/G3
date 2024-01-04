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
extern volatile float theta;
extern volatile float x;
extern volatile float y;
// zadajem
volatile float desired_x = 0;
volatile float desired_y = 0;
volatile float desired_theta = 0;
//izracuna
volatile float x_e = 0;
volatile float y_e = 0;
volatile float theta_e = 0;
volatile float theta_to_pos = 0;
volatile float distance;
volatile float theta_to_angle = 0;

volatile static int32_t theta_error = 0;
volatile static int32_t distance_error = 0;

extern volatile int32_t u_rot;
extern volatile int32_t u_tran;

extern volatile float V;
extern volatile float w;

volatile bool movement_init = false;
volatile static float x_init = 0;
volatile static float y_init = 0;
volatile static float theta_init = 0;

void
calculate_movement ()
{
  x_e = desired_x - x;				// [mm]
  y_e = desired_y - y;				// [mm]
  theta_e = desired_theta - theta;		// [rad]

  theta_to_pos = atan2 (y_e, x_e) - theta;	// [rad]
  distance = sqrt (x_e * x_e + y_e * y_e);	// [mm]
  theta_to_angle = theta_e;			// [rad]
}

void
set_starting_position (float starting_x, float starting_y,
		       float starting_theta_degrees)
{
  x = starting_x;
  y = starting_y;
  theta = starting_theta_degrees * M_PI / 180;
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
  desired_x = x;			// od	0 	do	3000
  desired_y = y;			// od	0 	do	2000
  desired_theta = limit_angle (theta);	// od	-3.14	do	3.14
}

void
move_to_xy (float x, float y)
{
  move_full (x, y, theta);// TODO: ne moze ovako, ovo bi vazilo kada bi stalno prolazio kroz ovo
}

void
move_to_angle (float theta_degrees)
{
  if (!movement_init)
    {
      movement_init = true;
      x_init = x;
      y_init = y;
      theta_init = theta_degrees * M_PI / 180;
    }
  move_full (x_init, y_init, theta_init);
}

void
move_on_direction (float distance)
{
  if (!movement_init)
    {
      movement_init = true;
      x_init = x + distance * cos (theta);
      y_init = y + distance * sin (theta);
    }
  move_full (x_init, y_init, theta);
}

void
move_relative_angle (float angle_degrees)
{
  if (!movement_init)
    {
      movement_init = true;
      x_init = x;
      y_init = y;
      theta_init = theta + angle_degrees * M_PI / 180;
    }
  move_full (x_init, y_init, theta_init);
}
