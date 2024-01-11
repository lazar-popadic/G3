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
#include <stdbool.h>
#include "../../movement/movement.h"
#include "../regulation.h"
#include "../../h-bridge/h-bridge.h"
#include "../../pwm/pwm.h"

#define ROT_TO_ANGLE		0
#define ROT_TO_POS		1
#define TRAN_WITH_ROT		2
#define TRAN_WITHOUT_ROT	3

#define THETA_I_LIMIT		2500
#define DISTANCE_I_LIMIT	2500
#define U_ROT_LIMIT		2500
#define U_TRAN_LIMIT		2500

#define MAXON_LIMIT_R		360	//TODO: izmeri ovo
#define MAXON_LIMIT_L		360

#define LEFT_MAXON_FORW_OFFSET	490
#define LEFT_MAXON_BACK_OFFSET	-540

#define SPEED_LIMIT	1500 // inkrementi, direktno za pwm duty cycle

static const float KP_ROT = 200;
static const float KI_ROT = 0;
static const float KD_ROT = 0;
static const float KP_TRAN = 1;
static const float KI_TRAN = 0;
static const float KD_TRAN = 0;

volatile int16_t ref_speed_right = 0;
volatile int16_t ref_speed_left = 0;

volatile static int32_t theta_error = 0;
volatile static int32_t distance_error = 0;
volatile static float rot_faktor = 1;

extern volatile float theta_to_pos;
extern volatile float theta_to_angle;
volatile static float theta_er_previous;
volatile static float theta_er_i;
volatile static float theta_er_d;
volatile int32_t u_rot;

extern volatile float distance;
volatile static float distance_er_previous;
volatile static float distance_er_i;
volatile static float distance_er_d;
volatile int32_t u_tran;

volatile uint8_t regulation_phase = 0;
volatile bool regulation_phase_init = false;

extern volatile float x;
extern volatile float y;
extern volatile float theta;

static float const INTERGAL_LIMIT = 80;
static float const KP = 10;
static float const KI = 0.0;
static float const KD = 0;

volatile static float error = 0;
volatile static float error_i = 0;
volatile static float error_d = 0;
volatile static float error_previous = 0;
volatile static int16_t u = 0;

volatile static int16_t left_offset = 0;

void
regulation_single_wheel (float referent_position, float measured_position)
{
  error = referent_position - measured_position;
  error_i += error;
  error_i = int_saturation (error_i, INTERGAL_LIMIT, -INTERGAL_LIMIT);
  error_d = error - error_previous;

  u = KP * error + KI * error_i + KD * error_d;
  u = int_saturation (u, SPEED_LIMIT, -SPEED_LIMIT);
  //ref_speed_left = int_saturation (u, MAXON_LIMIT_L, -MAXON_LIMIT_L);
  if (u > 20)
    {
      left_wheel_forwards ();
      left_offset = LEFT_MAXON_FORW_OFFSET;
    }
  else if (u < -20)
    {
      left_offset = LEFT_MAXON_BACK_OFFSET;
      left_wheel_backwards ();
    }
  else
    {
    left_offset = 0;
    stop_left_wheel();
    }
  u += left_offset;

  // Tj. ovde postavlja referencu za struju
  pwm_duty_cycle_left (abs (u));

  error_previous = error;
}

