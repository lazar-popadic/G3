/*
 * tactics.c
 *
 *  Created on: Nov 14, 2023
 *      Author: lazar
 */

#include "main.h"

#define AX_MAX_SPEED 528

extern volatile bool interrupted;
extern volatile uint8_t sensors_case_timer;
uint8_t tactic_state = 0;
bool tactic_state_init = false;
bool tactic_finished;
uint8_t home_counter = 0;
uint8_t home_side = 0;

volatile int8_t current_task_status;
volatile uint16_t current_task_time = 0;
extern uint16_t sys_time_s;
volatile uint8_t current_task_retries = 0;

volatile target plant_blue1 =
  { 1000, 1300 };
volatile target plant_blue2 =
  { 1000, 700 };
volatile target plant_central1 =
  { 1500, 1500 };
volatile target plant_central2 =
  { 1500, 500 };
volatile target plant_yellow1 =
  { 2000, 1300 };
volatile target plant_yellow2 =
  { 2000, 700 };

volatile target planter_blue_y =
  { 600 + (325 / 2) - 30, 2000 - 60 };
volatile target planter_blue_x_close =
  { 0, 2000 - 450 - (325 / 2) };
volatile target planter_blue_x_far =
  { 3000 - 0, 450 + (325 / 2) };
volatile target planter_yellow_y =
  { 3000 - 600 - (325 / 2) + 30, 2000 - 60 };
volatile target planter_yellow_x_close =
  { 3000 - 0, 2000 - 450 - (325 / 2) };
volatile target planter_yellow_x_far =
  { 0, 450 + (325 / 2) };

volatile target home_blue1 =
  { 450, 2000 - 450 };
volatile target home_blue2 =
  { 450, 450 };
volatile target home_blue3 =
  { 3000 - 450, 1000 };
volatile target home_yellow1 =
  { 3000 - 450, 2000 - 450 };
volatile target home_yellow2 =
  { 3000 - 450, 450 };
volatile target home_yellow3 =
  { 450, 1000 };

volatile target solar_blue =
  { 500, 190 };
volatile target solar_central =
  { 1500, 190 };
volatile target solar_yellow =
  { 2500, 190 };

target homes[2];
target plants[6];
target alt_plants[6];

