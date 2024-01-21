/*
 * odometry.c
 *
 *  Created on: Nov 2, 2023
 *      Author: lazar
 */

#include "odometry.h"

#include <stdint.h>
#include "../encoder/encoder.h"
#include <math.h>
#include "../timer/timer.h"

#define d 321.9
#define d_odometrijskog 62

volatile float V = 0;
volatile float w = 0;
static float inc2rad = 0; //broj inkremenata pasivnog tocka za 1 krug robota
static float inc2mm = 0;	//TODO: eksperimentalno koriguj oba ova
int16_t Vd_inc = 0;
int16_t Vl_inc = 0;

volatile position robot_position =
  { 0, 0, 0 };

static float theta_degrees;
extern volatile uint8_t state_angle;

void
odometry_init ()
{
  inc2mm = d_odometrijskog * M_PI / (4.0 * 2048.0);
  inc2rad = inc2mm / d;
}

void
odometry_robot ()
{
  Vd_inc = speed_of_encoder_right_passive ();
  Vl_inc = speed_of_encoder_left_passive ();

  V = (Vd_inc + Vl_inc) * 0.5 * inc2mm;
  w = (Vd_inc - Vl_inc) * inc2rad;

  robot_position.theta_rad += w;
  robot_position.x_mm += V * cos (robot_position.theta_rad);
  robot_position.y_mm += V * sin (robot_position.theta_rad);

  switch (state_angle)
    {
    case PLUS_MINUS_PI:
      normalize_robot_angle_plus_minus_pi ();
      break;
    case PLUS_2PI:
      normalize_robot_angle_plus_2pi ();
      break;
    case MINUS_2PI:
      normalize_robot_angle_minus_2pi ();
      break;
    }

  theta_degrees = robot_position.theta_rad * 180 / M_PI;
}

void
normalize_robot_angle_plus_minus_pi ()
{
  robot_position.theta_rad = float_normalize (robot_position.theta_rad, -M_PI,
					      +M_PI);
}

void
normalize_robot_angle_plus_2pi ()
{
  robot_position.theta_rad = float_normalize (robot_position.theta_rad, 0,
					      +2 * M_PI);
}

void
normalize_robot_angle_minus_2pi ()
{
  robot_position.theta_rad = float_normalize (robot_position.theta_rad,
					      -2 * M_PI, 0);
}

float
limit_angle (float angle)
{
  if (angle > M_PI)
    return angle - 2 * M_PI;
  if (angle < -M_PI)
    return angle + 2 * M_PI;
  return angle;
}

float
float_normalize (float signal, float min, float max)
{
  if (signal > max)
    return signal - (max - min);
  if (signal < min)
    return signal + (max - min);
  return signal;
}
