/*
 * regulation.c
 *
 *  Created on: Nov 16, 2023
 *      Author: lazar
 */

#include "regulation.h"
#include <stdbool.h>
#include "../../periphery/encoder/encoder.h"
#include "../pwm/pwm.h"
#include "../odometrija/odometrija.h"
#include "math.h"
#include "../../periphery/io/io.h"

#define EPSILON_THETA		0.1
#define EPSILON_DISTANCE	0.1
#define EPSILON_DISTANCE_ROT	10
// TODO: nemoj ovako vec promenljive pravi zbog tipa
#define KP_SPEED		0
#define KI_SPEED		0
#define KD_SPEED		0
#define KP_ROT			0
#define KI_ROT			0
#define KD_ROT			0
#define KP_TRAN			0
#define KI_TRAN			0
#define KD_TRAN			0

#define EI_MAX 			0 // narednih 10 do 100 iteracija  vrednosti, ei ne sme preko toga ??????????
#define EI_MIN 			0
#define SPEED_LIMIT		0
#define U_ROT_MAX		0
#define U_ROT_MIN		0
#define U_TRAN_MAX		0
#define U_TRAN_MIN		0

static bool
regulation_rotation (float theta, float faktor);
static bool
regulation_translation (float distance, float theta);
static void
regulation_get_u_rotation (float theta, float faktor);
static float
saturation (float signal, float MAX, float MIN);

static float ref_speed_1 = 0;
static float ref_speed_2 = 0;
static float e_1 = 0;
static float e_i_1 = 0;
static float e_d_1 = 0;
static float e_previous_1 = 0;
static float e_2 = 0;
static float e_i_2 = 0;
static float e_d_2 = 0;
static float e_previous_2 = 0;
static float u_1 = 0;			// TODO: sve u inkremente!!!!!!!!!!
static float u_2 = 0;

/*
 * p1 - zeljena pozicija u koordinatno sistemu robota
 * p0 - zeljena pozicija koju smo mi zadali (koordinatni sistem stola)
 * smisli kako da zadajes vektor p0 u main/tactics
 */
static float p1x = 0;
static float p1y = 0;
static float p0x = 0;
static float p0y = 0;
static float distance = 0;
static float theta_0 = 0;
static float theta_1 = 0;
static float theta_2 = 0;

float theta_previous;
float theta_i;
float theta_d;
float u_rot;

float distance_previous;
float distance_i;
float distance_d;
float u_tran;

static uint8_t state_regulation = 0;

void
regulation_init ()
{

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
regulation_speed ()		//TODO: sve radi u inkrementima!!!!!!!!!!!!!!!!!
{
  e_1 = ref_speed_1 - timer_speed_of_encoder3 (); //TODO: OBRATI PAZNJU DA SU OBA U ISTIM JEDINICAMA!!!!!!!!!
  e_i_1 += e_1;
  e_i_1 = saturation (e_i_1, EI_MAX, EI_MIN);
  e_d_1 = e_1 - e_previous_1;

  e_2 = ref_speed_2 - timer_speed_of_encoder3 (); //TODO: OBRATI PAZNJU DA SU OBA U ISTIM JEDINICAMA!!!!!!!!!
  e_i_2 += e_2;
  e_i_2 = saturation (e_i_2, EI_MAX, EI_MIN);
  e_d_2 = e_2 - e_previous_2;

  u_1 = KP_SPEED * e_1 + KI_SPEED * e_i_1 + KD_SPEED * e_d_1;
  u_1 = saturation (u_1, SPEED_LIMIT, -SPEED_LIMIT);
  if (u_1 > 0)
    set_direction_1_wheel_1 ();
  else
    set_direction_2_wheel_1 ();

  u_2 = KP_SPEED * e_2 + KI_SPEED * e_i_2 + KD_SPEED * e_d_2;
  u_2 = saturation (u_2, SPEED_LIMIT, -SPEED_LIMIT);
  if (u_2 > 0)
    set_direction_1_wheel_2 ();
  else
    set_direction_2_wheel_2 ();
  //pwm_duty_cycle((uint16_t)fabs(u_saturated));	//fabs je za float apsolutnu vrednost

  e_previous_1 = e_1;
}

void
regulation_position ()
{

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

  if (state_regulation == 0)
    {
      if (regulation_rotation (theta_1, 1))
	state_regulation++;
    }
  if (state_regulation == 1)
    {
      if (regulation_translation (distance, theta_1))
	state_regulation++;
    }
  //treca rotacija, isto kao prva samo theta_2
  if (state_regulation == 2)
    {
      if (regulation_rotation (theta_2, 1))
	state_regulation++;
    }
}

static bool
regulation_rotation (float theta, float faktor)
{
  theta_i += theta;
  theta_d = theta - theta_previous;
  u_rot = KP_ROT * theta + KI_ROT * theta_i + KD_ROT * theta_d;
  u_rot = saturation (u_rot, U_ROT_MAX, U_ROT_MIN);
  u_rot *= faktor;

  ref_speed_1 = +u_rot;
  ref_speed_2 = -u_rot;

  theta_previous = theta;

  if (fabs (theta) < EPSILON_THETA)
    {
      theta_i = 0;
      theta_d = 0;
      theta_previous = 0;
      return true;
    }
  return false;
}

static bool
regulation_translation (float distance, float theta)
{
  distance_i += distance;
  distance_d = distance - distance_previous;

  u_tran = KP_TRAN * distance + KI_TRAN * distance_i + KD_TRAN * distance_d;
  u_tran = saturation (u_tran, U_TRAN_MAX, U_TRAN_MIN);
  if (fabs (distance) > EPSILON_DISTANCE_ROT)
    regulation_get_u_rotation (theta, 0.5); //radi i reg rotacije dok translira
  else
    u_rot = 0;

  ref_speed_1 = u_tran + u_rot;
  ref_speed_2 = u_tran - u_rot;

  distance_previous = distance;

  if (fabs (distance) < EPSILON_DISTANCE)
    {
      distance_i = 0;
      distance_d = 0;
      distance_previous = 0;
      return true;
    }
  return false;
}

static void
regulation_get_u_rotation (float theta, float faktor)
{
  theta_i += theta;
  theta_d = theta - theta_previous;
  u_rot = KP_ROT * theta + KI_ROT * theta_i + KD_ROT * theta_d;
  u_rot = saturation (u_rot, U_ROT_MAX, U_ROT_MIN);
  u_rot *= faktor;
}
