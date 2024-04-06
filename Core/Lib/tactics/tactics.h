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
blue_381 ();
bool
yellow_nts ();
bool
yellow_381 ();
bool
yellow_matijaV2 ();
bool
yellow_NSD ();
bool
yellow_4 ();
bool
yellow_risky();
bool
yellow_neo ();
bool
blue_matijaV2 ();
bool
blue_4();
bool
blue_risky ();
bool
blue_NSD ();
bool
blue_neo ();
bool
blue_memra ();

bool
homologation ();
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
uint8_t
uint8_t_min (uint8_t a, uint8_t b);

#endif /* LIB_MODULES_TACTICS_TACTICS_H_ */
