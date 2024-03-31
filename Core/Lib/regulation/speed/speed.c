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

#define V_LIMIT		1175
#define W_LIMIT		1175
#define U_LIMIT		1250

static const float KP_TRAN = 64.0;

static const float KP_ROT = 28.0;

extern volatile double V_m_s;
extern volatile double w_rad_s;
extern volatile float V_ref;
extern volatile float w_ref;

volatile static float e_v = 0;
volatile static float e_w = 0;

volatile static float u_v = 0;
volatile static float u_w = 0;
volatile float u_right = 0;
volatile float u_left = 0;

void
regulation_speed ()
{
  e_v = V_ref - V_m_s;

  e_w = w_ref - w_rad_s;

  u_v = KP_TRAN * e_v;
  u_v = float_saturation (u_v, V_LIMIT, -V_LIMIT);
  u_w = KP_ROT * e_w;
  u_w = float_saturation (u_w, W_LIMIT, -W_LIMIT);

  u_right = u_v + u_w;
  u_left = u_v - u_w;

  if (u_right > 10)
    right_wheel_forwards ();
  else if (u_right < -10)
    right_wheel_backwards ();
  else
    stop_right_wheel ();

  if (u_left > 10)
    left_wheel_forwards ();
  else if (u_left < -10)
    left_wheel_backwards ();
  else
    stop_left_wheel ();

  u_right = float_saturation (u_right, U_LIMIT, -U_LIMIT);
  u_left = float_saturation (u_left, U_LIMIT, -U_LIMIT);

  pwm_duty_cycle_right ((uint16_t) fabs (u_right));
  pwm_duty_cycle_left ((uint16_t) fabs (u_left));
}