bool
safe_yellow ()
{
  switch (tactic_state)
    {
    case 0:
      if (!tactic_state_init)
	{
	  tactic_state_init = true;
	  plants[0] = plant_yellow2;
	  plants[1] = plant_yellow1;
	  plants[2] = plant_central1;
	  plants[3] = plant_central2;
	  plants[4] = plant_blue2;
	  plants[5] = plant_blue1;

	  homes[0] = home_yellow2;
	  homes[1] = home_yellow3;
	  home_side = WALL;

	  tactic_finished = false;
	}
      current_task_time = sys_time_s;
      tactic_state++;
      tactic_state_init = false;
      break;
    case 1:
      current_task_status = task_pickup_plants (plants[0]);

      if (current_task_status == TASK_SUCCESS)
	{
	  reset_task ();
	  tactic_state++;
	  pop_plant ();
	  set_translation_speed_limit (1.0);
	  current_task_time = sys_time_s;
	  current_task_retries = 0;
	}
      else if (current_task_status == TASK_FAILED_1)
	{
	  current_task_retries++;
	  reset_movement ();
	  reset_task ();
	  if (!(current_task_retries % 3))
	    swap_first2_plants ();
	}
      break;

    case 2:
      current_task_status = task_dropoff_x_close_2 (YELLOW);
      if (current_task_status == TASK_SUCCESS)
	{
	  reset_task ();
	  tactic_state++;
	  current_task_time = sys_time_s;
	  current_task_retries = 0;
	}
      else if (current_task_status == TASK_FAILED_1) // na putu do polja, pre pomeranja saksija
	{
	  reset_movement ();
	  move_on_direction_2 (200, MECHANISM);
	  HAL_Delay (3000);
	  current_task_retries++;
	  reset_task ();
	  tactic_state = 10;
	}
      else if (current_task_status == TASK_FAILED_2)	// pri pomeranju saksija
	{
	  set_task_case (2);
	  reset_movement ();
	  current_task_retries++;
	}
      else if (current_task_status == TASK_FAILED_3)// dok se vraca do plantera
	{
	  current_task_retries++;
	  set_task_case (3);
	  reset_movement ();
	}
      else if (current_task_status == TASK_FAILED_4)// nakon sto je ostavio, dok se udaljava od plantera
	{
	  current_task_retries++;
	  set_task_case (8);
	  reset_movement ();
	}
      break;
    case 3:
      current_task_status = task_pickup_plants (plants[0]);
      if (current_task_status == TASK_SUCCESS)
	{
	  reset_task ();
	  tactic_state++;
	  pop_plant ();
	  current_task_time = sys_time_s;
	  current_task_retries = 0;
	}
      else if (current_task_status == TASK_FAILED_1)
	{
	  current_task_retries++;
	  reset_task ();
	  reset_movement ();
	  if (!(current_task_retries % 3))
	    swap_first2_plants ();
	}
      break;
    case 4:
      current_task_status = task_dropoff_y_2 (YELLOW);
      if (current_task_status == TASK_SUCCESS)
	{
	  reset_task ();
	  tactic_state++;
	  current_task_time = sys_time_s;
	  current_task_retries = 0;
	}
      else if (current_task_status == TASK_FAILED_1)	// na putu do plantera
	{
	  reset_movement ();
	  move_on_direction_2 (200, MECHANISM);
	  HAL_Delay (3000);
	  current_task_retries++;
	  reset_task ();
	  tactic_state = 20;
	}
      else if (current_task_status == TASK_FAILED_2)// nakon sto je ostavio, dok se udaljava od plantera
	{
	  current_task_retries++;
	  set_task_case (6);
	  reset_movement ();
	}
      break;
    case 5:
      current_task_status = task_solar (YELLOW, RESERVED, 1);
      if (current_task_status == TASK_SUCCESS)
	{
	  reset_task ();
	  tactic_state++;
	  current_task_time = sys_time_s;
	  current_task_retries = 0;
	}
      else if (current_task_status == TASK_FAILED_1)	// na putu do solara
	{
	  current_task_retries++;
	  reset_task ();
	  reset_movement ();
	  tactic_state = 30;
	}
      else if (current_task_status == TASK_FAILED_2)	// pri okretanju solara
	{
	  current_task_retries++;
	  set_task_case (5);
	  reset_movement ();
	}
      break;
    case HOME:
      current_task_status = task_go_home (homes[home_counter], home_side);
      if (current_task_status == TASK_SUCCESS)
	{
	  tactic_state = RETURN;
	  current_task_time = sys_time_s;
	  current_task_retries = 0;
	}
      if (current_task_status == TASK_FAILED_1)
	{
	  current_task_retries++;
	  reset_movement ();
	  reset_task ();
	  if (home_counter == 0)
	    {
	      home_counter++;
	      home_side++;
	      home_side %= 2;
	    }
	}
      break;

    case 10:
      current_task_status = task_dropoff_y_2 (YELLOW);
      if (current_task_status == TASK_SUCCESS)
	{
	  reset_task ();
	  tactic_state++;
	  current_task_time = sys_time_s;
	  current_task_retries = 0;
	}
      else if (current_task_status == TASK_FAILED_1)	// na putu do plantera
	{
	  reset_movement ();
	  move_on_direction_2 (200, MECHANISM);
	  HAL_Delay (3000);
	  current_task_retries++;
	  reset_task ();
	  tactic_state = 2;
	}
      else if (current_task_status == TASK_FAILED_2)// nakon sto je ostavio, dok se udaljava od plantera
	{
	  current_task_retries++;
	  set_task_case (6);
	  reset_movement ();
	}
      break;
    case 11:
      current_task_status = task_pickup_plants (plants[0]);
      if (current_task_status == TASK_SUCCESS)
	{
	  reset_task ();
	  tactic_state++;
	  pop_plant ();
	  current_task_time = sys_time_s;
	  current_task_retries = 0;
	}
      else if (current_task_status == TASK_FAILED_1)
	{
	  current_task_retries++;
	  reset_task ();
	  reset_movement ();
	  if (!(current_task_retries % 3))
	    swap_first2_plants ();
	}
      break;
    case 12:
      current_task_status = task_dropoff_x_close_2 (YELLOW);
      if (current_task_status == TASK_SUCCESS)
	{
	  reset_task ();
	  tactic_state = 5;
	  current_task_time = sys_time_s;
	  current_task_retries = 0;
	}
      else if (current_task_status == TASK_FAILED_1) // na putu do polja, pre pomeranja saksija
	{
	  reset_movement ();
	  move_on_direction_2 (200, MECHANISM);
	  HAL_Delay (3000);
	  current_task_retries++;
	  reset_task ();
	  tactic_state = 20;
	}
      else if (current_task_status == TASK_FAILED_2)	// pri pomeranju saksija
	{
	  set_task_case (2);
	  reset_movement ();
	  current_task_retries++;
	}
      else if (current_task_status == TASK_FAILED_3)// dok se vraca do plantera
	{
	  current_task_retries++;
	  set_task_case (3);
	  reset_movement ();
	}
      else if (current_task_status == TASK_FAILED_4)// nakon sto je ostavio, dok se udaljava od plantera
	{
	  current_task_retries++;
	  set_task_case (8);
	  reset_movement ();
	}
      break;

    case 20:
      current_task_status = task_solar (YELLOW, CENTRAL, 1);
      if (current_task_status == TASK_SUCCESS)
	{
	  reset_task ();
	  tactic_state++;
	  current_task_time = sys_time_s;
	  current_task_retries = 0;
	}
      else if (current_task_status == TASK_FAILED_1)	// na putu do solara
	{
	  current_task_retries++;
	  reset_movement ();
	}
      else if (current_task_status == TASK_FAILED_2)	// pri okretanju
	{
	  current_task_retries++;
	  reset_movement ();
	}
      break;
    case 21:
//      current_task_status = task_dropoff_x_far_2 (YELLOW);
      if (current_task_status == TASK_SUCCESS)
	{
	  reset_task ();
	  current_task_time = sys_time_s;
	  current_task_retries = 0;
	  homes[0] = home_yellow3;
	  homes[1] = home_yellow1;
	  tactic_state = HOME;
	}
      else if (current_task_status == TASK_FAILED_1) // na putu do polja, pre pomeranja saksija
	{
	  reset_task ();
	  current_task_retries++;
	  reset_movement ();
//	  tactic_state = HOME; TODO: nece valjda ovde failovati

	}
      else if (current_task_status == TASK_FAILED_2)	// pri pomeranju saksija
	{
	  current_task_retries++;
	  reset_movement ();
	}
      else if (current_task_status == TASK_FAILED_3)// dok se vraca do plantera
	{
	  current_task_retries++;
	  reset_movement ();
	}
      else if (current_task_status == TASK_FAILED_4)// nakon sto je ostavio, dok se udaljava od plantera
	{
	  current_task_retries++;
	  reset_movement ();
	}
      break;

    case 30:
      current_task_status = task_solar (YELLOW, CENTRAL, 1);
      if (current_task_status == TASK_SUCCESS)
	{
	  reset_task ();
	  homes[0] = home_yellow3;
	  homes[1] = home_yellow1;
	  tactic_state = HOME;
	  current_task_time = sys_time_s;
	  current_task_retries = 0;
	}
      else if (current_task_status == TASK_FAILED_1)	// na putu do solara
	{
	  reset_task ();
	  current_task_retries++;
	  reset_movement ();
	}
      else if (current_task_status == TASK_FAILED_2)	// pri okretanju solara
	{
	  current_task_retries++;
	  reset_movement ();
	}
      break;

    case RETURN:
      tactic_finished = true;
      break;

    }
  return tactic_finished;
}

