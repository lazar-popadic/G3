/*
 * pwm.h
 *
 *  Created on: Nov 15, 2023
 *      Author: lazar
 */

#ifndef LIB_PERIPHERY_PWM_PWM_H_
#define LIB_PERIPHERY_PWM_PWM_H_

#include <stdint.h>

void
pwm_init ();
void
pwm_start ();
void
pwm_duty_cycle_right (uint16_t duty_cycle);
void
pwm_duty_cycle_left (uint16_t duty_cycle);

#endif /* LIB_PERIPHERY_PWM_PWM_H_ */
