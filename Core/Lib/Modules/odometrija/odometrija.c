/*
 * odometrija.c
 *
 *  Created on: Nov 2, 2023
 *      Author: lazar
 */

#include "odometrija.h"

#include <stdint.h>
#include "../../periphery/timer/timer.h"
#include <math.h>

static float V = 0;
static float w = 0;
static float d = 0;		//rastojanje izmedju 2 pasivna tocka
static float d_odometrijskog = 0;		// precnik odometrijskog
static float inc2rad = 0;	//TODO: eksperimentalno koriguj
static float inc2mm = 0;
static float theta;
static float x;			// inicijalizuj na x_start i y_start u strategiji
static float y;

void
odometrija_init ()
{
  d_odometrijskog = 60;		//mm
  d = 320;			//razmak izmedju odometrijskih tockova [mm]
  //inc2rad = d * 2048 * 4 / d_odometrijskog ;	//(d * M_PI) / (d_odometrijskog * M_PI) * 2048 * 4;
  //inc2rad = (2 * M_PI) / inc2rad;		//ovo je bilo na vezbama, al msm da je cilag sjebao
  inc2mm = d_odometrijskog * M_PI / (4 * 2048);
  inc2rad = 2.0 * inc2mm / d;
}

void
odometrija_robot ()		//racun pozicije i orijentacije
{
  int16_t Vd_inc = timer_speed_of_encoder1 ();	//inc = inkrementi
  int16_t Vl_inc = timer_speed_of_encoder2 ();

  // translacija
  // N = 1000mm / ObimOdometrijskogTocka		//koliko odometrijski predje za 1 metar
  // n = N * 2048 * 4					//n = broj impulsa enkodera za 1 metar, 2048 = rezolucija enkodera, 4 = QEP
  // 1000mm : n = x : 1 => x = 1000mm / n		//iz impulsa u milimetre

  // rotacija
  // ObimOdometrijskogTocka = 2rPi
  // ObimRobotaOdometrijski = dPi
  // N = ObimRobotaOdometrijski / ObimOdometrijskogTocka
  // x = 2Pi / n					//iz impulsa u radijane

  // nemoj odmah da konvertujes brzine tockova u float, nego koristi inkremente, pa samo na kraju konvertuj
//  Vd = Vd_inc * inc2rad;
 // Vl = Vl_inc * inc2rad;

  // desni koordinatni sistem
  //w = (Vd_inc - Vl_inc) * inc2rad / d;
  //V = (Vd_inc + Vl_inc) * inc2rad * 0.5;
  V = (Vd_inc + Vl_inc) * inc2mm;
  w = (Vd_inc - Vl_inc) * inc2rad;

  // TESTIRAJ
  theta += w;
  x += V*cos(theta);
  y += V*sin(theta);
}

float
get_theta ()
{
  return theta;
}

float
get_x ()
{
  return x;
}

float
get_y ()
{
  return y;
}
