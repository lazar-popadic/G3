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

// narednih 10 do 100 iteracija vrednosti, ei ne sme preko toga ???
#define THETA_I_LIMIT		0.24
#define DISTANCE_I_LIMIT	2.4

static const float KP_ROT = 60.0;
static const float KI_ROT = 20.0;

static const float KP_TRAN = 0.036;
static const float KI_TRAN = 0.64;

extern volatile float theta_to_pos;
extern volatile float theta_to_angle;
volatile static float theta_er_i;

extern volatile float distance;
volatile static float distance_er_i;

volatile float V_ref = 0, w_ref = 0;
extern volatile float V_limit, w_limit;

volatile uint8_t regulation_phase = 0;
extern volatile float transition_factor;
extern volatile bool robot_moving;

void
regulation_position ()
{
  calculate_movement ();

  switch (regulation_phase)

    {
    case ROT_TO_ANGLE:
      if (fabs (distance) > EPSILON_DISTANCE * 2.0 && !robot_moving)
	{
	  regulation_phase = ROT_TO_POS;
	}
      //TODO: ako robot ne zabada vise onako (greska izmedju 10 i 20 mm) onda ovo radi posao
      else if (fabs (distance) > EPSILON_DISTANCE && !robot_moving)
	{
	  regulation_phase = TRAN_WITHOUT_ROT;
	}
      regulation_rotation (theta_to_angle, 1, 1);
      V_ref = 0;
      break;

    case ROT_TO_POS:
      if (fabs (theta_to_pos) < EPSILON_THETA_MEDIUM && !robot_moving)
	{
	  regulation_phase = TRAN_WITH_ROT;
	}
      /* (ako se zada mala kretnja)
       *
       */
//      if (fabs (distance) < EPSILON_DISTANCE && !robot_moving)
//	{
//	  regulation_phase = ROT_TO_ANGLE;
//	}
      regulation_rotation (theta_to_pos, 1, 1);
      V_ref = 0;
      break;

    case TRAN_WITH_ROT:
      if (fabs (distance) < EPSILON_DISTANCE_ROT)
	{
	  regulation_phase = TRAN_WITHOUT_ROT;
	}
      if (fabs (theta_to_pos) > EPSILON_THETA_BIG)
	{
	  regulation_phase = ROT_TO_POS;
	}
      if (fabs (theta_to_pos) > (M_PI / 2))
	regulation_translation (-distance, 1);
      else
	regulation_translation (distance, 1);
      regulation_rotation (theta_to_pos, 0.5, 1.0);
      break;

    case TRAN_WITHOUT_ROT:
      if (fabs (distance) < EPSILON_DISTANCE && !robot_moving)
	{
	  regulation_phase = ROT_TO_ANGLE;
	}
      if (fabs (distance) > EPSILON_DISTANCE_ROT * 2.0)
	{
	  regulation_phase = ROT_TO_POS;
	}
      if (fabs (theta_to_pos) > (M_PI / 2))
	regulation_translation (-distance, 1);
      else
	regulation_translation (distance, 1);
      w_ref = 0;
      break;
    }

}
/*
 * TODO: razmisli da li pokriva svaki slucaj!
 */

void
regulation_rotation (float theta_er, float factor, float limit_factor)
{
  static float w_ref_pid;
  theta_er_i += theta_er;
  theta_er_i = float_saturation (theta_er_i, THETA_I_LIMIT, -THETA_I_LIMIT);

  w_ref_pid = KP_ROT * theta_er + KI_ROT * theta_er_i;
  w_ref_pid = float_saturation (w_ref_pid, w_limit * limit_factor,
				-w_limit * limit_factor);
//  w_ref_pid = float_saturation2 (w_ref_pid, w_limit * limit_factor, 1.5, 0.15);
  w_ref = float_ramp_acc (w_ref, w_ref_pid, 3.2);
  w_ref *= factor;
}

void
regulation_translation (float distance_er, float factor)
{
  static float V_ref_pid;
  distance_er_i += distance_er;
  distance_er_i = float_saturation (distance_er_i, DISTANCE_I_LIMIT,
				    -DISTANCE_I_LIMIT);

  V_ref_pid = KP_TRAN * distance_er + KI_TRAN * distance_er_i;
  V_ref_pid = float_saturation (V_ref_pid, V_limit, -V_limit);
//  V_ref_pid = float_saturation2 (V_ref_pid, V_limit, 0.24, 0.5);
  V_ref = float_ramp_acc (V_ref, V_ref_pid, 0.36);
  V_ref *= factor; // TODO: ovo izbaci gore iznad saturacije
}

void
regulation_rotation_finished ()
{
  theta_er_i = 0;
}

void
regulation_translation_finished ()
{
  distance_er_i = 0;
}
