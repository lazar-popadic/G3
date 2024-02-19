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
#define START 0xff 	//255
#define END 0xfa	//250
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint8_t state_main = START;
bool state_main_init = false;
uint8_t state_debug = 0;

uint16_t sys_time_s = 0;
extern volatile uint32_t sys_time_half_ms;

uint16_t duty_cycle_test = 1000;
float v_max_test = V_REF_LIMIT_DEFAULT, w_max_test = W_REF_LIMIT_DEFAULT;
bool move_finished;

position pos_test =
  { 0, 0, 0 };
uint8_t init_rot_test = 0, final_rot_test = 0, tran_test = 1;

extern volatile position target_position, robot_position;
extern volatile bool regulation_on;
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
  adc_dma_init ();

  __enable_irq ();

//  timer_start_sys_time ();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
    {

      /* USER CODE END WHILE */

      /* USER CODE BEGIN 3 */
      sys_time_s = sys_time_half_ms * 0.0005;
      move_finished = movement_finished ();

//      if (timer_end ())
//      state_main = END;

      switch (state_main)
	{
	default:
	  break;

	case START:
	  if (io_cinc ())
	    {
	      timer_start_sys_time ();
	      state_main = 0;
	      pwm_start ();
	      set_starting_position (0, 0, 0);
	      regulation_on = true;
	    }
	  break;

	case 0:
//	  if (grabulja_test  ())
	  if (!state_main_init)
	    {
	      state_main_init = true;
	      state_debug = 0;
	    }
//	  set_rotation_speed_limit (w_max_test);
//	  set_translation_speed_limit (v_max_test);
	  move_to_angle (180, DEFAULT);
//	  move_full(450, 450, 0, 1, 0, 0);
	  if (movement_finished () && timer_delay_nonblocking (20))
	    {
//	      state_main++;
	      state_main = END;
	    }
	  break;

	case 1:
//	  move_full (pos_test.x_mm, pos_test.y_mm, pos_test.theta_rad, tran_test, init_rot_test, final_rot_test);
	  if (!state_main_init)
	    {
	      state_main_init = true;
	      state_debug = 0;
	    }

	  set_rotation_speed_limit (w_max_test);
	  set_translation_speed_limit (v_max_test);
	  move_to_angle (0, DEFAULT);
	  if (movement_finished () && timer_delay_nonblocking (20))
	    {
	      state_main++;
//	      state_main = END;
	    }
//	  if (timer_delay_nonblocking (20) && movement_finished ())
//	    state_main = END;
	  break;

	case END:
//	  regulation_on = false;
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
