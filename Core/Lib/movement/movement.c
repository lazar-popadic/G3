/*
 * movement.c
 *
 *  Created on: Jan 2, 2024
 *      Author: lazar
 */

#include "movement.h"
#include "../odometry/odometry.h"
#include "stdint.h"
#include "math.h"

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
volatile float x_error = 0;
volatile float y_error = 0;
volatile float theta_1_float = 0;
volatile float theta_2_float = 0;
extern volatile int32_t theta_1;
extern volatile int32_t distance;
extern volatile int32_t theta_2;

extern volatile float V;
extern volatile float w;

void
calculate_movement ()
{
  // float [mm]
  x_error = desired_x - x;
  y_error = desired_y - y;

  // int32_t
  // prva rotacija, da se robot okrene ka tacki ka kojoj treba da ide
  // [0.000 5 rad] = [0.03 degrees]
  theta_1_float = atan2 (y_error, x_error) - theta;
  //theta_1_float = limit_angle(theta_1_float);
  theta_1 = (int32_t) (2000 * theta_1_float);

  // translacija
  // [0.1mm]
  distance = (int32_t) (10 * sqrt (x_error * x_error + y_error * y_error));

  // druga rotacija
  // [0.000 5 rad] = [0.03 degrees]
  theta_2_float = desired_theta - theta;
  //theta_2_float = limit_angle(theta_2_float);
  theta_2 = (int32_t) (2000 * theta_2_float);
}

void
set_starting_position (float starting_x, float starting_y, float starting_theta)
{
  x = starting_x;
  y = starting_y;
  theta = starting_theta;
}

bool
no_movement ()
{
  if (w < W_LIMIT || V < V_LIMIT)
    return true;
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
move_xy (float desired_x, float desired_y)
{

}

void
move_theta (float desired_theta)
{

}

void
move_relative_rotate (float angle)
{

}

void
move_translate (float distance)
{

}
