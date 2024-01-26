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
#define V_LIMIT		2500
#define W_LIMIT		2500
#define U_LIMIT		2500

#define LEFT_MAXON_FORW_OFFSET	0	//bilo 490
#define LEFT_MAXON_BACK_OFFSET	0	//bilo -540
#define RIGHT_MAXON_FORW_OFFSET	0
#define RIGHT_MAXON_BACK_OFFSET	0

static const float KP_TRAN = 300.0;
static const float KI_TRAN = 0;
static const float KD_TRAN = 0;

static const float KP_ROT = 0.1;
static const float KI_ROT = 0;
static const float KD_ROT = 0;

extern volatile double V_m_s;
extern volatile double w_rad_s;
extern volatile float V_ref;
extern volatile float w_ref;

volatile static float e_v = 0;
volatile static float e_i_v = 0;
volatile static float e_d_v = 0;
volatile static float e_previous_v = 0;
volatile static float e_w = 0;
volatile static float e_i_w = 0;
volatile static float e_d_w = 0;
volatile static float e_previous_w = 0;

volatile static float u_v = 0;
volatile static float u_w = 0;
volatile static float u_right = 0;
volatile static float u_left = 0;
volatile static float left_offset = 0;
volatile static float right_offset = 0;

void
regulation_speed ()
{
  e_v = V_ref - V_m_s;
  e_i_v += e_v;
  e_i_v = float_saturation (e_i_v, EI_LIMIT, - EI_LIMIT);
  e_d_v = e_v - e_previous_v;

  e_w = w_ref - w_rad_s;
  e_i_w += e_w;
  e_i_w = float_saturation (e_i_w, EI_LIMIT, - EI_LIMIT);
  e_d_w = e_w - e_previous_w;

  u_v = KP_TRAN * e_v + KI_TRAN * e_i_v + KD_TRAN * e_d_v;
  u_v = float_saturation (u_v, V_LIMIT, -V_LIMIT);
  u_w = KP_ROT * e_w + KI_ROT * e_i_w + KD_ROT * e_d_w;
  u_w = float_saturation (u_w, W_LIMIT, -W_LIMIT);

  u_right = u_v + u_w;
  u_right = float_saturation (u_right, U_LIMIT, U_LIMIT);
  u_left = u_v - u_w;
  u_left = float_saturation (u_left, U_LIMIT, U_LIMIT);

  if (u_right > 50)
    {
      right_offset = RIGHT_MAXON_FORW_OFFSET;
      right_wheel_forwards ();
    }
  else if (u_right < -50)
    {
      right_offset = RIGHT_MAXON_BACK_OFFSET;
      right_wheel_backwards ();
    }
  else
    {
      right_offset = 0;
      stop_right_wheel ();
    }
  u_right += right_offset;
  if (u_left > 50)
    {
      left_offset = LEFT_MAXON_FORW_OFFSET;
      left_wheel_forwards ();
    }
  else if (u_left < -50)
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
