/*
 * tactics.c
 *
 *  Created on: Nov 14, 2023
 *      Author: lazar
 */

#include "main.h"

#define AX_MAX_SPEED 528

#define PLANT_1		110		// uzimanje prvih biljaka
#define PLANT_2		111		//	...
#define PLANT_3		112
#define PLANT_4		113
#define PLANT_5		114
#define PLANT_6		115
#define SOLAR_R		120		// rezervisani solari
#define SOLAR_C		121		// centralni solari
#define DROP_X_C	130		// ostavi u planter blizi svojoj strani
#define DROP_X_F	131		// ostavi u planter dalji od svoje strani
#define DROP_X_C_ALT	132
#define DROP_X_F_ALT	133
#define DROP_Y		134		// ostavi u planter kod sime
#define POT		140		// samo ostavi u saksije kod rezervisanog polja
#define POT_2		141		// ostavi u saksije kod rezervisanog polja i gurni ih u polje
#define POT_3     142 // saksije kod najudaljenijeg polja
#define POT_4     143  // gurni saksije kod najudaljenijeg polja
#define POT_5     144  // saksije izmedju retyerbisanih i sredisnjih solara

volatile uint8_t points = 0;

extern position robot_position;
extern volatile bool interrupted;
extern volatile uint8_t sensors_case_timer;
uint8_t tactic_state = 0;
bool tactic_state_init = false;
bool tactic_finished;
uint8_t home_counter = 0;
uint8_t home_side = 0;

volatile int8_t current_task_status;
extern uint16_t sys_time_s;
volatile uint8_t current_task_retries = 0;
extern volatile float distance;
volatile uint8_t alt = 0;

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
  { 600 + (325 / 2), 2000 - 60 };
volatile target planter_blue_x_close =
  { 0, 2000 - 450 - (325 / 2) };
volatile target planter_blue_x_far =
  { 3000 - 0, 450 + (325 / 2) };
volatile target planter_yellow_y =
  { 3000 - 600 - (325 / 2), 2000 - 60 };
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
volatile target home_blue3_close =
  { 3000 - 300, 1000 };
volatile target home_yellow1 =
  { 3000 - 450, 2000 - 450 };
volatile target home_yellow2 =
  { 3000 - 450, 450 };
volatile target home_yellow3 =
  { 450, 1000 };
