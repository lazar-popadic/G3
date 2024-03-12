/*
 * tactics.c
 *
 *  Created on: Nov 14, 2023
 *      Author: lazar
 */

#include "main.h"

#define AX_MAX_SPEED 528

uint8_t tactic_state = 0;
uint8_t previous_tactic_state = 0;
bool tactic_state_init = false;
bool tactic_finished;

int16_t dc = 0;
extern volatile uint8_t sensors_case_timer;
extern volatile bool sensors_state;
extern volatile position target_position;
extern volatile position robot_position;
volatile position previous_target =
  { 0, 0, 0 };

volatile uint8_t alternate_move = 0;
extern volatile uint8_t state_angle;

float V_max_perc = 1.0, w_max_perc = 1.0;
volatile int8_t current_task_status;

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
  { 600 + (325 / 2), 2000 - 80 };
volatile target planter_blue_x_close =
  { 80, 2000 - 450 - (325 / 2) };
volatile target planter_blue_x_far =
  { 3000 - 80, 450 + (325 / 2) };
volatile target planter_yellow_y =
  { 3000 - 600 - (325 / 2), 2000 - 80 };
volatile target planter_yellow_x_close =
  { 3000 - 80, 2000 - 450 - (325 / 2) };
volatile target planter_yellow_x_far =
  { 80, 450 + (325 / 2) };

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
// TODO: smisli za solare kako cemo

target homes[3];
target *homes_pointer = homes;
target plants[6];
target *plants_pointer = plants;

bool
test_tactic_yellow ()
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
	  plants[4] = plant_blue2;
	  plants[5] = plant_blue1;

	  homes[0] = home_yellow2;
	  homes[1] = home_yellow3;
	  homes[2] = home_yellow1;
	  tactic_finished = false;
	}
      tactic_state++;
      tactic_state_init = false;
      break;
    case 1:
      current_task_status = task_solar_from_start (YELLOW);
      if (current_task_status == 1)
	{
	  tactic_state++;
	  tactic_state_init = false;
	}
      break;
    case 2:
      current_task_status = task_pickup_plants (plants_pointer, 2);
      if (current_task_status == 1)
	{
	  tactic_state++;
	  tactic_state_init = false;
	}
      else if (current_task_status == -1)
	{
	  tactic_state = 5;
	  tactic_state_init = false;
	}
      break;
    case 3:
      current_task_status = task_central_solar_without (YELLOW);
      if (current_task_status == 1)
	{
	  tactic_state++;
	  tactic_state_init = false;
	}
      break;
    case 4:
      current_task_status = task_dropoff_plants_x_close (YELLOW);
      if (current_task_status == 1)
	{
	  tactic_state ++;
	  tactic_state_init = false;
	}
      break;
    case 5:
      current_task_status = task_go_home(homes_pointer);
      if (current_task_status == 1)
	{
	  tactic_state = RETURN;
	  tactic_state_init = false;
	}
      break;
    case RETURN:
      tactic_finished = true;
      break;

    }
  return tactic_finished;

}

bool
test_tactic_blue ()
{
  switch (tactic_state)
    {
    case 0:
      if (!tactic_state_init)
	{
	  tactic_state_init = true;
	  plants[0] = plant_blue1;
	  plants[1] = plant_blue2;
	  plants[2] = plant_central1;
	  plants[3] = plant_central2;
	  plants[4] = plant_yellow2;
	  plants[5] = plant_yellow1;

	  homes[0] = home_blue2;
	  homes[1] = home_blue3;
	  homes[2] = home_blue1;
	  tactic_finished = false;
	}
      tactic_state++;
      tactic_state_init = false;
      break;
    case 1:
      current_task_status = task_pickup_plants (plants_pointer, 2);
      if (current_task_status == 1)
	{
	  tactic_state++;
	  tactic_state_init = false;
	}
      else if (current_task_status == -1)
	{
	  tactic_state = 5;
	  tactic_state_init = false;
	}
      break;
//    case 3:
//      current_task_status = task_central_solar_without (BLUE);
//      if (current_task_status == 1)
//	{
//	  tactic_state++;
//	  tactic_state_init = false;
//	}
//      break;
    case 2:
      current_task_status = task_dropoff_plants_x_close (BLUE);
      if (current_task_status == 1)
	{
	  tactic_state ++;
	  tactic_state_init = false;
	}
      break;
    case 3:
      current_task_status = task_pickup_plants (plants_pointer, 2);
      if (current_task_status == 1)
	{
	  tactic_state++;
	  tactic_state_init = false;
	}
      else if (current_task_status == -1)
	{
	  tactic_state = 6;
	  tactic_state_init = false;
	}
      break;
    case 4:
          current_task_status = task_dropoff_plants_y (BLUE);
          if (current_task_status == 1)
    	{
    	  tactic_state ++;
    	  tactic_state_init = false;
    	}
          break;

    case 5:
      current_task_status = task_go_home(homes_pointer);
      if (current_task_status == 1)
	{
	  tactic_state = RETURN;
	  tactic_state_init = false;
	}
      break;
    case RETURN:
      tactic_finished = true;
      break;

    }
  return tactic_finished;

}

//case BRAKE:
//if (!tactic_state_init)
//  {
//    tactic_state_init = true;
//    previous_target = target_position;
//  }
//target_position = robot_position;
//if (timer_delay_nonblocking(4000))
//  {
//    tactic_state_init = false;
//    tactic_state = alternate_move;
//  }
//if (timer_delay_nonblocking(1000) && !sensors_state)
//  {
//    tactic_state_init = false;
//    tactic_state = previous_tactic_state;
//  }
//break;

//uint8_t
//alternative_move (position brake, uint8_t current_case, uint8_t next_target, uint8_t next_task, uint8_t wait_time, uint8_t number_of_retries)
//{
//  target_position = brake;
//  if (timer_delay_nonblocking(wait_time))
//    {
//      tactic_state = alternate_move;
//    }
//}