void
regulation_position ()
{
  calculate_movement ();

  switch (regulation_phase)

    {
    case ROT_TO_ANGLE:
      if (!regulation_phase_init)
	{
	  regulation_phase_init = true;
	  regulation_rotation_finished ();
	  regulation_translation_finished ();
	}
      regulation_rotation (theta_to_angle, 1);
      u_tran = 0;
      /*
       * POJAVI SE GRESKA U POZICIJI i NE KRECE SE
       * onda
       * OKRECI SE KA CILJU
       */
      if (distance > EPSILON_DISTANCE && no_movement ())
	{
	  regulation_phase_init = false;
	  regulation_rotation_finished ();
	  regulation_phase = ROT_TO_POS;
	}
      break;

    case ROT_TO_POS:
      if (!regulation_phase_init)
	{
	  regulation_phase_init = true;
	  regulation_rotation_finished ();
	  regulation_translation_finished ();
	}
      regulation_rotation (theta_to_pos, 1);
      u_tran = 0;
      /*
       * OKRENUO SE KA CILJU i NE KRECE SE VISE
       * onda
       * TRANSLIRAJ KA CILJU (sa rotacijom)
       */
      if (fabs (theta_to_pos) < EPSILON_THETA_SMALL && no_movement ())
	{
	  regulation_phase_init = false;
	  regulation_rotation_finished ();
	  regulation_phase = TRAN_WITH_ROT;
	}
      /* (ako se zada mala kretnja)
       *
       */
      if (fabs (distance) < EPSILON_DISTANCE && no_movement ())
	{
	  regulation_phase_init = false;
	  regulation_rotation_finished ();
	  regulation_phase = ROT_TO_ANGLE;
	}
      break;

    case TRAN_WITH_ROT:
      if (!regulation_phase_init)
	{
	  regulation_phase_init = true;
	  regulation_rotation_finished ();
	  regulation_translation_finished ();
	}
      regulation_translation (distance);
      regulation_rotation (theta_to_pos, 0.5);
      /*
       * PRIBLIZIO SE CILJU
       * onda
       * NASTAVI KA CILJU BEZ ROTACIJE
       */
      if (distance < EPSILON_DISTANCE_ROT)
	{
	  regulation_phase_init = false;
	  regulation_translation_finished ();
	  regulation_phase = TRAN_WITHOUT_ROT;
	}
      /*
       * POJAVILA SE VECA GRESKA U UGLU
       * onda
       * OKRENI SE KA CILJU
       */
      if (fabs (theta_to_pos) > EPSILON_THETA_BIG)
	{
	  regulation_phase_init = false;
	  regulation_translation_finished ();
	  regulation_phase = ROT_TO_POS;
	}
      break;

    case TRAN_WITHOUT_ROT:
      if (!regulation_phase_init)
	{
	  regulation_phase_init = true;
	  regulation_rotation_finished ();
	  regulation_translation_finished ();
	}
//TODO: razmisli kako ce robot da reaguje kad prebaci distancu, vidi kako su to u +381
      if (fabs (theta_to_pos) > (M_PI / 2))
	regulation_translation (-distance);
      else
	regulation_translation (distance);
      u_rot = 0;
      /*
       * NEMA GRESKE U POZICIJI
       * onda
       * DRZI ZADATI UGAO
       */
      if (distance < EPSILON_DISTANCE)
	{
	  regulation_phase_init = false;
	  regulation_translation_finished ();
	  regulation_phase = ROT_TO_ANGLE;
	}
      /*
       * POJAVI SE VECA GRESKA U POZICIJI
       * onda
       * VRATI SE U OKRETANJE KA CILJU
       */
      if (distance > EPSILON_DISTANCE_ROT)
	{
	  regulation_phase_init = false;
	  regulation_translation_finished ();
	  regulation_phase = ROT_TO_POS;
	}
      break;
    }

  ref_speed_right = int_ramp_simple (ref_speed_right, u_tran + u_rot, 1);
  //    ref_speed_right = u_tran + u_rot;	// bez rampe
  ref_speed_right = int_saturation (ref_speed_right, MAXON_LIMIT_R,
				    -MAXON_LIMIT_R);

  ref_speed_left = int_ramp_simple (ref_speed_left, u_tran - u_rot, 1);
  //    ref_speed_left = u_tran - u_rot;	// bez rampe
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

void
regulation_rotation (float theta_er, float faktor)
{
  theta_er_i += theta_er;
  theta_er_i = float_saturation (theta_er_i, THETA_I_LIMIT, -THETA_I_LIMIT);
  theta_er_d = theta_er - theta_er_previous;

  u_rot = (int32_t) (KP_ROT * theta_er + KI_ROT * theta_er_i
      + KD_ROT * theta_er_d);
  u_rot = int_saturation (u_rot, U_ROT_LIMIT, -U_ROT_LIMIT);
  u_rot *= faktor;

  theta_er_previous = theta_er;
}

void
regulation_translation (float distance_er)
{
  distance_er_i += distance_er;
  distance_er_i = float_saturation (distance_er_i, DISTANCE_I_LIMIT,
				    -DISTANCE_I_LIMIT);
  distance_er_d = distance_er - distance_er_previous;

  u_tran = (int32_t) (KP_TRAN * distance_er + KI_TRAN * distance_er_i
      + KD_TRAN * distance_er_d);
  u_tran = int_saturation (u_tran, U_TRAN_LIMIT, -U_TRAN_LIMIT);

  distance_er_previous = distance_er;
}

void
regulation_rotation_finished ()
{
  theta_er_i = 0;
  theta_er_d = 0;
  theta_er_previous = 0;
}

void
regulation_translation_finished ()
{
  distance_er_i = 0;
  distance_er_d = 0;
  distance_er_previous = 0;
}
