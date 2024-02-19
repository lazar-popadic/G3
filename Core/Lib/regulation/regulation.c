/*
 * regulation.c
 *
 *  Created on: Nov 16, 2023
 *      Author: lazar
 */

#include "regulation.h"
#include <stdlib.h>
#include "../pwm/pwm.h"
#include "../h-bridge/h-bridge.h"
#include <math.h>


volatile static uint8_t ramp_counter = 0;
volatile float V_limit, w_limit;

void
regulation_init ()
{
  V_limit = V_REF_LIMIT_DEFAULT;
  w_limit = W_REF_LIMIT_DEFAULT;
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

float
float_ramp (float signal, float desired_value, float slope)
{
  if (fabs (desired_value - signal) > slope)
    {
      return signal + sign (desired_value - signal) * slope;
    }
  return desired_value;
}

float
float_ramp2 (float signal, float desired_value, float slope_acceleration, float slope_deceleration)
{
  if ((desired_value - signal) > slope_acceleration)
    return signal + slope_acceleration;
  if ((signal - desired_value) > slope_deceleration)
    return signal - slope_deceleration;
  return desired_value;
}

int32_t
int_ramp_simple (int32_t signal, int32_t desired_value, int8_t slope)
{
  if (abs (desired_value - signal) > slope)
    {
      return signal + sign (desired_value - signal) * slope;
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
  if (abs (desired_value - signal) > slope)
    {
      return signal + sign (desired_value - signal) * slope;
    }
  return desired_value;
}

int8_t
sign (float signal)
{
  if (signal > 0)
    return 1;
  if (signal < 0)
    return -1;
  return 0;
}
