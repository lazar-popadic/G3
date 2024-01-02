/*
 * desired_position.c
 *
 *  Created on: Jan 2, 2024
 *      Author: lazar
 */

#include "desired_position.h"

extern volatile float p0x;
extern volatile float p0y;
extern volatile float theta_0;

void
set_xytheta (float desired_x, float desired_y, float desired_theta)
{
  p0x = desired_x;
  p0y = desired_y;
  theta_0 = desired_theta;
}
