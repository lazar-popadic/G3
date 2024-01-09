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

#define d 321.8
#define d_odometrijskog 64

volatile float V = 0;
volatile float w = 0;
static float inc2rad = 0;//broj inkremenata pasivnog tocka za 1 krug robota
static float inc2mm = 0;	//TODO: eksperimentalno koriguj oba ova

volatile position robot_position =
  { 0, 0, 0 };

static float theta_degrees;

void
odometry_init ()
{
  inc2mm = d_odometrijskog * M_PI / (4.0 * 2048.0);
  inc2rad = inc2mm / d;
}

void
odometry_robot ()
{
  int16_t Vd_inc = speed_of_encoder_right_passive ();
  int16_t Vl_inc = speed_of_encoder_left_passive ();

  V = (Vd_inc + Vl_inc) * 0.5 * inc2mm;
  w = (Vd_inc - Vl_inc) * inc2rad;

  robot_position.theta_rad += w;
  robot_position.theta_rad = limit_angle (robot_position.theta_rad);
  robot_position.x_mm += V * cos (robot_position.theta_rad);
  robot_position.y_mm += V * sin (robot_position.theta_rad);

  theta_degrees = robot_position.theta_rad * 180 / M_PI;
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
