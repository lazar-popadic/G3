/*
 * regulation.c
 *
 *  Created on: Nov 16, 2023
 *      Author: lazar
 */

#include "regulation.h"
#include <stdbool.h>
#include <stdlib.h>
#include "../encoder/encoder.h"
#include "math.h"
#include "../io/io.h"
#include "../odometry/odometry.h"
#include "../pwm/pwm.h"
#include "../h-bridge/h-bridge.h"

#define EPSILON_THETA		1	*10
#define EPSILON_DISTANCE	10	*10
#define EPSILON_DISTANCE_ROT	100	*10

#define EI_LIMIT 		0 // narednih 10 do 100 iteracija  vrednosti, ei ne sme preko toga ?
#define SPEED_LIMIT		2500 // inkrementi, direktno za pwm duty cycle
#define THETA_I_LIMIT		2500*10
#define DISTANCE_I_LIMIT	2500*10
#define U_ROT_MAX		2500*10
#define U_ROT_MIN		0
#define U_TRAN_MAX		2500*10
#define U_TRAN_MIN		0

#define FIRST_ROTATION		0
#define TRANSLATION_WITH_ROTATION		1
#define TRANSLATION_WITHOUT_ROTATION		2
#define FINAL_ROTATION_AND_WAITING		3

static void
regulation_rotation (int32_t theta_er, float faktor);
static void
regulation_translation (int32_t distance_er);
static void
regulation_rotation_finished ();
static void
regulation_translation_finished ();
static void
regulation_phase_calculator ();

static const float KP_SPEED = 1;
static const float KI_SPEED = 0;
static const float KD_SPEED = 0;
static const float KP_ROT = 1;
static const float KI_ROT = 0;
static const float KD_ROT = 0;
static const float KP_TRAN = 1;
static const float KI_TRAN = 0;
static const float KD_TRAN = 0;

volatile int16_t ref_speed_right = 0;
volatile int16_t ref_speed_left = 0;
volatile static int16_t e_right = 0;
volatile static int16_t e_i_right = 0;
volatile static int16_t e_d_right = 0;
volatile static int16_t e_previous_right = 0;
volatile static int16_t e_left = 0;
volatile static int16_t e_i_left = 0;
volatile static int16_t e_d_left = 0;
volatile static int16_t e_previous_left = 0;
volatile static int16_t u_right = 0;
volatile static int16_t u_left = 0;

// zadajem
volatile float p0x = 0;
volatile float p0y = 0;
volatile float theta_0 = 0;
//izracuna
volatile static float p1x = 0;
volatile static float p1y = 0;

volatile static int32_t theta_error = 0;
volatile static int32_t distance_error = 0;
volatile static float rot_faktor = 1;

volatile static int32_t theta_1 = 0;
volatile static int32_t theta_2 = 0;
volatile static int32_t theta_er_previous;
volatile static int32_t theta_er_i;
volatile static int32_t theta_er_d;
volatile static int32_t u_rot;

volatile static int32_t distance = 0;
volatile static int32_t distance_er_previous;
volatile static int32_t distance_er_i;
volatile static int32_t distance_er_d;
volatile static int32_t u_tran;

//static float inc2rad_deltaT = 0;
volatile static uint8_t regulation_phase = 0;
volatile static uint8_t ramp_counter = 0;

extern volatile float x;
extern volatile float y;
extern volatile float theta;

void
regulation_init ()
{
  //inc2rad_deltaT = M_PI / 4096;	// 2*Pi / 4*2048
}

