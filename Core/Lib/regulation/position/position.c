/*
 * position.c
 *
 *  Created on: Jan 3, 2024
 *      Author: lazar
 */

#include "position.h"
#include <stdint.h>
#include <math.h>
#include <stdlib.h>
#include "../../movement/movement.h"
#include "../regulation.h"

#define FIRST_ROTATION				0
#define TRANSLATION_WITH_ROTATION		1
#define TRANSLATION_WITHOUT_ROTATION		2
#define FINAL_ROTATION_AND_WAITING		3

#define THETA_I_LIMIT		2500*10
#define DISTANCE_I_LIMIT	2500*10
#define U_ROT_LIMIT		2500*10
#define U_TRAN_LIMIT		2500*10

#define MAXON_LIMIT_R		360	//TODO: izmeri ovo
#define MAXON_LIMIT_L		360

#define EPSILON_THETA		2	*35	// oko 35 inc za 1 stepen
#define EPSILON_DISTANCE	10	*10
#define EPSILON_DISTANCE_ROT	100	*10

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

static const float KP_ROT = 0.1;
static const float KI_ROT = 0;
static const float KD_ROT = 0;
static const float KP_TRAN = 0.1;
static const float KI_TRAN = 0;
static const float KD_TRAN = 0;

volatile int16_t ref_speed_right = 0;
volatile int16_t ref_speed_left = 0;

volatile static int32_t theta_error = 0;
volatile static int32_t distance_error = 0;
volatile static float rot_faktor = 1;

volatile int32_t theta_1 = 0;
volatile int32_t theta_2 = 0;
volatile static int32_t theta_er_previous;
volatile static int32_t theta_er_i;
volatile static int32_t theta_er_d;
volatile static int32_t u_rot;

volatile int32_t distance = 0;
volatile static int32_t distance_er_previous;
volatile static int32_t distance_er_i;
volatile static int32_t distance_er_d;
volatile static int32_t u_tran;

volatile static uint8_t regulation_phase = 0;

extern volatile float x;
extern volatile float y;
extern volatile float theta;

void
regulation_position ()
{
  calculate_movement ();

  regulation_phase_calculator ();

  //TODO: uradi racunanje da li sme da predje u drugu fazu

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
      if (abs (theta_1) > (M_PI * 1000)) // lazar je ovde uradio drugacije, kad je ugao veci od par stepeni
	distance_error = -distance;
      else
	distance_error = distance;
      break;

    case TRANSLATION_WITHOUT_ROTATION:
      theta_error = 0;
      if (abs (theta_1) > (M_PI * 1000))
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

  ref_speed_right = int_ramp_simple (ref_speed_right, u_tran + u_rot, 1);
//  ref_speed_right = u_tran + u_rot;	// bez rampe
  ref_speed_right = int_saturation (ref_speed_right, MAXON_LIMIT_R,
				    -MAXON_LIMIT_R);
  ref_speed_left = int_ramp_simple (ref_speed_left, u_tran - u_rot, 1);
//  ref_speed_left = u_tran - u_rot;	// bez rampe
  ref_speed_left = int_saturation (ref_speed_left, MAXON_LIMIT_L,
				   -MAXON_LIMIT_L);
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
  if (abs (distance) < EPSILON_DISTANCE)
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
  if (abs (theta_1) > EPSILON_THETA)
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
  u_rot = int_saturation (u_rot, U_ROT_LIMIT, -U_ROT_LIMIT);
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
  u_tran = int_saturation (u_tran, U_TRAN_LIMIT, -U_TRAN_LIMIT);

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