bool
yellow_3 ()
{
  switch (tactic_state)
    {
    case 0:
      if (!tactic_state_init)
	{
	  tactic_state_init = true;
	  plants[0] = plant_central1;
	  plants[1] = plant_yellow1;
	  plants[2] = plant_blue2;
	  plants[3] = plant_yellow2;
	  plants[4] = plant_central2;
	  plants[5] = plant_blue1;

	  alt_plants[2] = plant_blue1;

	  homes[0] = home_yellow2;
	  homes[1] = home_yellow3;
	  home_side = WALL;

	  tactic_finished = false;
	}
      current_task_time = sys_time_s;
      tactic_state++;
      tactic_state_init = false;
      break;
    case 1:
      current_task_status = task_pickup_plants (plants[0]);

      if (current_task_status == TASK_SUCCESS)
	{
	  reset_task ();
	  tactic_state++;
	  pop_plant ();
	  set_translation_speed_limit (1.0);
	  current_task_time = sys_time_s;
	  current_task_retries = 0;
	}
      else if (current_task_status == TASK_FAILED_1)
	{
	  reset_movement ();
	  move_on_direction_2 (200, MECHANISM);
	  HAL_Delay (2000);
	  current_task_retries++;
	  reset_task ();
//	  if (!(current_task_retries % 3))
//	  swap_first2_plants ();
	}
      break;

    case 2:
      current_task_status = task_dropoff_y_2 (YELLOW);
      if (current_task_status == TASK_SUCCESS)
	{
	  reset_task ();
	  tactic_state++;
	  current_task_time = sys_time_s;
	  current_task_retries = 0;
	}
      else if (current_task_status == TASK_FAILED_1)	// na putu do plantera
	{
	  reset_movement ();
	  move_on_direction_2 (200, MECHANISM);
	  HAL_Delay (3000);
	  current_task_retries++;
	  reset_task ();
//	  tactic_state = 2;
	}
      else if (current_task_status == TASK_FAILED_2)// nakon sto je ostavio, dok se udaljava od plantera
	{
	  current_task_retries++;
	  set_task_case (6);
	  reset_movement ();
	}
      break;
    case 3:
      current_task_status = task_pickup_plants (plants[0]);
      if (current_task_status == TASK_SUCCESS)
	{
	  reset_task ();
	  tactic_state++;
	  pop_plant ();
	  current_task_time = sys_time_s;
	  current_task_retries = 0;
	}
      else if (current_task_status == TASK_FAILED_1)
	{
	  reset_movement ();
	  move_on_direction_2 (200, MECHANISM);
	  HAL_Delay (2000);
	  current_task_retries++;
	  reset_task ();
//	  if (!(current_task_retries % 3))
//	  swap_first2_plants ();
	}
      break;
    case 4:
      current_task_status = task_dropoff_x (YELLOW, CLOSE);
      if (current_task_status == TASK_SUCCESS)
	{
	  reset_task ();
	  tactic_state++;
	  current_task_time = sys_time_s;
	  current_task_retries = 0;
	}
      else if (current_task_status == TASK_FAILED_1)	// na putu do plantera
	{
	  reset_movement ();
	  move_on_direction_2 (200, MECHANISM);
	  HAL_Delay (2000);
	  current_task_retries++;
	  reset_task ();
//	  tactic_state = 20;
	}
      else if (current_task_status == TASK_FAILED_2)// nakon sto je ostavio, dok se udaljava od plantera
	{
	  current_task_retries++;
	  set_task_case (6);
	  reset_movement ();
	}
      break;
    case 5:
      current_task_status = task_pickup_plants (plants[0]);

      if (current_task_status == TASK_SUCCESS)
	{
	  reset_task ();
	  tactic_state++;
	  pop_plant ();
	  set_translation_speed_limit (1.0);
	  current_task_time = sys_time_s;
	  current_task_retries = 0;
	}
      else if (current_task_status == TASK_FAILED_1)
	{
	  reset_movement ();
	  move_on_direction_2 (200, MECHANISM);
	  HAL_Delay (2000);
	  current_task_retries++;
	  reset_task ();
//	  if (!(current_task_retries % 3))
	  swap_plant_alt (2);
	}
      break;
    case 6:
      current_task_status = task_dropoff_x (YELLOW, FAR);
      if (current_task_status == TASK_SUCCESS)
	{
	  reset_task ();
	  tactic_state++;
	  current_task_time = sys_time_s;
	  current_task_retries = 0;
	}
      else if (current_task_status == TASK_FAILED_1)	// na putu do plantera
	{
	  reset_movement ();
	  move_on_direction_2 (200, MECHANISM);
	  HAL_Delay (2000);
	  current_task_retries++;
	  reset_task ();
//	  tactic_state = 20;
	}
      else if (current_task_status == TASK_FAILED_2)// nakon sto je ostavio, dok se udaljava od plantera
	{
	  current_task_retries++;
	  set_task_case (6);
	  reset_movement ();
	}
      break;
    case 7:
      current_task_status = task_solar (YELLOW, CENTRAL, 1);
      if (current_task_status == TASK_SUCCESS)
	{
	  reset_task ();
	  tactic_state++;
	  current_task_time = sys_time_s;
	  current_task_retries = 0;
	}
      else if (current_task_status == TASK_FAILED_1)	// na putu do solara
	{
	  current_task_retries++;
	  reset_task ();
	  reset_movement ();
//	  tactic_state = 30;
	}
      else if (current_task_status == TASK_FAILED_2)	// pri okretanju solara
	{
	  current_task_retries++;
	  set_task_case (5);
	  reset_movement ();
	}
      break;
    case 8:
      current_task_status = task_pickup_plants (plants[0]);

      if (current_task_status == TASK_SUCCESS)
	{
	  reset_task ();
	  tactic_state++;
	  pop_plant ();
	  set_translation_speed_limit (1.0);
	  current_task_time = sys_time_s;
	  current_task_retries = 0;
	}
      else if (current_task_status == TASK_FAILED_1)
	{
	  reset_movement ();
	  reset_task ();
	  tactic_state++;
	}
      break;
    case 9:
      current_task_status = task_solar (YELLOW, RESERVED, 1);
      if (current_task_status == TASK_SUCCESS)
	{
	  reset_task ();
	  tactic_state++;
	  current_task_time = sys_time_s;
	  current_task_retries = 0;
	}
      else if (current_task_status == TASK_FAILED_1)	// na putu do solara
	{
	  current_task_retries++;
	  reset_task ();
	  reset_movement ();
	  //	  tactic_state = 30;
	}
      else if (current_task_status == TASK_FAILED_2)	// pri okretanju solara
	{
	  current_task_retries++;
	  set_task_case (5);
	  reset_movement ();
	}
      break;
    case 10:
      current_task_status = task_pot_reserved (YELLOW);

      if (current_task_status == TASK_SUCCESS
	  || current_task_status == TASK_FAILED_2)
	{
	  reset_task ();
	  tactic_state++;
	  current_task_time = sys_time_s;
	  current_task_retries = 0;
	}
      else if (current_task_status == TASK_FAILED_1)
	{
	  reset_movement ();
	  reset_task ();
	  tactic_state++;
	}
      break;

    case HOME:
      current_task_status = task_go_home (homes[home_counter], home_side);
      if (current_task_status == TASK_SUCCESS)
	{
	  tactic_state = RETURN;
	  current_task_time = sys_time_s;
	  current_task_retries = 0;
	}
      if (current_task_status == TASK_FAILED_1)
	{
	  current_task_retries++;
	  reset_movement ();
	  reset_task ();
//	  if (home_counter == 0)
//	    {
//	      home_counter++;
//	      home_side++;
//	      home_side %= 2;
//	    }
	}
      break;

    case RETURN:
      tactic_finished = true;
      break;

    }
  return tactic_finished;
}

