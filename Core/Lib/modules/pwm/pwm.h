/*
 * pwm.h
 *
 *  Created on: Nov 15, 2023
 *      Author: lazar
 */

#ifndef LIB_MODULES_PWM_PWM_H_
#define LIB_MODULES_PWM_PWM_H_

#include <stdint.h>

void
pwm_init ();
void
pwm_duty_cycle_right_maxon (float duty_cycle_percentage);
void
pwm_duty_cycle_left_maxon (float duty_cycle_percentage);

#endif /* LIB_MODULES_PWM_PWM_H_ */
