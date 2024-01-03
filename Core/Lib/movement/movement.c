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
volatile float theta_to_pos_float = 0;
volatile float theta_to_angle_float = 0;
volatile int32_t theta_to_pos;
volatile int32_t distance;
volatile int32_t theta_to_angle;

volatile static int32_t theta_error = 0;
volatile static int32_t distance_error = 0;

extern volatile int32_t u_rot;
extern volatile int32_t u_tran;

extern volatile float V;
extern volatile float w;

volatile bool phase_init = false;
volatile bool phase_finished = false;
volatile uint8_t movement_phase = 0;

void
calculate_movement ()
{
  x_e = desired_x - x;						// [mm]
  y_e = desired_y - y;						// [mm]
  theta_e = desired_theta - theta;				// [rad]

  theta_to_pos_float = atan2 (y_e, x_e) - theta;		// [rad]
  theta_to_pos = (int32_t) (2000 * theta_to_pos_float);		// [0.000 5 rad] = [0.03 degrees]

  distance = (int32_t) (10 * sqrt (x_e * x_e + y_e * y_e));	// [0.1mm]

  theta_to_angle_float = theta_e;				// [rad]
  theta_to_angle = (int32_t) (2000 * theta_to_angle_float);	// [0.000 5 rad] = [0.03 degrees]
}

void
movement_fsm ()
{
  switch (movement_phase)
    {
    case 0:
      if (!phase_init)
	{
	  phase_init = true;

	}

      if (phase_finished)
	{
	  phase_init = false;

	}
      break;

    }
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
