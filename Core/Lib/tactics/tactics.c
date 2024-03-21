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
uint8_t home_counter = 0;
uint8_t home_side = 0;

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
  { 600 + (325 / 2), 2000 - 70 };
volatile target planter_blue_x_close =
  { 70, 2000 - 450 - (325 / 2) };
volatile target planter_blue_x_far =
  { 3000 - 70, 450 + (325 / 2) };
volatile target planter_yellow_y =
  { 3000 - 600 - (325 / 2), 2000 - 70 };
volatile target planter_yellow_x_close =
  { 3000 - 70, 2000 - 450 - (325 / 2) };
volatile target planter_yellow_x_far =
  { 70, 450 + (325 / 2) };

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

bool
safe_yellow ()
{
  switch (tactic_state)
    {
    case 0:
      if (!tactic_state_init)
	{
	  tactic_state_init = true;
	  plants[0] = plant_yellow1;
	  plants[1] = plant_yellow2;
	  plants[2] = plant_central1;
	  plants[3] = plant_central2;
	  plants[4] = plant_blue2;
	  plants[5] = plant_blue1;

//	  homes[0] = home_yellow1;
//	  homes[1] = home_yellow3;
//	  homes[2] = home_yellow2;
	  home_side = MECHANISM;

	  tactic_finished = false;
	}
      tactic_state++;
      tactic_state_init = false;
      break;
    case 1:
      current_task_status = task_pickup_plants (plants[0]);

      if (current_task_status == 1)
	{
	  reset_task ();
	  tactic_state++;
	  tactic_state_init = false;
	  pop_plant ();
	  set_translation_speed_limit (1.0);
	}
      else if (current_task_status == -1)
	{
	  reset_task ();
	  reset_movement ();
	  swap_first2_plants ();
	  tactic_state = 1;
	  tactic_state_init = false;
	}
      break;

    case 2:
      current_task_status = task_dropoff_x_close_2 (YELLOW);
      if (current_task_status == 1)
	{
	  reset_task ();
	  tactic_state++;
	  tactic_state_init = false;
	}
      break;
    case 3:
      current_task_status = task_pickup_plants (plants[0]);
      if (current_task_status == 1)
	{
	  reset_task ();
	  tactic_state++;
	  tactic_state_init = false;
	  pop_plant ();
	}
      else if (current_task_status == -1)
	{
	  // TODO: if (retries) ovo else tactic_state = go_home
	  reset_task ();
	  reset_movement ();
	  swap_first2_plants ();
	  tactic_state = 1;
	  tactic_state_init = false;
	}
      break;
//    case 4:
//      current_task_status = task_central_solar_short (YELLOW, MECHANISM);
//      if (current_task_status == 1)
//	{
//	  reset_task ();
//	  tactic_state++;
//	  tactic_state_init = false;
//	}
//      break;
    case 4:
      current_task_status = task_dropoff_y_2 (YELLOW);
      if (current_task_status == 1)
	{
	  reset_task ();
	  tactic_state++;
	  tactic_state_init = false;
	}
      break;
//    case 6:
//      current_task_status = reserved_solar (YELLOW);
//      if (current_task_status == 1)
//	{
//	  reset_task ();
//	  tactic_state++;
//	  tactic_state_init = false;
//	}
//      break;
    case 5:
      current_task_status = task_go_home (homes[home_counter], home_side,
      SENSORS_BACK);
      if (current_task_status == 1)
	{
	  tactic_state = RETURN;
	  tactic_state_init = false;
	}
      if (current_task_status == -1)
	{
	  if (home_counter == 0)
	    {
	      home_counter++;
	      reset_task();
	      reset_movement ();
	      home_side++;
	      home_side %= 2;
	    }
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

//	  homes[0] = home_yellow1;
//	  homes[1] = home_yellow3;
//	  homes[2] = home_yellow2;
	  home_side = MECHANISM;

	  tactic_finished = false;
	}
      tactic_state++;
      tactic_state_init = false;
      break;
    case 1:
      current_task_status = task_pickup_plants (plants[0]);

      if (current_task_status == 1)
	{
	  reset_task ();
	  tactic_state++;
	  tactic_state_init = false;
	  pop_plant ();
	  set_translation_speed_limit (1.0);
	}
      else if (current_task_status == -1)
	{
	  reset_task ();
	  reset_movement ();
	  swap_first2_plants ();
	  tactic_state = 1;
	  tactic_state_init = false;
	}
      break;

    case 2:
      current_task_status = task_dropoff_x_far_2 (YELLOW);
      if (current_task_status == 1)
	{
	  reset_task ();
	  tactic_state++;
	  tactic_state_init = false;
	}
      break;
    case 3:
      current_task_status = task_pickup_plants (plants[0]);
      if (current_task_status == 1)
	{
	  reset_task ();
	  tactic_state++;
	  tactic_state_init = false;
	  pop_plant ();
	}
      else if (current_task_status == -1)
	{
	  // TODO: if (retries) ovo else tactic_state = go_home
	  reset_task ();
	  reset_movement ();
	  swap_first2_plants ();
	  tactic_state = 1;
	  tactic_state_init = false;
	}
      break;
//    case 4:
//      current_task_status = task_central_solar_short (YELLOW, MECHANISM);
//      if (current_task_status == 1)
//	{
//	  reset_task ();
//	  tactic_state++;
//	  tactic_state_init = false;
//	}
//      break;
    case 4:
      current_task_status = task_dropoff_y_2 (YELLOW);
      if (current_task_status == 1)
	{
	  reset_task ();
	  tactic_state++;
	  tactic_state_init = false;
	}
      break;
    case 5:
      current_task_status = task_pickup_plants (plants[0]);
      if (current_task_status == 1)
	{
	  reset_task ();
	  tactic_state++;
	  tactic_state_init = false;
	  pop_plant ();
	}
      else if (current_task_status == -1)
	{
	  // TODO: if (retries) ovo else tactic_state = go_home
	  reset_task ();
	  reset_movement ();
	  swap_first2_plants ();
	  tactic_state = 1;
	  tactic_state_init = false;
	}
      break;
//    case 6:
//      current_task_status = reserved_solar (YELLOW);
//      if (current_task_status == 1)
//	{
//	  reset_task ();
//	  tactic_state++;
//	  tactic_state_init = false;
//	}
//      break;
//    case 7:
//      current_task_status = task_go_home (homes[home_counter], home_side,
//      SENSORS_BACK);
//      if (current_task_status == 1)
//	{
//	  tactic_state = RETURN;
//	  tactic_state_init = false;
//	}
//      if (current_task_status == -1)
//	{
//	  if (home_counter == 0)
//	    {
//	      home_counter++;
//	      reset_task();
//	      reset_movement ();
//	      home_side++;
//	      home_side %= 2;
//	    }
//	  tactic_state_init = false;
//	}
//      break;
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
