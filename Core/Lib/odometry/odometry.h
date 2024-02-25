/*
 * odometry.h
 *
 *  Created on: Nov 2, 2023
 *      Author: lazar
 */

#ifndef LIB_MODULES_ODOMETRIJA_ODOMETRIJA_H_
#define LIB_MODULES_ODOMETRIJA_ODOMETRIJA_H_

typedef struct
{
  float x_mm;
  float y_mm;
  float theta_rad;
} position;

void
odometry_robot ();
void
odometry_init ();
void
normalize_robot_angle ();
float
float_normalize_angle (float signal, float middle);
float
simple_normalize (float signal);

#endif /* LIB_MODULES_ODOMETRIJA_ODOMETRIJA_H_ */
