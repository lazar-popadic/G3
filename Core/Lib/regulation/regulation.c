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

float
float_saturation2 (float signal, float MAX, float MIN, float limit)
{
  if (fabs (signal) > MAX)
    return sign (signal) * MAX;
  if (fabs (signal) < limit)
    return 0;
  if (fabs (signal) < MIN)
    return sign (signal) * MIN;
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
float_ramp2 (float signal, float desired_value, float slope_acceleration,
	     float slope_deceleration)
{
  static float abs_desired = 0;
  static float abs_signal = 0;
  abs_desired = fabs (desired_value);
  abs_signal = fabs (signal);
  if ((abs_desired - abs_signal) > slope_acceleration)
    return signal + sign (desired_value - signal) * slope_acceleration;
  if ((abs_signal - abs_desired) > slope_deceleration)
    return signal + sign (desired_value - signal) * slope_deceleration;
  return desired_value;
}

float
float_ramp_acc (float signal, float desired_value, float slope_acceleration)
{
  if ((fabs (desired_value) - fabs (signal)) > slope_acceleration)
    return signal + sign (desired_value - signal) * slope_acceleration;
  return desired_value;
}

float
float_ramp_brake (float signal, float deceleration)
{
  if (fabs (signal) > deceleration)
    return signal - sign (signal) * deceleration;
  return 0;
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