bool
yellow_4 ()
{
  switch (tactic_state)
    {
    case 0:
      if (!tactic_state_init)
	{
	  tactic_state_init = true;
	  plants[0] = plant_central2;
	  plants[1] = plant_yellow1;
	  plants[2] = plant_yellow2;
	  plants[3] = plant_central1;
//	  plants[4] = plant_central2;
//	  plants[5] = plant_blue1;

	  alt_plants[2] = plant_blue1;

	  homes[0] = home_yellow3;
	  homes[1] = home_yellow1;
	  home_side = WALL;

	  tactic_finished = false;
	}
      current_task_time = sys_time_s;
      tactic_state++;
      tactic_state_init = false;
      break;
    case 1:
      current_task_status = task_pickup_plants (plants[0]);

      if (current_task_status == TASK_SUCCESS)
	{
	  reset_task ();
	  tactic_state++;
	  pop_plant ();
	  set_translation_speed_limit (1.0);
	  current_task_time = sys_time_s;
	  current_task_retries = 0;
	}
      else if (current_task_status == TASK_FAILED_1)
	{
	  reset_movement ();
	  current_task_retries++;
	  reset_task ();
	  // TODO: ovde stavi alt biljku, i zameni i drugi target ako to radis
	}
      break;

    case 2:
      current_task_status = task_solar (YELLOW, CENTRAL, 1);
      if (current_task_status == TASK_SUCCESS)
	{
	  reset_task ();
	  tactic_state++;
	  current_task_time = sys_time_s;
	  current_task_retries = 0;
	}
      else if (current_task_status == TASK_FAILED_1)	// na putu do solara
	{
	  current_task_retries++;
	  reset_task ();
	  reset_movement ();
	  //	  tactic_state = 30;
	}
      else if (current_task_status == TASK_FAILED_2)	// pri okretanju solara
	{
	  current_task_retries++;
	  set_task_case (5);
	  reset_movement ();
	}
      break;

    case 3:
      sensors_case_timer = SENSORS_HIGH;
      set_translation_speed_limit (1.0);
      move_to_xy (home_yellow2.x, home_yellow2.y, WALL);
      if (movement_finished () && timer_delay_nonblocking (20))
	{
	  tactic_state++;
	}
      if (interrupted)
	{
	  current_task_retries++;
	  reset_movement ();
	}
      break;

    case 4:
      current_task_status = task_pot_reserved (YELLOW);

      if (current_task_status == TASK_SUCCESS
	  || current_task_status == TASK_FAILED_2)
	{
	  reset_task ();
	  tactic_state++;
	  current_task_time = sys_time_s;
	  current_task_retries = 0;
	}
      else if (current_task_status == TASK_FAILED_1)
	{
	  reset_movement ();
	  reset_task ();
	  tactic_state++;
	}
      else if (current_task_status == TASK_FAILED_2)
	{
	  reset_movement ();
	  reset_task ();
	  tactic_state++;
	}
      break;

    case 5:
      current_task_status = task_pickup_plants (plants[0]);

      if (current_task_status == TASK_SUCCESS)
	{
	  reset_task ();
	  tactic_state++;
	  pop_plant ();
	  set_translation_speed_limit (1.0);
	  current_task_time = sys_time_s;
	  current_task_retries = 0;
	}
      else if (current_task_status == TASK_FAILED_1)
	{
	  reset_movement ();
	  current_task_retries++;
	  reset_task ();
	  // TODO: ovde stavi alt biljku
	}
      break;

    case 6:
      current_task_status = task_dropoff_x (YELLOW, CLOSE);
      if (current_task_status == TASK_SUCCESS)
	{
	  reset_task ();
	  tactic_state++;
	  current_task_time = sys_time_s;
	  current_task_retries = 0;
	}
      else if (current_task_status == TASK_FAILED_1)	// na putu do plantera
	{
	  reset_movement ();
	  current_task_retries++;
	  reset_task ();
//	  tactic_state = 20;
	}
      else if (current_task_status == TASK_FAILED_2)	// pri pomeranju saksija
	{
	  set_task_case (2);
	  reset_movement ();
	  current_task_retries++;
	}
      else if (current_task_status == TASK_FAILED_3)// dok se vraca do plantera
	{
	  current_task_retries++;
	  set_task_case (3);
	  reset_movement ();
	}
      else if (current_task_status == TASK_FAILED_4)// nakon sto je ostavio, dok se udaljava od plantera
	{
	  current_task_retries++;
	  set_task_case (8);
	  reset_movement ();
	}
      break;
    case 7:
      current_task_status = task_pickup_plants (plants[0]);

      if (current_task_status == TASK_SUCCESS)
	{
	  reset_task ();
	  tactic_state++;
	  pop_plant ();
	  set_translation_speed_limit (1.0);
	  current_task_time = sys_time_s;
	  current_task_retries = 0;
	}
      else if (current_task_status == TASK_FAILED_1)
	{
	  reset_movement ();
	  current_task_retries++;
	  reset_task ();
	  // TODO: ovde stavi alt biljku
	}
      break;

    case 8:
      current_task_status = task_solar (YELLOW, RESERVED, 1.0);
      if (current_task_status == TASK_SUCCESS)
	{
	  reset_task ();
	  tactic_state++;
	  current_task_time = sys_time_s;
	  current_task_retries = 0;
	}
      else if (current_task_status == TASK_FAILED_1)	// na putu do solara
	{
	  current_task_retries++;
	  reset_task ();
	  reset_movement ();
	  //	  tactic_state = 30;
	}
      else if (current_task_status == TASK_FAILED_2)	// pri okretanju solara
	{
	  current_task_retries++;
	  set_task_case (5);
	  reset_movement ();
	}
      break;

    case 9:
      current_task_status = task_dropoff_y_2 (YELLOW);
      if (current_task_status == TASK_SUCCESS)
	{
	  reset_task ();
	  tactic_state++;
	  current_task_time = sys_time_s;
	  current_task_retries = 0;
	}
      else if (current_task_status == TASK_FAILED_1)	// na putu do plantera
	{
	  reset_movement ();
	  current_task_retries++;
	  reset_task ();
//	  tactic_state = 2;
	}
      else if (current_task_status == TASK_FAILED_2)// nakon sto je ostavio, dok se udaljava od plantera
	{
	  current_task_retries++;
	  set_task_case (6);
	  reset_movement ();
	}
      break;
    case 10:
      current_task_status = task_pickup_plants (plants[0]);

      if (current_task_status == TASK_SUCCESS)
	{
	  reset_task ();
	  tactic_state++;
	  pop_plant ();
	  set_translation_speed_limit (1.0);
	  current_task_time = sys_time_s;
	  current_task_retries = 0;
	}
      else if (current_task_status == TASK_FAILED_1)
	{
	  reset_movement ();
	  current_task_retries++;
	  reset_task ();
	  // TODO: ovde stavi alt biljku
	}
      break;

    case 11:
      current_task_status = task_dropoff_x (YELLOW, FAR);
      if (current_task_status == TASK_SUCCESS)
	{
	  reset_task ();
	  tactic_state++;
	  current_task_time = sys_time_s;
	  current_task_retries = 0;
	}
      else if (current_task_status == TASK_FAILED_1)	// na putu do plantera
	{
	  reset_movement ();
	  current_task_retries++;
	  reset_task ();
//	  tactic_state = 20;
	}
      else if (current_task_status == TASK_FAILED_2)	// pri pomeranju saksija
	{
	  set_task_case (2);
	  reset_movement ();
	  current_task_retries++;
	}
      else if (current_task_status == TASK_FAILED_3)// dok se vraca do plantera
	{
	  current_task_retries++;
	  set_task_case (3);
	  reset_movement ();
	}
      else if (current_task_status == TASK_FAILED_4)// nakon sto je ostavio, dok se udaljava od plantera
	{
	  current_task_retries++;
	  set_task_case (8);
	  reset_movement ();
	}
      break;

    case HOME:
      current_task_status = task_go_home (homes[home_counter], home_side);
      if (current_task_status == TASK_SUCCESS)
	{
	  tactic_state = RETURN;
	  current_task_time = sys_time_s;
	  current_task_retries = 0;
	}
      if (current_task_status == TASK_FAILED_1)
	{
	  current_task_retries++;
	  reset_movement ();
	  reset_task ();
//	  if (home_counter == 0)
//	    {
//	      home_counter++;
//	      home_side++;
//	      home_side %= 2;
//	    }
	}
      break;

    case RETURN:
      tactic_finished = true;
      break;

    }
  return tactic_finished;
}

