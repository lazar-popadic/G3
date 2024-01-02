/*
 * desired_position.c
 *
 *  Created on: Jan 2, 2024
 *      Author: lazar
 */

#include "desired_position.h"
#include "../odometry/odometry.h"

extern volatile float desired_x;
extern volatile float desired_y;
extern volatile float theta_0;

extern volatile float theta;
extern volatile float x;
extern volatile float y;

void
set_desired_position (float x, float y, float theta)
{
  desired_x = x;			// od	0 	do	3000
  desired_y = y;			// od	0 	do	2000
  theta_0 = limit_angle(theta);		// od	-1.57	do	1.57
}

void
set_starting_position (float starting_x, float starting_y, float starting_theta)
{
  x = starting_x;
  y = starting_y;
  theta = starting_theta;
}
