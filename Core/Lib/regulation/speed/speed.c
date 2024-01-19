/*
 * speed.c
 *
 *  Created on: Jan 3, 2024
 *      Author: lazar
 */

#include "speed.h"
#include "../regulation.h"
#include <stdlib.h>
#include "../../pwm/pwm.h"
#include "../../h-bridge/h-bridge.h"
#include <math.h>

#define EI_LIMIT 	40 	// narednih 10 do 100 iteracija vrednosti, ei ne sme preko toga ???
#define V_LIMIT	1000 		// inkrementi, direktno za pwm duty cycle
#define W_LIMIT	1000
#define LEFT_MAXON_FORW_OFFSET	490
#define LEFT_MAXON_BACK_OFFSET	-540

static const float KP_SPEED = 400.0;
static const float KI_SPEED = 0;
static const float KD_SPEED = 0;

extern volatile float V;
extern volatile float w;
extern volatile float V_ref;
extern volatile float w_ref;

volatile static int16_t e_v = 0;
volatile static int16_t e_i_v = 0;
volatile static int16_t e_d_v = 0;
volatile static int16_t e_previous_v = 0;
volatile static int16_t e_w = 0;
volatile static int16_t e_i_w = 0;
volatile static int16_t e_d_w = 0;
volatile static int16_t e_previous_w = 0;

volatile static float u_v = 0;
volatile static float u_w = 0;
volatile static int16_t u_right = 0;
volatile static int16_t u_left = 0;
volatile static int16_t left_offset = 0;
volatile static int16_t right_offset = 0;

void
regulation_speed ()
{
  e_v = V_ref - V;
  e_i_v += e_v;
  e_i_v = float_saturation (e_i_v, EI_LIMIT, - EI_LIMIT);
  e_d_v = e_v - e_previous_v;

  e_w = w_ref - w;
  e_i_w += e_w;
  e_i_w = float_saturation (e_i_w, EI_LIMIT, - EI_LIMIT);
  e_d_w = e_w - e_previous_w;

  u_v = KP_SPEED * e_v + KI_SPEED * e_i_v + KD_SPEED * e_d_v;
  u_v = float_saturation (u_v, V_LIMIT, -V_LIMIT);
  u_w = KP_SPEED * e_w + KI_SPEED * e_i_w + KD_SPEED * e_d_w;
  u_w = float_saturation (u_w, W_LIMIT, -W_LIMIT);

  u_right = u_v + u_w;
  u_left = u_v - u_w;

  if (u_right > 20)
    {
      right_offset = LEFT_MAXON_FORW_OFFSET;
      right_wheel_forwards ();
    }
  else if (u_right < -20)
    {
      right_offset = LEFT_MAXON_BACK_OFFSET;
      right_wheel_backwards ();
    }
  else
    {
      right_offset = 0;
      stop_right_wheel ();
    }
  u_right += right_offset;
  if (u_left > 20)
    {
      left_offset = LEFT_MAXON_FORW_OFFSET;
      left_wheel_forwards ();
    }
  else if (u_left < -20)
    {
      left_offset = LEFT_MAXON_BACK_OFFSET;
      left_wheel_backwards ();
    }
  else
    {
      left_offset = 0;
      stop_left_wheel ();
    }
  u_left += left_offset;

  // Tj. ovde postavlja referencu za struju
  // mozda float rampa ovde
  pwm_duty_cycle_right ((uint16_t) fabs (u_right));
  pwm_duty_cycle_left ((uint16_t) fabs (u_left));

  e_previous_v = e_v;
  e_previous_w = e_w;
}
