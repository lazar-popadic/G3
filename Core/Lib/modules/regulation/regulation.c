/*
 * regulation.c
 *
 *  Created on: Nov 16, 2023
 *      Author: lazar
 */

#include "regulation.h"
#include <stdbool.h>
#include "../../periphery/encoder/encoder.h"
#include "../odometrija/odometrija.h"
#include "math.h"
#include "../../periphery/io/io.h"
#include "../../periphery/pwm/pwm.h"

#define EPSILON_THETA		0.1
#define EPSILON_DISTANCE	0.1
#define EPSILON_DISTANCE_ROT	10

#define EI_LIMIT 		0 // narednih 10 do 100 iteracija  vrednosti, ei ne sme preko toga ?
#define SPEED_LIMIT		0
#define THETA_I_LIMIT		0
#define DISTANCE_I_LIMIT	0
#define U_ROT_MAX		0
#define U_ROT_MIN		0
#define U_TRAN_MAX		0
#define U_TRAN_MIN		0

static bool
regulation_rotation (float theta);
static bool
regulation_translation (float distance);
static float
saturation (float signal, float MAX, float MIN);

static const float KP_SPEED = 0;
static const float KI_SPEED = 0;
static const float KD_SPEED = 0;
static const float KP_ROT = 0;
static const float KI_ROT = 0;
static const float KD_ROT = 0;
static const float KP_TRAN = 0;
static const float KI_TRAN = 0;
static const float KD_TRAN = 0;

static float ref_speed_right = 0;
static float ref_speed_left = 0;
static float e_right = 0;
static float e_i_right = 0;
static float e_d_right = 0;
static float e_previous_right = 0;
static float e_left = 0;
static float e_i_left = 0;
static float e_d_left = 0;
static float e_previous_left = 0;
static float u_right = 0;		// TODO: jedinice, razmisli
static float u_left = 0;

static float p1x = 0;
static float p1y = 0;
static float p0x = 0;
static float p0y = 0;
static float distance = 0;
static float theta_0 = 0;

static float theta_error = 0;
static float distance_error = 0;

static float theta_1 = 0;
static float theta_2 = 0;
static float theta_er_previous;
static float theta_er_i;
static float theta_er_d;
static float u_rot;

static float distance_er_previous;
static float distance_er_i;
static float distance_er_d;
static float u_tran;

static float inc2rad_deltaT = 0;

void
regulation_init ()
{
  inc2rad_deltaT = M_PI / 4096;	//2*Pi / 4*2048
}

static float
saturation (float signal, float MAX, float MIN)
{
  if (signal > MAX)
    return MAX;
  if (signal < MIN)
    return MIN;
  return signal;
}

void
regulation_speed ()
{
  e_right = ref_speed_right
      - inc2rad_deltaT * timer_speed_of_encoder_right_maxon (); //[rad/deltaT]
  e_i_right += e_right;
  e_i_right = saturation (e_i_right, EI_LIMIT, - EI_LIMIT);
  e_d_right = e_right - e_previous_right;

  e_left = ref_speed_left
      - inc2rad_deltaT * timer_speed_of_encoder_left_maxon ();
  e_i_left += e_left;
  e_i_left = saturation (e_i_left, EI_LIMIT, - EI_LIMIT);
  e_d_left = e_left - e_previous_left;

  u_right = KP_SPEED * e_right + KI_SPEED * e_i_right + KD_SPEED * e_d_right;
  u_right = saturation (u_right, SPEED_LIMIT, -SPEED_LIMIT);
  u_left = KP_SPEED * e_left + KI_SPEED * e_i_left + KD_SPEED * e_d_left;
  u_left = saturation (u_left, SPEED_LIMIT, -SPEED_LIMIT);

  if (u_right > 0)
    set_direction_1_wheel_1 ();
  else
    set_direction_2_wheel_1 ();

  if (u_left > 0)
    set_direction_1_wheel_2 ();
  else
    set_direction_2_wheel_2 ();
  //pwm_duty_cycle((uint16_t)fabs(u_saturated));	//fabs je za float apsolutnu vrednost
  //u pwm_dc ide procenat od max brzine, a ovde cu da racunam u rad/deltaT
  // Tj. ovde postavlja referencu za struju

  e_previous_left = e_left;
  e_previous_right = e_right;
}

void
regulation_position ()
{
  /*
   * p1 - zeljena pozicija u koordinatno sistemu robota
   * p0 - zeljena pozicija koju smo mi zadali (koordinatni sistem stola)
   * TODO: smisli kako da zadajes p0x, p0y i theta_0 u main/tactics
   */

  p1x = cos (get_theta ()) * (p0x - get_x ())
      - sin (get_theta ()) * (p0y - get_y ());
  p1y = sin (get_theta ()) * (p0x - get_x ())
      + cos (get_theta ()) * (p0y - get_y ());
  //translacija
  distance = sqrt (p1x * p1x + p1y * p1y);
  //prva rotacija
  theta_1 = atan2 (p1y, p1x);
  //druga rotacija
  theta_2 = theta_0 - get_theta ();

  //TODO: proveri da li svuda ovde ide theta_1

  if (fabs(theta_1) > EPSILON_THETA && fabs(distance) > EPSILON_DISTANCE_ROT)
    {
      theta_error = theta_1;
      distance_error = 0;
    }
  if (fabs(theta_1) < EPSILON_THETA && fabs(distance) > EPSILON_DISTANCE)
    {
      theta_error = 0;
      // TODO: ovde ono racunanje znaka za distance_error u zavisnosti od orijentacije robota ili greske orijentacije, proveri, razmisli
      if ( fabs(theta_1) > (M_PI / 2) )
	distance_error = -distance;
      else
	distance_error = distance;
    }
  if (fabs(distance) < EPSILON_DISTANCE)
    {
      theta_error = theta_2;
      distance_error = 0;
    }

  if(theta_error)
    regulation_rotation (theta_error);
  if(distance_error)
    regulation_translation (distance_error);

  ref_speed_right = u_tran + u_rot;
  ref_speed_left = u_tran - u_rot;
}

static bool
regulation_rotation (float theta_er)
{
  theta_er_i += theta_er;
  theta_er_i = saturation (theta_er_i, THETA_I_LIMIT, -THETA_I_LIMIT);
  theta_er_d = theta_er - theta_er_previous;
  u_rot = KP_ROT * theta_er + KI_ROT * theta_er_i + KD_ROT * theta_er_d;
  u_rot = saturation (u_rot, U_ROT_MAX, U_ROT_MIN);

  theta_er_previous = theta_er;

  if (fabs (theta_er) < EPSILON_THETA)
    {
      theta_er_i = 0;
      theta_er_d = 0;
      theta_er_previous = 0;
      return true;
    }
  return false;
}

static bool
regulation_translation (float distance_er)
{
  distance_er_i += distance_er;
  distance_er_i = saturation (distance_er_i, DISTANCE_I_LIMIT,
			      -DISTANCE_I_LIMIT);
  distance_er_d = distance_er - distance_er_previous;

  u_tran = KP_TRAN * distance_er + KI_TRAN * distance_er_i
      + KD_TRAN * distance_er_d;
  u_tran = saturation (u_tran, U_TRAN_MAX, U_TRAN_MIN);

  distance_er_previous = distance_er;

  if (fabs (distance_er) < EPSILON_DISTANCE)
    {
      distance_er_i = 0;
      distance_er_d = 0;
      distance_er_previous = 0;
      return true;
    }
  return false;
}
