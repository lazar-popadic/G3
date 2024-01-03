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

//static float inc2rad_deltaT = 0;
volatile static uint8_t ramp_counter = 0;

void
regulation_init ()
{
  //inc2rad_deltaT = M_PI / 4096;	// 2*Pi / 4*2048
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
sign (int32_t signal)
{
  if (signal > 0)
    return 1;
  if (signal < 0)
    return -1;
  return 0;
}
