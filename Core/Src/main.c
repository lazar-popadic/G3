/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 *
 *	  Created on: Oct 19, 2023
 *	      Author: lazar
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include <string.h>

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

/* USER CODE BEGIN PV */
uint8_t state_main = POSITIONING;
bool state_main_init = false;
//uint8_t state_main = 5;

uint16_t duty_cycle_test = 100;

bool positioning_done = false;
uint8_t selected_tactic = 0;

extern volatile uint16_t sys_time_s;
extern volatile target plant_blue1;
extern volatile target plant_blue2;
extern volatile target plant_central1;
extern volatile target plant_central2;
extern volatile target plant_yellow1;
extern volatile target plant_yellow2;
extern volatile float transition_factor;
extern int16_t Vd_sum;
extern int16_t Vl_sum;
extern int16_t Vd_inc;
extern int16_t Vl_inc;

extern volatile position target_position, robot_position;
extern volatile bool regulation_on;
int16_t calib1 = 512;
int16_t calib2 = 512;

bool s1 = false;
bool s2 = false;
bool s3 = false;

uint8_t tactic_chooser = 0;

extern volatile uint8_t sensors_case_timer;
extern volatile bool interrupted;

char tactic_string[10];
char y_risky[10] = "Y: Risk";
char y_matijaV2[10] = "Y: MV2";
char y_4[10] = "Y: 4";
char y_NSD[10] = "Y: NSD";
char y_NTS[10] = "Y: NTS";
char y_381[10] = "Y: +381";
char b_381[10] = "B: +381";
char b_risky[10] = "B: Risk";
char b_matijaV2[10] = "B: MV2";
char b_4[10] = "B: 4";
char b_NSD[10] = "B: NSD";
char b_NEO[10] = "B: NEO";
char y_NEO[10] = "Y: NEO";
char b_memra[10] = "B: memra";

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void
SystemClock_Config (void);
static void
MX_GPIO_Init (void);
static void
MX_I2C1_Init (void);
/* USER CODE BEGIN PFP */
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
char snum[5];
char snum_time[5];
/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int
main (void)
{

  /* USER CODE BEGIN 1 */
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */


  HAL_Init ();

  /* USER CODE BEGIN Init */
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config ();

  /* USER CODE BEGIN SysInit */
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init ();
  MX_I2C1_Init ();
  /* USER CODE BEGIN 2 */
  io_init ();
  timer_init ();
  encoder_init ();
  odometry_init ();
  uart_init ();
  pwm_init ();
  sensors_init ();
  h_bridge_init ();
  regulation_init ();

  __enable_irq ();

  regulation_on = false;
  HD44780_Init (2);
  HD44780_NoBacklight ();
  HD44780_Clear ();
  HD44780_SetCursor (0, 0);
  HD44780_PrintStr ("G3");
  HD44780_Backlight ();

//  timer_start_sys_time ();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
    {

      /* USER CODE END WHILE */

      /* USER CODE BEGIN 3 */
      switch (state_main)
	{
	default:
	  break;

	case POSITIONING:
	  if (!positioning_done)
	    {
	      set_rotation_speed_limit (0.2);
	      transition_factor = 2.5;
	      timer_start_sys_time ();
	      Vd_sum = 0;
	      Vl_sum = 0;
	      Vd_inc = 0;
	      Vl_inc = 0;
	      pwm_start ();
	      regulation_on = true;
	      positioning_done = true;
	      tactic_chooser = switch_1 () + switch_2 ();

	      if (blue_side_selected ())
		{
		  switch (tactic_chooser)
		    {
		    case 0:	// risky 13
		      set_starting_position (3000 - 100 - 85, 1225 - 30 - 170,
					     0);
		      turn_to_pos (plant_yellow1.x + 360, plant_yellow1.y, MECHANISM);
		      selected_tactic = 113;
		      strcpy (tactic_string, b_memra);
		      break;
		    case 1:	// nsd 11
		      set_starting_position (100 + 85, 32.5 + 170, 180);
		      turn_to_pos (plant_blue2.x, plant_blue2.y, MECHANISM);
		      selected_tactic = 11;
		      strcpy (tactic_string, b_NSD);
		      break;
		    case 2:	// neo
		      set_starting_position (100 + 85, 2000 - 32.5 - 170, 180);
		      turn_to_pos (plant_central1.x, plant_central1.y,
		      MECHANISM);
		      selected_tactic = 117;
		      strcpy (tactic_string, b_NEO);
		      break;
		    case 3:	// blue_4 	9
		      set_starting_position (100 + 85, 32.5 + 170, 180);
		      turn_to_pos (plant_central2.x, plant_central2.y,
		      MECHANISM);
		      selected_tactic = 119;
		      strcpy (tactic_string, b_381);
		      break;
		    }
		}
	      else
		{
		  switch (tactic_chooser)
		    {
		    case 0:	// yellow_risky
		      set_starting_position (100 + 85, 1225 - 40 - 170, 180);
		      turn_to_pos (plant_blue2.x, plant_blue2.y, MECHANISM);
		      selected_tactic = 14;
		      strcpy (tactic_string, y_risky);
		      break;
		    case 1:	// yellow_NTS
		      set_starting_position (3000 - 100 - 85, 2000 - 32.5 - 170, 0);
		      turn_to_pos (plant_central1.x, plant_central1.y, MECHANISM);
		      selected_tactic = 112;
		      strcpy (tactic_string, y_NTS);
		      break;
		    case 2:	// neo
		      set_starting_position (3000 - 100 - 85, 2000 - 32.5 - 170, 0);
		      turn_to_pos (plant_central1.x, plant_central1.y,
		      MECHANISM);
		      selected_tactic = 110;
		      strcpy (tactic_string, y_NEO);
		      break;
		    case 3:	// y_381
		      set_starting_position (3000 - 100 - 85, 32.5 + 170, 0);
		      turn_to_pos (plant_central2.x, plant_central2.y,
		      MECHANISM);
		      selected_tactic = 118;
		      strcpy (tactic_string, y_381);
		      break;
		    }
		}
	      /*	//homologacija
	       set_starting_position (3000 - 100 - 85, 32.5 + 170, 0);
	       turn_to_pos (3000 - 333, 215,
	       MECHANISM);
	       selected_tactic = 20;
	       */
	    }
	  mechanism_open ();
	  solar_in_l ();
	  solar_in_r ();
	  if (movement_finished () && timer_delay_nonblocking (20))
	    state_main = RESET_BEFORE_START;
	  break;
	case RESET_BEFORE_START:
	  pwm_duty_cycle_left (0);
	  pwm_duty_cycle_right (0);
	  set_rotation_speed_limit (1.0);
	  reset_and_stop_timer ();
	  regulation_on = false;
	  write_to_display (0, tactic_string);
	  state_main = START;
	  break;

	case START:
	  if (io_cinc ())
	    {
	      timer_start_sys_time ();
	      Vd_sum = 0;
	      Vl_sum = 0;
	      Vd_inc = 0;
	      Vl_inc = 0;
	      state_main = selected_tactic;
	      set_rotation_speed_limit (1.0);
	      set_translation_speed_limit (1.0);
	      pwm_start ();
	      regulation_on = true;
	      transition_factor = 1.0;
	    }
	  break;

	case 7:
	  if (blue_matijaV2 ())
	    state_main = END;
	  break;

	case 9:
	  if (blue_4 ())
	    state_main = END;
	  break;

	case 11:
	  if (blue_NSD ())
	    state_main = END;
	  break;

	case 117:
	  if (blue_neo ())
	    state_main = END;
	  break;

	case 13:
	  if (blue_risky ())
	    state_main = END;
	  break;

	case 113:
	  if (blue_memra ())
	    state_main = END;
	  break;

	case 8:
	  if (yellow_4 ())
	    state_main = END;
	  break;

	case 118:
	  if (yellow_381 ())
	    state_main = END;
	  break;

	case 10:
	  if (yellow_matijaV2 ())
	    state_main = END;
	  break;

	case 12:
	  if (yellow_NSD ())
	    state_main = END;
	  break;

	case 112:
	  if (yellow_nts ())
	    state_main = END;
	  break;

	case 14:
	  if (yellow_risky ())
	    state_main = END;
	  break;

	case 110:
	  if (yellow_neo())
	    state_main = END;
	  break;

	case 119:
	  if (blue_381())
	    state_main = END;
	  break;

	case 20:
	  if (homologation ())
	    state_main = END;
	  break;

//	case 10:
//	  move_on_direction_2 (200,MECHANISM);
////	  if (interrupted)
////	    reset_movement();
//	  if (movement_finished () && timer_delay_nonblocking (20))
//	    state_main = END;
//	  break;
//
	case END:
//	  write_to_display (get_points (), tactic_string);
	  timer_stop_sys_time ();
	  stop_right_wheel ();
	  stop_left_wheel ();
	  pwm_duty_cycle_left (0);
	  pwm_duty_cycle_right (0);
	  regulation_on = false;
	  write_to_display_time (get_points (), sys_time_s);
	  break;
	}
    } // while
  /* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void
SystemClock_Config (void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct =
    { 0 };
  RCC_ClkInitTypeDef RCC_ClkInitStruct =
    { 0 };

  /** Configure the main internal regulator output voltage
   */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

  /** Initializes the RCC Oscillators according to the specified parameters
   * in the RCC_OscInitTypeDef structure.
   */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 84;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig (&RCC_OscInitStruct) != HAL_OK)
    {
      Error_Handler ();
    }

  /** Initializes the CPU, AHB and APB buses clocks
   */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
      | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig (&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
    {
      Error_Handler ();
    }
}

