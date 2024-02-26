/*
 * ax12a.h
 *
 *  Created on: Nov 11, 2023
 *      Author: lazar
 */

#ifndef LIB_MODULES_AX12A_AX12A_H_
#define LIB_MODULES_AX12A_AX12A_H_

#include <stdint.h>

void
ax_move (uint8_t id, uint16_t angle, uint16_t speed);
void
solar_out_l ();
void
solar_in_l ();
void
solar_out_r ();
void
solar_in_r ();

#endif /* LIB_MODULES_AX12A_AX12A_H_ */
