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

#define d 334.2
#define d_odometrijskog 65.94		//65.42
#define d_odometrijskog_levi 65.68	//65.11

volatile double V_deltaT = 0;
volatile double w_deltaT = 0;
volatile double V_m_s = 0;
volatile double w_rad_s = 0;
static double inc2rad = 0; //broj inkremenata pasivnog tocka za 1 krug robota
static double inc2mm = 0;
static double inc2rad_s = 0;
static double diff_factor = 0;
double f_Vl_inc = 0;
int16_t Vd_inc = 0;
int16_t Vl_inc = 0;
int16_t Vd_sum = 0;
int16_t Vl_sum = 0;

volatile position robot_position =
  { 0, 0, 0 };

volatile double theta_robot_normalized = 0;
static float theta_degrees;

void
odometry_init ()
{
  inc2mm = d_odometrijskog * M_PI / (4.0 * 2048.0);
  inc2rad = inc2mm / d;
  inc2rad_s = inc2rad * 500.0;// TODO: ovde ubaci neki faktor da bude preciznije npr. 1,003 (360/359)
  diff_factor = d_odometrijskog_levi / d_odometrijskog;
}

void
odometry_robot ()
{
  Vl_inc = speed_of_encoder_left_passive ();
  f_Vl_inc = (double) (Vl_inc) * diff_factor;
  Vd_inc = speed_of_encoder_right_passive ();
  Vd_sum += Vd_inc;
  Vl_sum += f_Vl_inc;

  V_deltaT = ((double) Vd_inc + f_Vl_inc) * 0.5 * inc2mm;// [mm / 0.5ms] = [m / 2s]
  V_m_s = V_deltaT * 0.25;	// ipak ovde treba 0.5, al ovako bolje radi
  w_rad_s = ((double) Vd_inc - f_Vl_inc) * inc2rad_s;		// [rad / s]
  w_deltaT = w_rad_s * 0.002;					// [rad / 2ms]

  robot_position.x_mm += V_deltaT
      * cos (robot_position.theta_rad + w_deltaT / 2.0);
  robot_position.y_mm += V_deltaT
      * sin (robot_position.theta_rad + w_deltaT / 2.0);
  robot_position.theta_rad += w_deltaT;
  normalize_robot_angle ();

//  theta_robot_normalized = float_normalize_angle (robot_position.theta_rad, 0);
  theta_degrees = robot_position.theta_rad * 180 / M_PI;
//  theta_degrees = theta_robot_normalized * 180 / M_PI;
}

void
normalize_robot_angle ()
{
  robot_position.theta_rad = simple_normalize (robot_position.theta_rad);
}

float
float_normalize_angle (float signal, float middle)
{
  float max = middle + M_PI;
  float min = middle - M_PI;
  if (signal > max)
    return signal - (int8_t) ((signal - min) / (2 * M_PI)) * 2 * M_PI;
  if (signal < min)
    return signal + (int8_t) ((max - signal) / (2 * M_PI)) * 2 * M_PI;
  return signal;
}

float
simple_normalize (float signal)
{
  if (signal > M_PI)
    return signal - 2 * M_PI;
  if (signal < -M_PI)
    return signal + 2 * M_PI;
  return signal;
}