/**
 * @brief I2C1 Initialization Function
 * @param None
 * @retval None
 */
static void
MX_I2C1_Init (void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init (&hi2c1) != HAL_OK)
    {
      Error_Handler ();
    }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
static void
MX_GPIO_Init (void)
{
  /* USER CODE BEGIN MX_GPIO_Init_1 */
  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /* USER CODE BEGIN MX_GPIO_Init_2 */
  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
void
write_to_display (uint8_t points, char tactic[10])
{
//  uint8_t a;
//  a = number;
//  itoa (a, snum, 10);

  itoa (points, snum, 10);
  HD44780_Init (2);
  HD44780_Backlight ();
  HD44780_Clear ();
  HD44780_SetCursor (0, 0);
  HD44780_PrintStr ("G3");
  HD44780_SetCursor (8, 0);
  HD44780_PrintStr (tactic);
  HD44780_SetCursor (0, 1);
  HD44780_PrintStr ("POINTS:");
  HD44780_SetCursor (13, 1);
  HD44780_PrintStr (snum);
}

void
write_to_display_time (uint8_t points, uint8_t time)
{
  itoa (points, snum, 10);
  itoa (time, snum_time, 10);
  HD44780_Init (2);
  HD44780_Backlight ();
  HD44780_Clear ();
  HD44780_SetCursor (0, 0);
  HD44780_PrintStr ("G3    TIME:");
  HD44780_SetCursor (13, 0);
  HD44780_PrintStr (snum_time);
  HD44780_SetCursor (0, 1);
  HD44780_PrintStr ("POINTS:");
  HD44780_SetCursor (13, 1);
  HD44780_PrintStr (snum);
}
/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void
Error_Handler (void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq ();
  while (1)
    {
    }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
