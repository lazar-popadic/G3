/*
 * regulation.c
 *
 *  Created on: Nov 16, 2023
 *      Author: lazar
 */

#include "regulation.h"
#include <stdbool.h>
#include "../encoder/encoder.h"
#include "../odometrija/odometrija.h"
#include "math.h"
#include "../io/io.h"
#include "../pwm/pwm.h"

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

#define FIRST_ROTATION		0
#define TRANSLATION_WITH_ROTATION		1
#define TRANSLATION_WITHOUT_ROTATION		2
#define FINAL_ROTATION_AND_WAITING				3

static void
regulation_rotation (float theta, float faktor);
static void
regulation_translation (float distance);
static float
saturation (float signal, float MAX, float MIN);
static void
regulation_rotation_finished ();
static void
regulation_translation_finished ();
static void
regulation_phase_calculator ();

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
static float rot_faktor = 1;

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
static uint8_t regulation_phase = 0;

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
  //prva rotacija, da se robot okrene ka tacki ka kojoj treba da ide
  theta_1 = atan2 (p1y, p1x);
  //translacija
  distance = sqrt (p1x * p1x + p1y * p1y);
  //druga rotacija
  theta_2 = theta_0 - get_theta ();

  regulation_phase_calculator ();

  switch (regulation_phase)
    {
    case FIRST_ROTATION:
      theta_error = theta_1;
      rot_faktor = 1;
      distance_error = 0;
      break;

    case TRANSLATION_WITH_ROTATION:
      theta_error = theta_1;
      rot_faktor = 0.5;
      /*
       * TODO: ovde ono racunanje znaka za distance_error u zavisnosti od orijentacije robota ili greske orijentacije, proveri, razmisli
       * TODO: razmisli kako ce robot da reaguje kad prebaci distancu, vidi kako su to u +381
       * TODO: ako menjas nesto i u donjoj funkciji moras da izmenis
       */
      if (fabs (theta_1) > (M_PI / 2))
	distance_error = -distance;
      else
	distance_error = distance;
      break;

    case TRANSLATION_WITHOUT_ROTATION:
      theta_error = 0;
      if (fabs (theta_1) > (M_PI / 2))
	distance_error = -distance;
      else
	distance_error = distance;
      break;

    case FINAL_ROTATION_AND_WAITING:
      theta_error = theta_2;
      rot_faktor = 1;
      distance_error = 0;
      break;
    }

  if (theta_error)
    {
      regulation_rotation (theta_error, rot_faktor);
    }
  else
    u_rot = 0;
  if (distance_error)
    {
      regulation_translation (distance_error);
    }
  else
    u_tran = 0;

  ref_speed_right = u_tran + u_rot;
  ref_speed_left = u_tran - u_rot;
}

/*
 * TODO: razmisli da li pokriva svaki slucaj!
 * TODO: dodaj slucaj kada premasi poziciju, tada je distance pozitivan, on prepravi distance_error da bude negativan,
 * ali ce mozda uci u deo za prvu rotaciju,
 * ako stavis da je distance veci od epsilon_distance_rot onda nece moci da se vrati iz faze 3 u fazu 1 ako ga nesto sjebe
 * mozda u trecoj fazi stavi uslov za vracanje u prvu,
 */
static void
regulation_phase_calculator ()
{
  /*
   * NEMA GRESKE U POZICIJI
   * onda
   * DRZI ZADATI UGAO
   */
  if (fabs(distance) < EPSILON_DISTANCE)
    {
      regulation_translation_finished ();
      regulation_phase = FINAL_ROTATION_AND_WAITING;
      return;
    }
  /*
   * IMA GRESKU U POZICIJI
   * i
   * NIJE ORIJENTISAN KA CILJU
   * onda
   * OKRECE SE KA CILJU
   */
  if (fabs (theta_1) > EPSILON_THETA)
    {
      regulation_translation_finished ();
      regulation_phase = FIRST_ROTATION;
      return;
    }
  /*
   * IMA GRESKU U POZICIJI, koja je VECA od EPSILON_DISTANCE_ROT
   * i
   * JESTE ORIJENTISAN KA CILJU
   * onda
   * IDE KA CILJU SA regulacijom ugla
   */
  if (fabs (distance) > EPSILON_DISTANCE_ROT)
    {
      regulation_rotation_finished ();
      regulation_phase = TRANSLATION_WITH_ROTATION;
      return;
    }
  /*
   * IMA GRESKU U POZICIJI, koja je MANJA od EPSILON_DISTANCE_ROT
   * i
   * JESTE ORIJENTISAN KA CILJU
   * onda
   * IDE KA CILJU BEZ regulacijom ugla
   */
    {
      regulation_rotation_finished ();
      regulation_phase = TRANSLATION_WITHOUT_ROTATION;
      return;
    }
}

static void
regulation_rotation (float theta_er, float faktor)
{
  theta_er_i += theta_er;
  theta_er_i = saturation (theta_er_i, THETA_I_LIMIT, -THETA_I_LIMIT);
  theta_er_d = theta_er - theta_er_previous;

  u_rot = KP_ROT * theta_er + KI_ROT * theta_er_i + KD_ROT * theta_er_d;
  u_rot = saturation (u_rot, U_ROT_MAX, U_ROT_MIN);
  u_rot *= faktor;

  theta_er_previous = theta_er;
}

static void
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
}

static void
regulation_rotation_finished ()
{
  theta_er_i = 0;
  theta_er_d = 0;
  theta_er_previous = 0;
}

static void
regulation_translation_finished ()
{
  distance_er_i = 0;
  distance_er_d = 0;
  distance_er_previous = 0;
}
