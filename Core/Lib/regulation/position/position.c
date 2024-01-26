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

#define THETA_I_LIMIT		1
#define DISTANCE_I_LIMIT	1
#define V_REF_LIMIT		1	// m/s
#define W_REF_LIMIT		12.6	// rad/s

#define SPEED_LIMIT	1500 // inkrementi, direktno za pwm duty cycle

static const float KP_ROT = 0.8;
static const float KI_ROT = 0;
static const float KD_ROT = 0;
static const float KP_TRAN = 0.0003;
static const float KI_TRAN = 0;
static const float KD_TRAN = 0;

extern volatile float theta_to_pos;
extern volatile float theta_to_angle;
volatile static float theta_er_previous;
volatile static float theta_er_i;
volatile static float theta_er_d;

extern volatile float distance;
volatile static float distance_er_previous;
volatile static float distance_er_i;
volatile static float distance_er_d;

volatile float V_ref = 0, w_ref = 0;

volatile uint8_t regulation_phase = 0;
volatile bool regulation_phase_init = false;

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
      V_ref = 0;
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
      V_ref = 0;
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
      w_ref = 0;
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

  w_ref = KP_ROT * theta_er + KI_ROT * theta_er_i + KD_ROT * theta_er_d;
//  w_ref = float_saturation (w_ref, W_REF_LIMIT, -W_REF_LIMIT);
  w_ref = float_ramp(w_ref, float_saturation (w_ref, W_REF_LIMIT, -W_REF_LIMIT), 0.01);
  w_ref *= faktor;

  theta_er_previous = theta_er;
}

void
regulation_translation (float distance_er)
{
  distance_er_i += distance_er;
  distance_er_i = float_saturation (distance_er_i, DISTANCE_I_LIMIT,
				    -DISTANCE_I_LIMIT);
  distance_er_d = distance_er - distance_er_previous;

  V_ref = KP_TRAN * distance_er + KI_TRAN * distance_er_i
      + KD_TRAN * distance_er_d;
//  V_ref = float_saturation (V_ref, V_REF_LIMIT, -V_REF_LIMIT)
  V_ref = float_ramp(V_ref, float_saturation (V_ref, V_REF_LIMIT, -V_REF_LIMIT), 0.001);

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
