/*
 * task_modules.h
 *
 *  Created on: Feb 27, 2024
 *      Author: lazar
 */

#ifndef LIB_TACTICS_TASK_MODULES_H_
#define LIB_TACTICS_TASK_MODULES_H_

#include "../../Inc/main.h"
#define INTERRUPTED 200

typedef struct
{
  float x;
  float y;
} target;

bool
task_go_home (target** home_array_pointer);

#endif /* LIB_TACTICS_TASK_MODULES_H_ */