bool
risky_yellow ()
{
  switch (tactic_state)
    {
    case 0:
      if (!tactic_state_init)
	{
	  tactic_state_init = true;
	  plants[0] = plant_blue2;
	  plants[1] = plant_yellow1;
	  plants[2] = plant_yellow2;
	  plants[3] = plant_central1;
	  plants[4] = plant_central2;
	  plants[5] = plant_blue1;

	  homes[0] = home_yellow1;
	  homes[1] = home_yellow2;
	  home_side = MECHANISM;

	  tactic_finished = false;
	}
      tactic_state++;
      tactic_state_init = false;
      break;
    case 1:
      current_task_status = task_pickup_plants (plants[0]);

      if (current_task_status == TASK_SUCCESS)
	{
	  reset_task ();
	  tactic_state++;
	  pop_plant ();
	  set_translation_speed_limit (1.0);
	  current_task_time = sys_time_s;
	  current_task_retries = 0;
	}
      else if (current_task_status == TASK_FAILED_1)
	{
	  current_task_retries++;
	  reset_task ();
	  reset_movement ();
	  swap_first2_plants ();
	  tactic_state = 1;

	}
      break;

    case 2:
//      current_task_status = task_dropoff_x_far_2 (YELLOW);
      if (current_task_status == TASK_SUCCESS)
	{
	  reset_task ();
	  tactic_state++;
	  current_task_time = sys_time_s;
	  current_task_retries = 0;
	}
      break;
    case 3:
      current_task_status = task_pickup_plants (plants[0]);
      if (current_task_status == TASK_SUCCESS)
	{
	  reset_task ();
	  tactic_state++;
	  pop_plant ();
	  current_task_time = sys_time_s;
	  current_task_retries = 0;
	}
      else if (current_task_status == TASK_FAILED_1)
	{
	  current_task_retries++;
	  reset_task ();
	  reset_movement ();
	  swap_first2_plants ();
	  tactic_state = 1;

	}
      break;
    case 4:
      current_task_status = task_dropoff_y_2 (YELLOW);
      if (current_task_status == TASK_SUCCESS)
	{
	  reset_task ();
	  tactic_state++;
	  current_task_time = sys_time_s;
	  current_task_retries = 0;
	}
      break;
    case 5:
      current_task_status = task_pickup_plants (plants[0]);
      if (current_task_status == TASK_SUCCESS)
	{
	  reset_task ();
	  tactic_state++;
	  pop_plant ();
	  current_task_time = sys_time_s;
	  current_task_retries = 0;
	}
      else if (current_task_status == TASK_FAILED_1)
	{
	  current_task_retries++;
	  reset_task ();
	  reset_movement ();
	  swap_first2_plants ();
	  tactic_state = 1;

	}
      break;
    case 7:
      current_task_status = task_go_home (homes[home_counter], home_side);
      if (current_task_status == TASK_SUCCESS)
	{
	  tactic_state = RETURN;
	  current_task_time = sys_time_s;
	  current_task_retries = 0;
	}
      if (current_task_status == TASK_FAILED_1)
	{
	  current_task_retries++;
	  reset_task ();
	  if (home_counter == 0)
	    {
	      home_counter++;
	      reset_task ();
	      reset_movement ();
	      home_side++;
	      home_side %= 2;
	    }
	}
      break;
    case RETURN:
      tactic_finished = true;
      break;

    }
  return tactic_finished;

}

void
pop_plant ()
{
  for (int i = 0; i < 5; i++)
    {
      plants[i] = plants[i + 1];
    }
  plants[5].x = 1500;
  plants[5].y = 1000;
}

void
swap_first2_plants ()
{
  target temp = plants[0];
  plants[0] = plants[1];
  plants[1] = temp;
}

void
swap_plant_alt (uint8_t i)
{
  target temp = plants[i];
  plants[i] = alt_plants[i];
  alt_plants[i] = temp;
}
