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
limit_angle (float angle);
float
float_normalize (float signal, float min, float max);

#endif /* LIB_MODULES_ODOMETRIJA_ODOMETRIJA_H_ */
