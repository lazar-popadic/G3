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

#define d 340.0
#define d_odometrijskog 64.2

volatile double V_deltaT = 0;
volatile double w_deltaT = 0;
volatile double V_m_s = 0;
volatile double w_rad_s = 0;
static double inc2rad = 0; //broj inkremenata pasivnog tocka za 1 krug robota
static double inc2mm = 0;	//TODO: eksperimentalno koriguj oba ova
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

  V_deltaT = (Vd_inc + Vl_inc) * 0.5 * inc2mm;	// [mm / 0.5ms] = [m / 0.5ms]
  V_m_s = V_deltaT * 2.0;					// [m / s]
  w_rad_s = (Vd_inc - Vl_inc) * inc2rad * 2000.0;	// [rad / s]
  w_deltaT = w_rad_s * 0.0005;					// [rad / 0.5ms]

  robot_position.theta_rad += w_deltaT;
  robot_position.x_mm += V_deltaT * cos (robot_position.theta_rad);
  robot_position.y_mm += V_deltaT * sin (robot_position.theta_rad);

  switch (state_angle)
    {
    case PLUS_MINUS_PI:
      normalize_robot_angle ();
      break;
    case MAX_PLUS_2PI:
      normalize_robot_angle_max ();
      break;
    case MIN_MINUS_2PI:
      normalize_robot_angle_min ();
      break;
    }

  theta_degrees = robot_position.theta_rad * 180 / M_PI;
}

void
normalize_robot_angle ()
{
  robot_position.theta_rad = float_normalize (robot_position.theta_rad, -M_PI,
					      +M_PI);
}

void
normalize_robot_angle_max ()
{
  if (robot_position.theta_rad > 2 * M_PI)
    robot_position.theta_rad -= 2 * M_PI;
}

void
normalize_robot_angle_min ()
{
  if (robot_position.theta_rad < 2 * M_PI)
    robot_position.theta_rad += 2 * M_PI;
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
