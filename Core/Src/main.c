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

/* USER CODE BEGIN PV */
uint8_t state_main = POSITIONING;
bool state_main_init = false;
//uint8_t state_main = 5;

uint16_t duty_cycle_test = 100;

bool positioning_done = false;
uint8_t selected_tactic = 0;

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

extern volatile uint8_t sensors_case_timer;
extern volatile bool interrupted;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void
SystemClock_Config (void);

/* USER CODE BEGIN PFP */
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
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

//  timer_start_sys_time ();
//  Vd_sum = 0;
//  Vl_sum = 0;
//  Vd_inc = 0;
//  Vl_inc = 0;
//  pwm_start ();
//  regulation_on = true;
//  sensors_case_timer = SENSORS_HIGH;
//  timer_start_sys_time ();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
    {

      /* USER CODE END WHILE */

      /* USER CODE BEGIN 3 */
//      w_ref = ref_test;
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
	      if (blue_side_selected ())
		{
		  if (tactic_1_selected ())		// plava sigurna
		    {
		      set_starting_position (100 + 85, 2000 - 30 - 170, 180);
		      turn_to_pos (plant_blue2.x, plant_blue2.y, MECHANISM);
		      selected_tactic = 1;
		    }
		  else				// plava rizicna
		    {
		      set_starting_position (3000 - 50 - 85, 1000 - 225 + 50,
					     0);
		      turn_to_pos (plant_yellow2.x, plant_yellow2.y, MECHANISM);
		      selected_tactic = 2;
		    }
		}
	      else
		{
		  if (tactic_1_selected ())		// zuta sigurna
		    {
		      set_starting_position (3000 - 100 - 85, 2000 - 30 - 170,
					     0);
		      turn_to_pos (plant_yellow2.x, plant_yellow2.y, MECHANISM);
		      selected_tactic = 3;
		    }
		  else				// zuta rizicna
		    {
		      set_starting_position (3000 - 100 - 85, 30 + 170, 0);
		      turn_to_pos (plant_central2.x, plant_central2.y,
				   MECHANISM);
		      selected_tactic = 8;
		    }
		}
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
	      pwm_start ();
	      regulation_on = true;
	      set_rotation_speed_limit (1.0);
	      set_translation_speed_limit (1.0);
	      transition_factor = 1.0;
	    }
	  break;

//	case 1:
//	  if (safe_blue ())
//	    state_main = END;
//	  break;
//
//	case 2:
//	  if (risky_blue ())
//	    state_main = END;
//	  break;

	case 3:
	  if (safe_yellow ())
	    state_main = END;
	  break;

	case 4:
	  if (risky_yellow ())
	    state_main = END;
	  break;

	case 6:
//	  if (yellow_3 ())
	    state_main = END;
	  break;

	case 8:
	  if (yellow_4 ())
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
	  timer_stop_sys_time ();
	  stop_right_wheel ();
	  stop_left_wheel ();
	  pwm_duty_cycle_left (0);
	  pwm_duty_cycle_right (0);
	  regulation_on = false;
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

/* USER CODE BEGIN 4 */
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
