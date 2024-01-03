/*
 * position.h
 *
 *  Created on: Jan 3, 2024
 *      Author: lazar
 */

#ifndef LIB_REGULATION_POSITION_POSITION_H_
#define LIB_REGULATION_POSITION_POSITION_H_

#include <stdint.h>

void
regulation_position ();
void
regulation_single_wheel (int16_t referent_position, int16_t measured_position);

#endif /* LIB_REGULATION_POSITION_POSITION_H_ */
