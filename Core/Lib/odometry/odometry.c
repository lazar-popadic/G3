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
#define d_odometrijskog 66

static float V = 0;
static float w = 0;
//static float d = 320;				//razmak izmedju odometrijskih tockova [mm]
//static float d_odometrijskog = 60;		//precnik odometrijskog tocka [mm]
static float inc2rad = 0; //broj inkremenata na pasivnom tocku za 1 krug robota
static float inc2mm = 0;	//TODO: eksperimentalno koriguj oba ova

volatile float theta;
volatile float x;
volatile float y;

static float theta_degrees;

void
odometry_init ()
{
  //inc2rad = d * 2048 * 4 / d_odometrijskog ;	//(d * M_PI) / (d_odometrijskog * M_PI) * 2048 * 4;
  //inc2rad = (2 * M_PI) / inc2rad;		//ovo je bilo na vezbama, al msm da je cilag sjebao
  inc2mm = d_odometrijskog * M_PI / (4.0 * 2048.0);
  inc2rad = inc2mm / d;
}

void
odometry_robot ()		//racun pozicije i orijentacije
{
  int16_t Vd_inc = speed_of_encoder_right_passive ();	//inc = inkrementi
  int16_t Vl_inc = speed_of_encoder_left_passive ();

  // translacija
  // N = 1000mm / ObimOdometrijskogTocka	//koliko odometrijski predje za 1 metar
  // n = N * 2048 * 4				//n = broj impulsa enkodera za 1 metar, 2048 = rezolucija enkodera, 4 = QEP
  // 1000mm : n = x : 1 => x = 1000mm / n	//iz impulsa u milimetre

  // rotacija
  // ObimOdometrijskogTocka = 2rPi
  // ObimRobotaOdometrijski = dPi
  // N = ObimRobotaOdometrijski / ObimOdometrijskogTocka
  // x = 2Pi / n				//iz impulsa u radijane

  // desni koordinatni sistem
  //w = (Vd_inc - Vl_inc) * inc2rad / d;
  //V = (Vd_inc + Vl_inc) * inc2rad * 0.5;
  V = (Vd_inc + Vl_inc) * 0.5 * inc2mm;
  w = (Vd_inc - Vl_inc) * inc2rad;

  // TESTIRAJ
  theta += w;
  theta = limit_angle(theta);
  x += V * cos (theta);
  y += V * sin (theta);

  theta_degrees = theta * 180 / M_PI;
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
