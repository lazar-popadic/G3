/*
 * pwm.h
 *
 *  Created on: Nov 15, 2023
 *      Author: lazar
 */

#ifndef LIB_MODULES_PWM_PWM_H_
#define LIB_MODULES_PWM_PWM_H_

#include <stdint.h>
#include "stm32f4xx.h"

void
pwm_init ();
void
pwm_duty_cycle (uint16_t duty_cycle);

#endif /* LIB_MODULES_PWM_PWM_H_ */
