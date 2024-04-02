/*
 * tactics.h
 *
 *  Created on: Nov 14, 2023
 *      Author: lazar
 */

#ifndef LIB_MODULES_TACTICS_TACTICS_H_
#define LIB_MODULES_TACTICS_TACTICS_H_

#define BRAKE 200
#define RETURN 249
#define START 255
#define END 250
#define POSITIONING 253
#define RESET_BEFORE_START 254

bool
yellow_matijaV2 ();
bool
yellow_NSD ();
bool
yellow_4 ();
bool
yellow_risky();
bool
blue_4();
void
pop_plant ();
void
swap_first2_plants ();
void
swap_plant_alt (uint8_t i);
uint8_t
get_points ();
void
add_points (uint8_t number);

#endif /* LIB_MODULES_TACTICS_TACTICS_H_ */
