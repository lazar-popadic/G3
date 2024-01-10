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

#define EI_LIMIT 	40 // narednih 10 do 100 iteracija vrednosti, ei ne sme preko toga ???
#define SPEED_LIMIT	1500 // inkrementi, direktno za pwm duty cycle
#define LEFT_MAXON_FORW_OFFSET	490
#define LEFT_MAXON_BACK_OFFSET	-540

static const float KP_SPEED = 6.0;
static const float KI_SPEED = 0;
static const float KD_SPEED = 0;

extern volatile int16_t ref_speed_right;
extern volatile int16_t ref_speed_left;

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
volatile static int16_t left_offset = 0;

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
  // calculate offset
  if (u_left > 20)
    left_offset = LEFT_MAXON_FORW_OFFSET;
  else if (u_left < -20)
    left_offset = LEFT_MAXON_BACK_OFFSET;
  else
    left_offset = 0;
  u_left += left_offset;

  // Tj. ovde postavlja referencu za struju
  pwm_duty_cycle_right (abs (u_right));
  pwm_duty_cycle_left (abs (u_left));

  e_previous_left = e_left;
  e_previous_right = e_right;
}