void
regulation_position ()
{
  /*
   * p1 - zeljena pozicija u koordinatnom sistemu robota
   * p0 - zeljena pozicija koju smo mi zadali (koordinatni sistem stola)
   */
  // float [mm]
  p1x = cos (theta) * (p0x - x) - sin (theta) * (p0y - y);
  p1y = sin (theta) * (p0x - x) + cos (theta) * (p0y - y);

  // uint32_t [1/10 = 0.1]
  //prva rotacija, da se robot okrene ka tacki ka kojoj treba da ide
  theta_1 = (uint32_t)(10 * atan2 (p1y, p1x));
  //translacija
  distance = (uint32_t)(10 * sqrt (p1x * p1x + p1y * p1y));
  //druga rotacija
  theta_2 = theta_0 - (uint32_t)(10 * theta);

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
      if (fabs (theta_1) > (M_PI / 2)) // lazar je ovde uradio drugacije, kad je ugao veci od par stepeni
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

  ref_speed_right = int_ramp_simple(ref_speed_right, u_tran + u_rot, 10);
  ref_speed_left = int_ramp_simple(ref_speed_left, u_tran - u_rot, 10);
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
  if (fabs (distance) < EPSILON_DISTANCE)
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
regulation_rotation (int32_t theta_er, float faktor)
{
  theta_er_i += theta_er;
  theta_er_i = int_saturation (theta_er_i, THETA_I_LIMIT, -THETA_I_LIMIT);
  theta_er_d = theta_er - theta_er_previous;

  u_rot = KP_ROT * theta_er + KI_ROT * theta_er_i + KD_ROT * theta_er_d;
  u_rot = int_saturation (u_rot, U_ROT_MAX, U_ROT_MIN);
  u_rot *= faktor;

  theta_er_previous = theta_er;
}

static void
regulation_translation (int32_t distance_er)
{
  distance_er_i += distance_er;
  distance_er_i = int_saturation (distance_er_i, DISTANCE_I_LIMIT,
				  -DISTANCE_I_LIMIT);
  distance_er_d = distance_er - distance_er_previous;

  u_tran = KP_TRAN * distance_er + KI_TRAN * distance_er_i
      + KD_TRAN * distance_er_d;
  u_tran = int_saturation (u_tran, U_TRAN_MAX, U_TRAN_MIN);

  distance_er_previous = distance_er;
}

void
regulation_speed (int16_t speed_right, int16_t speed_left)
{
  e_right = ref_speed_right - speed_right;
  e_i_right += e_right;
  e_i_right = int_saturation (e_i_right, EI_LIMIT, - EI_LIMIT);
  e_d_right = e_right - e_previous_right;

  e_left = ref_speed_left - speed_left;
  e_i_left += e_left;
  e_i_left = int_saturation (e_i_left, EI_LIMIT, - EI_LIMIT);
  e_d_left = e_left - e_previous_left;

  u_right = KP_SPEED * e_right + KI_SPEED * e_i_right + KD_SPEED * e_d_right;
  u_right = int_saturation (u_right, SPEED_LIMIT, -SPEED_LIMIT);
  u_left = KP_SPEED * e_left + KI_SPEED * e_i_left + KD_SPEED * e_d_left;
  u_left = int_saturation (u_left, SPEED_LIMIT, -SPEED_LIMIT);

  if (u_right > 0)
    right_wheel_forwards ();
  else
    right_wheel_backwards ();

  if (u_left > 0)
    left_wheel_forwards ();
  else
    left_wheel_backwards ();

  // Tj. ovde postavlja referencu za struju
  pwm_duty_cycle_right (abs (u_right));
  pwm_duty_cycle_left (abs (u_left));

  e_previous_left = e_left;
  e_previous_right = e_right;
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

float
float_saturation (float signal, float MAX, float MIN)
{
  if (signal > MAX)
    return MAX;
  if (signal < MIN)
    return MIN;
  return signal;
}

int32_t
int_saturation (int32_t signal, int32_t MAX, int32_t MIN)
{
  if (signal > MAX)
    return MAX;
  if (signal < MIN)
    return MIN;
  return signal;
}

int32_t
int_ramp_simple (int32_t signal, int32_t desired_value, int8_t slope)
{
  if (desired_value - signal > slope)
    {
      return signal + slope;
    }
  return desired_value;
}

int32_t
int_ramp_advanced (int32_t signal, int32_t desired_value, int8_t slope,
		   uint8_t prescaler)
{
  if (ramp_counter < prescaler)
    {
      ramp_counter++;
      return signal;
    }
  ramp_counter = 0;
  if (desired_value - signal > slope)
    {
      return signal + slope;
    }
  return desired_value;
}