volatile target home_yellow3_close =
  { 300, 1000 };

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
yellow_matijaV2 ()
{
  switch (tactic_state)
    {

    case 0:
      if (!tactic_state_init)
	{
	  tactic_state_init = true;
	  plants[0] = plant_central1;
	  plants[1] = plant_yellow1;
	  plants[2] = plant_yellow2;
	  plants[3] = plant_central2;
//	  plants[4] = plant_central2;
//	  plants[5] = plant_blue1;

	  homes[0] = home_yellow2;
	  homes[1] = home_yellow3_close;
	  home_side = WALL;

	  tactic_finished = false;
	}
      tactic_state = PLANT_1;
      tactic_state_init = false;
      break;

    case PLANT_1:
      current_task_status = task_pickup_plants (plants[0], 1);

      if (current_task_status == TASK_SUCCESS)
	{
	  reset_task ();
	  tactic_state = 1;

	  set_translation_speed_limit (1.0);
	  current_task_retries = 0;
	}
      else if (current_task_status == TASK_FAILED_1)
	{
	  reset_movement ();
	  current_task_retries++;

	  reset_task ();

	}
      break;

    case 1:
      sensors_case_timer = SENSORS_HIGH;
      set_translation_speed_limit (1.0);
      move_to_xy (home_yellow1.x, 2000 - 250, WALL);
      if (movement_finished () && timer_delay_nonblocking (20))
	{
	  current_task_retries = 0;
	  tactic_state = POT;
	}
      if (interrupted)
	{
	  current_task_retries++;
	  reset_movement ();
	}
      break;

    case POT:
      current_task_status = task_pot_reserved (YELLOW);

      if (current_task_status == TASK_SUCCESS)
	{
	  points += get_and_reset_task_points ();
	  current_task_retries = 0;
	  reset_task ();
	  tactic_state = PLANT_2;
	  current_task_retries = 0;
	}
      else if (current_task_status == TASK_FAILED_1)
	{
	  reset_movement ();
	  reset_task ();
	  current_task_retries++;

	}
      else if (current_task_status == TASK_FAILED_2)
	{
	  reset_movement ();
	  set_task_case (4);
	}
      break;

    case PLANT_2:
      current_task_status = task_pickup_plants (plants[1], 1);

      if (current_task_status == TASK_SUCCESS)
	{
	  reset_task ();
	  tactic_state = DROP_X_C;

	  set_translation_speed_limit (1.0);
	  current_task_retries = 0;
	}
      else if (current_task_status == TASK_FAILED_1)
	{
	  reset_movement ();
	  current_task_retries++;
	  reset_task ();

	}
      break;

    case DROP_X_C:
      current_task_status = task_dropoff_x (YELLOW, CLOSE);
      if (current_task_status == TASK_SUCCESS)
	{
	  points += get_and_reset_task_points ();
	  reset_task ();
	  tactic_state = 2;
	  current_task_retries = 0;
	}
      else if (current_task_status == TASK_FAILED_1)	// na putu do plantera
	{
	  reset_movement ();
	  current_task_retries++;
	  reset_task ();
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
	  set_task_case (7);
	  reset_movement ();
	}
      break;

    case 2:
      sensors_case_timer = SENSORS_HIGH;
      set_translation_speed_limit (1.0);
      move_to_xy (2500, 900, WALL);
      if (movement_finished () && timer_delay_nonblocking (20))
	{
	  current_task_retries = 0;
	  tactic_state = 3;
	}
      if (interrupted)
	{
	  current_task_retries++;
	  reset_movement ();
	}
      break;

    case 3:
      sensors_case_timer = SENSORS_MECHANISM;
      set_translation_speed_limit (0.5);
      move_to_xy (2000, 700, MECHANISM);
      if (movement_finished () && timer_delay_nonblocking (20))
	{
	  set_translation_speed_limit (1.0);
	  current_task_retries = 0;
	  tactic_state = PLANT_3;
	}
      if (interrupted)
	{
	  current_task_retries++;
	  reset_movement ();
	}
      break;

    case PLANT_3:
      current_task_status = task_pickup_plants (plants[2], 1);

      if (current_task_status == TASK_SUCCESS)
	{
	  reset_task ();
	  tactic_state = POT_5;

	  set_translation_speed_limit (1.0);
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

    case POT_5:
      current_task_status = task_pot_solar (YELLOW);

      if (current_task_status == TASK_SUCCESS)
	{
	  points += get_and_reset_task_points ();
	  current_task_retries = 0;
	  reset_task ();
	  tactic_state = PLANT_4;
	  current_task_retries = 0;
	}
      else if (current_task_status == TASK_FAILED_1)
	{
	  reset_movement ();
	  reset_task ();
	  current_task_retries++;

	}
      else if (current_task_status == TASK_FAILED_2)
	{
	  reset_movement ();
	  set_task_case (4);
	}
      break;

    case SOLAR_C:
      current_task_status = task_solar (YELLOW, CENTRAL, 1, WALL);
      if (current_task_status == TASK_SUCCESS)
	{
	  points += get_and_reset_task_points ();
	  reset_task ();
	  tactic_state = SOLAR_R;
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
	  set_task_case (4);
	  reset_movement ();
	}
      break;

    case 4:
      sensors_case_timer = SENSORS_HIGH;
      set_translation_speed_limit (1.0);
      move_to_xy (1700, 220, WALL);
      if (movement_finished () && timer_delay_nonblocking (20))
	{
	  current_task_retries = 0;
	  tactic_state = DROP_Y;
	}
      if (interrupted)
	{
	  current_task_retries++;
	  reset_movement ();
	}
      break;

    case DROP_Y:
      current_task_status = task_dropoff_y_2 (YELLOW, MECHANISM);
      if (current_task_status == TASK_SUCCESS)
	{
	  points += get_and_reset_task_points ();
	  reset_task ();

	  tactic_state = HOME;
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
	  set_task_case (5);
	  reset_movement ();
	}
      break;

    case SOLAR_R:
      current_task_status = task_solar (YELLOW, RESERVED, 0.5, WALL);
      if (current_task_status == TASK_SUCCESS)
	{
	  reset_task ();
	  points += get_and_reset_task_points ();
	  tactic_state = DROP_Y;
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
	  set_task_case (4);
	  reset_movement ();
	}
      break;

    case PLANT_4:
      current_task_status = task_pickup_plants (plants[3], 1);

      if (current_task_status == TASK_SUCCESS)
	{
	  reset_task ();
	  tactic_state = SOLAR_C;
	  //	  pop_plant ();
	  set_translation_speed_limit (1.0);
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

    case HOME:
      current_task_status = task_go_home (homes[home_counter], home_side);
      if (current_task_status == TASK_SUCCESS)
	{
	  points += get_and_reset_task_points ();
	  mechanism_down ();
	  mechanism_down ();
	  mechanism_down ();
	  mechanism_down ();
	  tactic_state = RETURN;
	  current_task_retries = 0;
	}
      if (current_task_status == TASK_FAILED_1)
	{
	  current_task_retries++;
	  reset_movement ();
	  reset_task ();
	}
      break;

    case RETURN:
      tactic_finished = true;
      break;

    }
  return tactic_finished;
}

bool
yellow_matija ()
{
  switch (tactic_state)
    {

    case 0:
      if (!tactic_state_init)
	{
	  tactic_state_init = true;
	  plants[0] = plant_central1;
	  plants[1] = plant_yellow1;
	  plants[2] = plant_central2;
	  plants[3] = plant_blue2;
//	  plants[4] = plant_central2;
//	  plants[5] = plant_blue1;

	  homes[0] = home_yellow2;
	  homes[1] = home_yellow3_close;
	  home_side = WALL;

	  tactic_finished = false;
	}
      tactic_state = PLANT_1;
      tactic_state_init = false;
      break;

    case PLANT_1:
      current_task_status = task_pickup_plants (plants[0], 1);

      if (current_task_status == TASK_SUCCESS)
	{
	  reset_task ();
	  tactic_state = 1;

	  set_translation_speed_limit (1.0);
	  current_task_retries = 0;
	}
      else if (current_task_status == TASK_FAILED_1)
	{
	  reset_movement ();
	  current_task_retries++;

	  reset_task ();

	}
      break;

    case 1:
      sensors_case_timer = SENSORS_HIGH;
      set_translation_speed_limit (1.0);
      move_to_xy (home_yellow1.x, 2000 - 250, WALL);
      if (movement_finished () && timer_delay_nonblocking (20))
	{
	  current_task_retries = 0;
	  tactic_state = POT;
	}
      if (interrupted)
	{
	  current_task_retries++;
	  reset_movement ();
	}
      break;

    case POT:
      current_task_status = task_pot_reserved (YELLOW);

      if (current_task_status == TASK_SUCCESS)
	{
	  points += get_and_reset_task_points ();
	  current_task_retries = 0;
	  reset_task ();
	  tactic_state = PLANT_2;
	  current_task_retries = 0;
	}
      else if (current_task_status == TASK_FAILED_1)
	{
	  reset_movement ();
	  reset_task ();
	  current_task_retries++;

	}
      else if (current_task_status == TASK_FAILED_2)
	{
	  reset_movement ();
	  set_task_case (4);
	}
      break;

    case PLANT_2:
      current_task_status = task_pickup_plants (plants[1], 1);

      if (current_task_status == TASK_SUCCESS)
	{
	  reset_task ();
	  tactic_state = DROP_X_C;

	  set_translation_speed_limit (1.0);
	  current_task_retries = 0;
	}
      else if (current_task_status == TASK_FAILED_1)
	{
	  reset_movement ();
	  current_task_retries++;
	  reset_task ();

	}
      break;

    case DROP_X_C:
      current_task_status = task_dropoff_x (YELLOW, CLOSE);
      if (current_task_status == TASK_SUCCESS)
	{
	  points += get_and_reset_task_points ();
	  reset_task ();
	  tactic_state = 2;
	  current_task_retries = 0;
	}
      else if (current_task_status == TASK_FAILED_1)	// na putu do plantera
	{
	  reset_movement ();
	  current_task_retries++;
	  reset_task ();
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
	  set_task_case (7);
	  reset_movement ();
	}
      break;

    case 2:
      sensors_case_timer = SENSORS_HIGH;
      set_translation_speed_limit (1.0);
      move_to_xy (2500, 900, WALL);
      if (movement_finished () && timer_delay_nonblocking (20))
	{
	  current_task_retries = 0;
	  tactic_state = 3;
	}
      if (interrupted)
	{
	  current_task_retries++;
	  reset_movement ();
	}
      break;

    case 3:
      sensors_case_timer = SENSORS_MECHANISM;
      set_translation_speed_limit (0.5);
      move_to_xy (2000, 700, MECHANISM);
      if (movement_finished () && timer_delay_nonblocking (20))
	{
	  set_translation_speed_limit (1.0);
	  current_task_retries = 0;
	  tactic_state = PLANT_3;
	}
      if (interrupted)
	{
	  current_task_retries++;
	  reset_movement ();
	}
      break;

    case PLANT_3:
      current_task_status = task_pickup_plants (plants[2], 1);

      if (current_task_status == TASK_SUCCESS)
	{
	  reset_task ();
	  tactic_state = SOLAR_C;

	  set_translation_speed_limit (1.0);
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

    case SOLAR_C:
      current_task_status = task_solar (YELLOW, CENTRAL, 1, WALL);
      if (current_task_status == TASK_SUCCESS)
	{
	  points += get_and_reset_task_points ();
	  reset_task ();
	  tactic_state = 4;
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
	  set_task_case (4);
	  reset_movement ();
	}
      break;

    case 4:
      sensors_case_timer = SENSORS_HIGH;
      set_translation_speed_limit (1.0);
      move_to_xy (1700, 220, WALL);
      if (movement_finished () && timer_delay_nonblocking (20))
	{
	  current_task_retries = 0;
	  tactic_state = DROP_Y;
	}
      if (interrupted)
	{
	  current_task_retries++;
	  reset_movement ();
	}
      break;

    case DROP_Y:
//      current_task_status = task_dropoff_y_2 (YELLOW, );
      if (current_task_status == TASK_SUCCESS)
	{
	  points += get_and_reset_task_points ();
	  reset_task ();

	  tactic_state = SOLAR_R;
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
	  set_task_case (5);
	  reset_movement ();
	}
      break;

    case SOLAR_R:
      current_task_status = task_solar (YELLOW, RESERVED, 1.0, WALL);
      if (current_task_status == TASK_SUCCESS)
	{
	  reset_task ();
	  points += get_and_reset_task_points ();
	  tactic_state = HOME;
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
	  set_task_case (4);
	  reset_movement ();
	}
      break;

    case HOME:
      current_task_status = task_go_home (homes[home_counter], home_side);
      if (current_task_status == TASK_SUCCESS)
	{
	  points += get_and_reset_task_points ();
	  mechanism_down ();
	  mechanism_down ();
	  mechanism_down ();
	  mechanism_down ();
	  tactic_state = RETURN;
	  current_task_retries = 0;
	}
      if (current_task_status == TASK_FAILED_1)
	{
	  current_task_retries++;
	  reset_movement ();
	  reset_task ();
	}
      break;

    case RETURN:
      tactic_finished = true;
      break;

    }
  return tactic_finished;
}

bool
yellow_NSD ()
{
  switch (tactic_state)
    {
    case 0:
      if (!tactic_state_init)
	{
	  tactic_state_init = true;
	  plants[0] = plant_yellow2;
	  plants[1] = plant_central2;
	  plants[2] = plant_yellow1;
	  plants[3] = plant_central1;
//	  plants[4] = plant_central2;
//	  plants[5] = plant_blue1;

//	  alt_plants[0] = plant_yellow2;
	  alt_plants[1] = plant_yellow1;
	  alt_plants[2] = plant_central1;
	  alt_plants[3] = plant_blue1;

	  homes[0] = home_yellow1;
	  homes[1] = home_yellow3_close;
	  home_side = WALL;

	  tactic_finished = false;
	}
      tactic_state = PLANT_1;
      tactic_state_init = false;
      break;

    case PLANT_1:
      current_task_status = task_pickup_plants (plants[0], 1);

      if (current_task_status == TASK_SUCCESS)
	{
	  reset_task ();
	  tactic_state = POT_5;
	  set_translation_speed_limit (1.0);
	  current_task_retries = 0;
	}
      else if (current_task_status == TASK_FAILED_1)
	{
	  reset_movement ();
	  current_task_retries++;
	  reset_task ();

	}
      break;

    case POT:
      current_task_status = task_pot_reserved (YELLOW);

      if (current_task_status == TASK_SUCCESS)
	{
	  points += get_and_reset_task_points ();
	  current_task_retries = 0;
	  reset_task ();
	  tactic_state = PLANT_3;
	  current_task_retries = 0;
	}
      else if (current_task_status == TASK_FAILED_1)
	{
	  reset_movement ();
	  reset_task ();
	  current_task_retries++;

	}
      else if (current_task_status == TASK_FAILED_2)
	{
	  reset_movement ();
	  set_task_case (4);
	}
      break;

    case POT_5:
      current_task_status = task_pot_solar (YELLOW);

      if (current_task_status == TASK_SUCCESS)
	{
	  points += get_and_reset_task_points ();
	  current_task_retries = 0;
	  reset_task ();
	  tactic_state = PLANT_2;
	  current_task_retries = 0;
	}
      else if (current_task_status == TASK_FAILED_1)
	{
	  reset_movement ();
	  reset_task ();
	  current_task_retries++;

	}
      else if (current_task_status == TASK_FAILED_2)
	{
	  reset_movement ();
	  set_task_case (4);
	}
      break;

    case PLANT_2:
      current_task_status = task_pickup_plants (plants[1], 1);

      if (current_task_status == TASK_SUCCESS)
	{
	  reset_task ();
	  tactic_state = SOLAR_C;

	  set_translation_speed_limit (1.0);
	  current_task_retries = 0;
	}
      else if (current_task_status == TASK_FAILED_1)
	{
	  reset_movement ();
	  current_task_retries++;
	  reset_task ();

	}
      break;

    case DROP_X_C:
      current_task_status = task_dropoff_x (YELLOW, CLOSE);
      if (current_task_status == TASK_SUCCESS)
	{
	  points += get_and_reset_task_points ();
	  reset_task ();
	  tactic_state = HOME;
	  current_task_retries = 0;
	}
      else if (current_task_status == TASK_FAILED_1)	// na putu do plantera
	{
	  reset_movement ();
	  current_task_retries++;

	  reset_task ();

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

    case PLANT_4:
      current_task_status = task_pickup_plants (plants[3], 1);

      if (current_task_status == TASK_SUCCESS)
	{
	  reset_task ();
	  tactic_state = DROP_X_C;
	  //	  pop_plant ();
	  set_translation_speed_limit (1.0);
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

    case 1:
      sensors_case_timer = SENSORS_HIGH;
      set_translation_speed_limit (0.5);
      move_to_xy (3000 - 250, 220, WALL);
      if (movement_finished () && timer_delay_nonblocking (20))
	{
	  points += 2 * 5;
	  current_task_retries = 0;
	  tactic_state = SOLAR_R;
	  set_translation_speed_limit (1.0);

	}
      if (interrupted)
	{
	  current_task_retries++;
	  reset_movement ();
	}
      break;

    case PLANT_3:
      current_task_status = task_pickup_plants (plants[2], 1);

      if (current_task_status == TASK_SUCCESS)
	{
	  reset_task ();
	  tactic_state = DROP_Y;

	  set_translation_speed_limit (1.0);
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

    case SOLAR_C:
      current_task_status = task_solar (YELLOW, CENTRAL, 1, WALL);
      if (current_task_status == TASK_SUCCESS)
	{
	  points += get_and_reset_task_points ();
	  reset_task ();
	  tactic_state = 1;
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
	  set_task_case (4);
	  reset_movement ();
	}
      break;

    case DROP_Y:
      current_task_status = task_dropoff_y_2 (YELLOW, MECHANISM);
      if (current_task_status == TASK_SUCCESS)
	{
	  points += get_and_reset_task_points ();
	  reset_task ();
	  tactic_state = PLANT_4;
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
	  set_task_case (5);
	  reset_movement ();
	}
      break;

    case SOLAR_R:
      current_task_status = task_solar (YELLOW, RESERVED, 1.0, MECHANISM);
      if (current_task_status == TASK_SUCCESS)
	{
	  points += get_and_reset_task_points ();
	  reset_task ();

	  tactic_state = POT;
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
	  set_task_case (4);
	  reset_movement ();
	}
      break;

    case HOME:
      current_task_status = task_go_home (homes[home_counter], home_side);
      if (current_task_status == TASK_SUCCESS)
	{
	  points += get_and_reset_task_points ();
	  mechanism_down ();
	  mechanism_down ();
	  mechanism_down ();
	  mechanism_down ();
	  tactic_state = RETURN;
	  current_task_retries = 0;
	}
      if (current_task_status == TASK_FAILED_1)
	{
	  current_task_retries++;
	  reset_movement ();
	  reset_task ();
	  if (home_counter == 0 && distance > 200)
	    {
	      home_counter++;
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
	  plants[3] = plant_blue2;
//	  plants[4] = plant_central2;
//	  plants[5] = plant_blue1;

	  alt_plants[0] = plant_yellow2;
	  alt_plants[2] = plant_central2;
	  alt_plants[3] = plant_blue1;

	  homes[0] = home_yellow1;
	  homes[1] = home_yellow3_close;
	  home_side = WALL;

	  tactic_finished = false;
	  points = 0;
	}
      tactic_state = PLANT_1;
      tactic_state_init = false;
      break;

    case PLANT_1:
      current_task_status = task_pickup_plants (plants[0], 1);

      if (current_task_status == TASK_SUCCESS)
	{
	  reset_task ();
	  if (alt == 0)
	    tactic_state = 1;
	  else if (alt == 1)
	    tactic_state = POT;
//	  pop_plant ();
	  set_translation_speed_limit (1.0);
	  current_task_retries = 0;
	}
      else if (current_task_status == TASK_FAILED_1)
	{
	  /*
	   * reset_movement ();
	   * reset_task ();
	   */
	  reset_movement ();
	  current_task_retries++;
	  current_task_retries %= 2;
	  alt = current_task_retries;
	  reset_task ();
	  swap_plant_alt (0);
	  swap_plant_alt (2);
	  tactic_state = 1;
	}
      break;

    case 1:
      sensors_case_timer = SENSORS_HIGH;
      set_translation_speed_limit (1.0);
      move_to_xy (home_yellow2.x, 250, WALL);
      if (movement_finished () && timer_delay_nonblocking (20))
	{
	  current_task_retries = 0;
	  if (alt == 0)
	    tactic_state = POT;
	  else if (alt == 1)
	    tactic_state = PLANT_1;
	}
      if (interrupted)
	{
	  current_task_retries++;
	  reset_movement ();
	}
      break;

    case 2:
      sensors_case_timer = SENSORS_HIGH;
      set_translation_speed_limit (1.0);
      turn_to_pos (3000 - 700, robot_position.y_mm, WALL);
      if (movement_finished () && timer_delay_nonblocking (20))
	{
	  tactic_state = 3;
	}
      break;

    case 3:
      sensors_case_timer = SENSORS_HIGH;
      set_translation_speed_limit (1.0);
      move_to_xy (3000 - 700, robot_position.y_mm, WALL);
      if (movement_finished () && timer_delay_nonblocking (20))
	{
	  tactic_state = 4;
	}
      if (interrupted)
	{
	  current_task_retries++;
	  reset_movement ();
	}
      break;

    case 4:
      sensors_case_timer = SENSORS_MECHANISM;
      set_translation_speed_limit (1.0);
      turn_to_pos (3000 - 700, 2000 - 640, MECHANISM);
      if (movement_finished () && timer_delay_nonblocking (20))
	{
	  tactic_state = 5;
	}
      break;

    case 5:
      sensors_case_timer = SENSORS_MECHANISM;
      set_translation_speed_limit (1.0);
      move_to_xy (3000 - 700, 2000 - 640, MECHANISM);
      if (movement_finished () && timer_delay_nonblocking (20))
	{
	  tactic_state = POT;
	}
      if (interrupted)
	{
	  current_task_retries++;
	  reset_movement ();
	}
      break;

    case POT:
      current_task_status = task_pot_reserved (YELLOW);

      if (current_task_status == TASK_SUCCESS)
	{
	  points += get_and_reset_task_points ();
	  current_task_retries = 0;
	  reset_task ();
	  tactic_state = PLANT_2;
	  current_task_retries = 0;
	}
      else if (current_task_status == TASK_FAILED_1)
	{
	  reset_movement ();
	  reset_task ();
	  current_task_retries++;
	  if (current_task_retries == 1)
	    {
	      if (alt == 0)
		tactic_state = 2;
	      else if (alt == 1)
		tactic_state = 4;
	    }
	  else
	    tactic_state = POT;
	}
      else if (current_task_status == TASK_FAILED_2)
	{
	  reset_movement ();
	  set_task_case (4);
	}
      break;

    case PLANT_2:
      current_task_status = task_pickup_plants (plants[1], 1);

      if (current_task_status == TASK_SUCCESS)
	{
	  reset_task ();
	  tactic_state = DROP_X_C;
//	  pop_plant ();
	  set_translation_speed_limit (1.0);
	  current_task_retries = 0;
	}
      else if (current_task_status == TASK_FAILED_1)
	{
	  reset_movement ();
	  current_task_retries++;
	  reset_task ();
	  swap_first2_plants ();
	}
      break;

    case DROP_X_C:
      current_task_status = task_dropoff_x (YELLOW, CLOSE);
      if (current_task_status == TASK_SUCCESS)
	{
	  points += get_and_reset_task_points ();
	  reset_task ();
	  tactic_state = PLANT_3;
	  current_task_retries = 0;
	}
      else if (current_task_status == TASK_FAILED_1)	// na putu do plantera
	{
	  reset_movement ();
	  current_task_retries++;
	  alt = 2;
	  reset_task ();
	  if (alt == 2)
	    tactic_state = DROP_Y;
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
	  set_task_case (7);
	  reset_movement ();
	}
      break;

    case PLANT_3:
      current_task_status = task_pickup_plants (plants[2], 1);

      if (current_task_status == TASK_SUCCESS)
	{
	  reset_task ();
	  tactic_state = SOLAR_R;
//	  pop_plant ();
	  set_translation_speed_limit (1.0);
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

    case SOLAR_R:
      current_task_status = task_solar (YELLOW, RESERVED, 1.0, WALL);
      if (current_task_status == TASK_SUCCESS)
	{
	  points += get_and_reset_task_points ();
	  reset_task ();
	  if (alt == 0 || alt == 1)
	    tactic_state = DROP_Y;
	  else if (alt == 2)
	    tactic_state = DROP_X_C_ALT;
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
	  set_task_case (4);
	  reset_movement ();
	}
      break;

    case DROP_X_C_ALT:
      current_task_status = task_dropoff_x_alt (YELLOW, CLOSE);
      if (current_task_status == TASK_SUCCESS)
	{
	  points += get_and_reset_task_points ();
	  reset_task ();
	  tactic_state = SOLAR_C;
	  current_task_retries = 0;
	}
      else if (current_task_status == TASK_FAILED_1)	// na putu do plantera
	{
	  reset_task ();
	  reset_movement ();
	  current_task_retries++;
//	  tactic_state = 20;
	}
      else if (current_task_status == TASK_FAILED_2)	// tokom izvlacenja
	{
	  set_task_case (6);
	  reset_movement ();
	  current_task_retries++;
	}
      break;

    case SOLAR_C:
      current_task_status = task_solar (YELLOW, CENTRAL, 1, WALL);
      if (current_task_status == TASK_SUCCESS)
	{
	  points += get_and_reset_task_points ();
	  reset_task ();
	  tactic_state = PLANT_4;
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
	  set_task_case (4);
	  reset_movement ();
	}
      break;

    case DROP_Y:
      current_task_status = task_dropoff_y_2 (YELLOW, MECHANISM);
      if (current_task_status == TASK_SUCCESS)
	{
	  points += get_and_reset_task_points ();
	  reset_task ();
	  if (alt == 0 || alt == 1)
	    tactic_state = SOLAR_C;
	  else if (alt == 2)
	    tactic_state = POT_2;
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
	  set_task_case (5);
	  reset_movement ();
	}
      break;

    case POT_2:
      current_task_status = task_push_pots (YELLOW);

      if (current_task_status == TASK_SUCCESS)
//	  || current_task_status == TASK_FAILED_2)
	{
	  points += get_and_reset_task_points ();
	  current_task_retries = 0;
	  reset_task ();
	  tactic_state = PLANT_3;
	  current_task_retries = 0;
	}
      else if (current_task_status == TASK_FAILED_1)
	{
	  reset_movement ();
	  reset_task ();
	  current_task_retries++;
	}
      break;

    case PLANT_4:
      current_task_status = task_pickup_plants (plants[3], 1);

      if (current_task_status == TASK_SUCCESS)
	{
	  reset_task ();
	  tactic_state = DROP_X_F;
//	  pop_plant ();
	  set_translation_speed_limit (1.0);
	  current_task_retries = 0;
	  homes[0] = home_yellow3_close;
	  homes[1] = home_yellow1;
	}
      else if (current_task_status == TASK_FAILED_1)
	{
	  reset_movement ();
	  current_task_retries++;
	  reset_task ();
	  // TODO: ovde stavi alt biljku
	}
      break;

    case DROP_X_F:
      current_task_status = task_dropoff_x (YELLOW, FAR);
      if (current_task_status == TASK_SUCCESS)
	{
	  points += get_and_reset_task_points ();
	  reset_task ();
	  tactic_state = HOME;
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
	  set_task_case (7);
	  reset_movement ();
	}
      break;

    case HOME:
      current_task_status = task_go_home (homes[home_counter], home_side);
      if (current_task_status == TASK_SUCCESS)
	{
	  points += get_and_reset_task_points ();
	  mechanism_down ();
	  mechanism_down ();
	  mechanism_down ();
	  mechanism_down ();
	  tactic_state = RETURN;
	  current_task_retries = 0;
	}
      if (current_task_status == TASK_FAILED_1)
	{
	  current_task_retries++;
	  reset_movement ();
	  reset_task ();
	  if (home_counter == 0 && distance > 200)
	    {
	      home_counter++;
//	      home_side++;
//	      home_side %= 2;
	    }
	}
      break;

    case RETURN:
      tactic_finished = true;
      break;

    }
  return tactic_finished;
}

bool
yellow_risky ()
{
  switch (tactic_state)
    {
    case 0:
      if (!tactic_state_init)
	{
	  tactic_state_init = true;
	  plants[0] = plant_blue2;
	  plants[1] = plant_yellow1;
	  plants[2] = plant_central1;
	  plants[3] = plant_yellow2;
	  plants[4] = plant_central2;
	  //plants[5] = plant_central1;

	  alt_plants[0] = plant_blue1;

	  homes[0] = home_yellow2;
	  homes[1] = home_yellow1;
	  home_side = WALL;

	  tactic_finished = false;
	}

      tactic_state = PLANT_1;
      tactic_state_init = false;
      break;

    case PLANT_1:
      current_task_status = task_pickup_plants (plants[0], 1);

      if (current_task_status == TASK_SUCCESS)
	{
	  reset_task ();
	  tactic_state = DROP_X_F;
	  set_translation_speed_limit (1.0);
	  current_task_retries = 0;
	}
      else if (current_task_status == TASK_FAILED_1)
	{
	  reset_movement ();
	  current_task_retries++;
	  reset_task ();
	  swap_plant_alt (0);
//	   swap_plant_alt (2);
//	  tactic_state = 1;
	}
      break;

    case DROP_X_F:
      current_task_status = task_dropoff_x (YELLOW, FAR);
      if (current_task_status == TASK_SUCCESS)
	{
	  points += get_and_reset_task_points ();
	  reset_task ();
	  tactic_state = PLANT_2;
	  current_task_retries = 0;
	}
      else if (current_task_status == TASK_FAILED_1)	// na putu do plantera
	{
	  reset_movement ();
	  current_task_retries++;
	  reset_task ();
//	 //	  tactic_state = 20;
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
	  set_task_case (7);
	  reset_movement ();
	}
      break;

    case PLANT_2:
      current_task_status = task_pickup_plants (plants[1], 1);

      if (current_task_status == TASK_SUCCESS)
	{
	  reset_task ();
	  tactic_state = DROP_Y;
	  //	  pop_plant ();
	  set_translation_speed_limit (1.0);
	  current_task_retries = 0;
	}
      else if (current_task_status == TASK_FAILED_1)
	{
	  reset_movement ();
	  current_task_retries++;
	  reset_task ();
//	 	  swap_first2_plants ();
	}
      break;

    case POT_5:
      current_task_status = task_pot_solar (YELLOW);

      if (current_task_status == TASK_SUCCESS)
	{
	  points += get_and_reset_task_points ();
	  current_task_retries = 0;
	  reset_task ();
	  tactic_state = SOLAR_C;
	  current_task_retries = 0;
	}
      else if (current_task_status == TASK_FAILED_1)
	{
	  reset_movement ();
	  reset_task ();
	  current_task_retries++;

	}
      else if (current_task_status == TASK_FAILED_2)
	{
	  reset_movement ();
	  set_task_case (4);
	}
      break;

    case SOLAR_C:
      current_task_status = task_solar (YELLOW, CENTRAL, 1, WALL);
      if (current_task_status == TASK_SUCCESS)
	{
	  points += get_and_reset_task_points ();
	  reset_task ();
	  tactic_state = PLANT_5;
	  current_task_retries = 0;
	}
      else if (current_task_status == TASK_FAILED_1)	// na putu do solara
	{
	  current_task_retries++;
	  reset_task ();
	  reset_movement ();
//		  //	  tactic_state = 30;
	}
      else if (current_task_status == TASK_FAILED_2)	// pri okretanju solara
	{
	  current_task_retries++;
	  set_task_case (4);
	  reset_movement ();
	}
      break;

    case PLANT_5:
      current_task_status = task_pickup_plants (plants[4], 1);

      if (current_task_status == TASK_SUCCESS)
	{
	  reset_task ();
	  tactic_state = SOLAR_R;
	  //	  pop_plant ();
	  set_translation_speed_limit (1.0);
	  current_task_retries = 0;
	}
      else if (current_task_status == TASK_FAILED_1)
	{
	  reset_movement ();
	  current_task_retries++;
	  reset_task ();
//	 	  swap_first2_plants ();
	}
      break;

//    case 1:
//      sensors_case_timer = SENSORS_HIGH;
//      set_translation_speed_limit (1.0);
//      move_to_xy (2000, 500, WALL);
//      if (movement_finished () && timer_delay_nonblocking (20))
//	{
//	  current_task_retries = 0;
//	  tactic_state = SOLAR_R;
//	}
//      if (interrupted)
//	{
//	  current_task_retries++;
//	  reset_movement ();
//	}
//      break;

    case SOLAR_R:
      current_task_status = task_solar (YELLOW, RESERVED, 1.0, WALL);
      if (current_task_status == TASK_SUCCESS)
	{
	  points += get_and_reset_task_points ();
	  reset_task ();
	  tactic_state = HOME;
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
	  set_task_case (4);
	  reset_movement ();
	}
      break;

    case PLANT_3:
      current_task_status = task_pickup_plants (plants[2], 1);

      if (current_task_status == TASK_SUCCESS)
	{
	  reset_task ();
	  tactic_state = POT;
	  //	  pop_plant ();
	  set_translation_speed_limit (1.0);
	  current_task_retries = 0;
	}
      else if (current_task_status == TASK_FAILED_1)
	{
	  reset_movement ();
	  current_task_retries++;
	  reset_task ();

	}
      break;

    case POT:
      current_task_status = task_pot_reserved (YELLOW);

      if (current_task_status == TASK_SUCCESS)
	{
	  points += get_and_reset_task_points ();
	  current_task_retries = 0;
	  reset_task ();
	  tactic_state = PLANT_4;
	  current_task_retries = 0;
	}
      else if (current_task_status == TASK_FAILED_1)
	{
	  reset_movement ();
	  reset_task ();
	  current_task_retries++;
	  if (current_task_retries == 1)
	    {
	      if (alt == 0)
		tactic_state = 2;
	      else if (alt == 1)
		tactic_state = 4;
	    }
	  else
	    tactic_state = POT;
	}
      else if (current_task_status == TASK_FAILED_2)
	{
	  reset_movement ();
	  set_task_case (4);
	}
      break;

    case DROP_X_C:
      current_task_status = task_dropoff_x (YELLOW, CLOSE);
      if (current_task_status == TASK_SUCCESS)
	{
	  points += get_and_reset_task_points ();
	  reset_task ();
	  tactic_state = SOLAR_C;
	  current_task_retries = 0;
	}
      else if (current_task_status == TASK_FAILED_1)	// na putu do plantera
	{
	  reset_movement ();
	  current_task_retries++;
	  reset_task ();
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
	  set_task_case (7);
	  reset_movement ();
	}
      break;

    case PLANT_4:
      current_task_status = task_pickup_plants (plants[3], 1);

      if (current_task_status == TASK_SUCCESS)
	{
	  reset_task ();
	  tactic_state = DROP_X_C;
	  //	  pop_plant ();
	  set_translation_speed_limit (1.0);
	  current_task_retries = 0;
	}
      else if (current_task_status == TASK_FAILED_1)
	{
	  reset_movement ();
	  current_task_retries++;
	  reset_task ();
	}
      break;

    case DROP_Y:
      current_task_status = task_dropoff_y_2 (YELLOW, WALL);

      if (current_task_status == TASK_SUCCESS)
	{
	  points += get_and_reset_task_points ();
	  reset_task ();
	  tactic_state = PLANT_3;
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
	  set_task_case (5);
	  reset_movement ();
	}
      break;
    case HOME:
      current_task_status = task_go_home (homes[home_counter], home_side);
      if (current_task_status == TASK_SUCCESS)
	{
	  points += get_and_reset_task_points ();
	  tactic_state = 1;
	  current_task_retries = 0;
	}
      if (current_task_status == TASK_FAILED_1)
	{
	  current_task_retries++;
	  reset_movement ();
	  reset_task ();
	  if (home_counter == 0 && distance > 200)
	    {
	      home_counter++;
	      home_side++;
	      home_side %= 2;
	    }
	}
      break;
    case 1:
      sensors_case_timer = SENSORS_HIGH;
      set_translation_speed_limit (0.5);
      set_rotation_speed_limit (0.5);
      move_to_xy (3000 - 350, 350, MECHANISM);
      if (movement_finished () && timer_delay_nonblocking (20))
	{
	  current_task_retries = 0;
	  tactic_state = RETURN;
	  mechanism_down ();
	  mechanism_down ();
	  mechanism_down ();
	  mechanism_down ();
	  mechanism_open ();
	  mechanism_open ();
	  mechanism_open ();
	  mechanism_open ();
	}
      break;

    case RETURN:
      tactic_finished = true;
      break;
    }
  return tactic_finished;
}

bool
blue_4 ()
{
  switch (tactic_state)
    {
    case 0:
      if (!tactic_state_init)
	{
	  tactic_state_init = true;
	  plants[0] = plant_central2;
	  plants[1] = plant_blue1;
	  plants[2] = plant_blue2;
	  plants[3] = plant_yellow2;
//	  plants[4] = plant_central2;
//	  plants[5] = plant_blue1;

	  alt_plants[0] = plant_blue2;
	  alt_plants[2] = plant_central2;
	  alt_plants[3] = plant_yellow1;

	  homes[0] = home_blue1;
	  homes[1] = home_blue3_close;
	  home_side = WALL;

	  tactic_finished = false;
	  points = 0;
	}
      tactic_state = PLANT_1;
      tactic_state_init = false;
      break;

    case PLANT_1:
      current_task_status = task_pickup_plants (plants[0], 1);

      if (current_task_status == TASK_SUCCESS)
	{
	  reset_task ();
	  if (alt == 0)
	    tactic_state = 1;
	  else if (alt == 1)
	    tactic_state = POT;
//	  pop_plant ();
	  set_translation_speed_limit (1.0);
	  current_task_retries = 0;
	}
      else if (current_task_status == TASK_FAILED_1)
	{
	  /*
	   * reset_movement ();
	   * reset_task ();
	   */
	  reset_movement ();
	  current_task_retries++;
	  current_task_retries %= 2;
	  alt = current_task_retries;
	  reset_task ();
	  swap_plant_alt (0);
	  swap_plant_alt (2);
	  tactic_state = 1;
	}
      break;

    case 1:
      sensors_case_timer = SENSORS_HIGH;
      set_translation_speed_limit (1.0);
      move_to_xy (home_blue2.x, 250, WALL);
      if (movement_finished () && timer_delay_nonblocking (20))
	{
	  current_task_retries = 0;
	  if (alt == 0)
	    tactic_state = POT;
	  else if (alt == 1)
	    tactic_state = PLANT_1;
	}
      if (interrupted)
	{
	  current_task_retries++;
	  reset_movement ();
	}
      break;

    case 2:
      sensors_case_timer = SENSORS_HIGH;
      set_translation_speed_limit (1.0);
      turn_to_pos (700, robot_position.y_mm, WALL);
      if (movement_finished () && timer_delay_nonblocking (20))
	{
	  tactic_state = 3;
	}
      break;

    case 3:
      sensors_case_timer = SENSORS_HIGH;
      set_translation_speed_limit (1.0);
      move_to_xy (700, robot_position.y_mm, WALL);
      if (movement_finished () && timer_delay_nonblocking (20))
	{
	  tactic_state = 4;
	}
      if (interrupted)
	{
	  current_task_retries++;
	  reset_movement ();
	}
      break;

    case 4:
      sensors_case_timer = SENSORS_MECHANISM;
      set_translation_speed_limit (1.0);
      turn_to_pos (700, 2000 - 640, MECHANISM);
      if (movement_finished () && timer_delay_nonblocking (20))
	{
	  tactic_state = 5;
	}
      break;

    case 5:
      sensors_case_timer = SENSORS_MECHANISM;
      set_translation_speed_limit (1.0);
      move_to_xy (700, 2000 - 640, MECHANISM);
      if (movement_finished () && timer_delay_nonblocking (20))
	{
	  tactic_state = POT;
	}
      if (interrupted)
	{
	  current_task_retries++;
	  reset_movement ();
	}
      break;

    case POT:
      current_task_status = task_pot_reserved (BLUE);

      if (current_task_status == TASK_SUCCESS)
	{
	  points += get_and_reset_task_points ();
	  current_task_retries = 0;
	  reset_task ();
	  tactic_state = PLANT_2;
	  current_task_retries = 0;
	}
      else if (current_task_status == TASK_FAILED_1)
	{
	  reset_movement ();
	  reset_task ();
	  current_task_retries++;
	  if (current_task_retries == 1)
	    {
	      if (alt == 0)
		tactic_state = 2;
	      else if (alt == 1)
		tactic_state = 4;
	    }
	  else
	    tactic_state = POT;
	}
      else if (current_task_status == TASK_FAILED_2)
	{
	  reset_movement ();
	  set_task_case (4);
	}
      break;

    case PLANT_2:
      current_task_status = task_pickup_plants (plants[1], 1);

      if (current_task_status == TASK_SUCCESS)
	{
	  reset_task ();
	  tactic_state = DROP_X_C;
//	  pop_plant ();
	  set_translation_speed_limit (1.0);
	  current_task_retries = 0;
	}
      else if (current_task_status == TASK_FAILED_1)
	{
	  reset_movement ();
	  current_task_retries++;
	  reset_task ();
	  swap_first2_plants ();
	}
      break;

    case DROP_X_C:
      current_task_status = task_dropoff_x (BLUE, CLOSE);
      if (current_task_status == TASK_SUCCESS)
	{
	  points += get_and_reset_task_points ();
	  reset_task ();
	  tactic_state = PLANT_3;
	  current_task_retries = 0;
	}
      else if (current_task_status == TASK_FAILED_1)	// na putu do plantera
	{
	  reset_movement ();
	  current_task_retries++;
	  alt = 2;
	  reset_task ();
	  if (alt == 2)
	    tactic_state = DROP_Y;
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
	  set_task_case (7);
	  reset_movement ();
	}
      break;

    case PLANT_3:
      current_task_status = task_pickup_plants (plants[2], 1);

      if (current_task_status == TASK_SUCCESS)
	{
	  reset_task ();
	  tactic_state = SOLAR_R;
//	  pop_plant ();
	  set_translation_speed_limit (1.0);
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

    case SOLAR_R:
      current_task_status = task_solar (BLUE, RESERVED, 1.0, WALL);
      if (current_task_status == TASK_SUCCESS)
	{
	  points += get_and_reset_task_points ();
	  reset_task ();
	  if (alt == 0 || alt == 1)
	    tactic_state = DROP_Y;
	  else if (alt == 2)
	    tactic_state = DROP_X_C_ALT;
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
	  set_task_case (4);
	  reset_movement ();
	}
      break;

    case DROP_X_C_ALT:
      current_task_status = task_dropoff_x_alt (BLUE, CLOSE);
      if (current_task_status == TASK_SUCCESS)
	{
	  points += get_and_reset_task_points ();
	  reset_task ();
	  tactic_state = SOLAR_C;
	  current_task_retries = 0;
	}
      else if (current_task_status == TASK_FAILED_1)	// na putu do plantera
	{
	  reset_task ();
	  reset_movement ();
	  current_task_retries++;
//	  tactic_state = 20;
	}
      else if (current_task_status == TASK_FAILED_2)	// tokom izvlacenja
	{
	  set_task_case (6);
	  reset_movement ();
	  current_task_retries++;
	}
      break;

    case SOLAR_C:
      current_task_status = task_solar (BLUE, CENTRAL, 1, WALL);
      if (current_task_status == TASK_SUCCESS)
	{
	  points += get_and_reset_task_points ();
	  reset_task ();
	  tactic_state = PLANT_4;
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
	  set_task_case (4);
	  reset_movement ();
	}
      break;

    case DROP_Y:
      current_task_status = task_dropoff_y_2 (YELLOW, MECHANISM);

      if (current_task_status == TASK_SUCCESS)
	{
	  points += get_and_reset_task_points ();
	  reset_task ();
	  if (alt == 0 || alt == 1)
	    tactic_state = SOLAR_C;
	  else if (alt == 2)
	    tactic_state = POT_2;
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
	  set_task_case (5);
	  reset_movement ();
	}
      break;

    case POT_2:
      current_task_status = task_push_pots (BLUE);

      if (current_task_status == TASK_SUCCESS)
//	  || current_task_status == TASK_FAILED_2)
	{
	  points += get_and_reset_task_points ();
	  current_task_retries = 0;
	  reset_task ();
	  tactic_state = PLANT_3;
	  current_task_retries = 0;
	}
      else if (current_task_status == TASK_FAILED_1)
	{
	  reset_movement ();
	  reset_task ();
	  current_task_retries++;
	}
      break;

    case PLANT_4:
      current_task_status = task_pickup_plants (plants[3], 1);

      if (current_task_status == TASK_SUCCESS)
	{
	  reset_task ();
	  tactic_state = DROP_X_F;
//	  pop_plant ();
	  set_translation_speed_limit (1.0);
	  current_task_retries = 0;
	  homes[0] = home_blue3_close;
	  homes[1] = home_blue1;
	}
      else if (current_task_status == TASK_FAILED_1)
	{
	  reset_movement ();
	  current_task_retries++;
	  reset_task ();
	  // TODO: ovde stavi alt biljku
	}
      break;

    case DROP_X_F:
      current_task_status = task_dropoff_x (BLUE, FAR);
      if (current_task_status == TASK_SUCCESS)
	{
	  points += get_and_reset_task_points ();
	  reset_task ();
	  tactic_state = HOME;
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
	  set_task_case (7);
	  reset_movement ();
	}
      break;

    case HOME:
      current_task_status = task_go_home (homes[home_counter], home_side);
      if (current_task_status == TASK_SUCCESS)
	{
	  points += get_and_reset_task_points ();
	  mechanism_down ();
	  mechanism_down ();
	  mechanism_down ();
	  mechanism_down ();
	  tactic_state = RETURN;
	  current_task_retries = 0;
	}
      if (current_task_status == TASK_FAILED_1)
	{
	  current_task_retries++;
	  reset_movement ();
	  reset_task ();
	  if (home_counter == 0 && distance > 200)
	    {
	      home_counter++;
//	      home_side++;
//	      home_side %= 2;
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

uint8_t
get_points ()
{
  return points;
}

void
add_points (uint8_t number)
{
  points += number;
}
