/*
 * movement.c
 *
 *  Created on: Jan 2, 2024
 *      Author: lazar
 */

#include "movement.h"
#include "../desired_position/desired_position.h"
#include "../odometry/odometry.h"
#include "stdint.h"
#include "math.h"

// meri
extern volatile float theta;
extern volatile float x;
extern volatile float y;
// zadajem
volatile float desired_x = 0;
volatile float desired_y = 0;
volatile float theta_0 = 0;
//izracuna
volatile float x_error = 0;
volatile float y_error = 0;
volatile float theta_1_float = 0;
volatile float theta_2_float = 0;
extern volatile int32_t theta_1;
extern volatile int32_t distance;
extern volatile int32_t theta_2;

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
  distance = (int32_t) (10 * sqrt (x_error*x_error + y_error * y_error));

  // druga rotacija
  // [0.000 5 rad] = [0.03 degrees]
  theta_2_float = theta_0 - theta;
  //theta_2_float = limit_angle(theta_2_float);
  theta_2 = (int32_t) (2000 * theta_2_float);
}

void
move_full (float desired_x, float desired_y, float desired_theta)
{
  set_desired_position (desired_x, desired_y, desired_theta);
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
