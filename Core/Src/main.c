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
I2C_HandleTypeDef hi2c1;

/* USER CODE BEGIN PV */
uint8_t state_main = START;
bool state_main_init = false;

uint16_t sys_time_s = 0;
extern volatile uint32_t sys_time_half_ms;

uint16_t duty_cycle_test = 100;

extern volatile target plant_blue1;
extern volatile target plant_blue2;
extern volatile target plant_central1;
extern volatile target plant_central2;
extern volatile target plant_yellow1;
extern volatile target plant_yellow2;

position pos_test =
  { 0, 0, 0 };
uint8_t init_rot_test = 0, final_rot_test = 0, tran_test = 1;

extern volatile position target_position, robot_position;
extern volatile bool regulation_on;
int16_t calib1 = 512;
int16_t calib2 = 512;
volatile float ref_test;
extern volatile float V_ref, w_ref;
unsigned char brojac = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
/* USER CODE BEGIN PFP */
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
char snum[5];
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_I2C1_Init();
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
  HD44780_Init(2);
  HD44780_NoBacklight();
 HD44780_Clear();
 HD44780_SetCursor(0, 0);
 HD44780_PrintStr("G3 Robotics");

//  timer_start_sys_time ();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
    {

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
      sys_time_s = sys_time_half_ms * 0.0005;
//      w_ref = ref_test;

//      if (timer_end ())
//      state_main = END;
//      ax_move(9, calib1, 500);
	  if (timer_delay_nonblocking(200))
		  brojac ++;
      ispis_displej(brojac);

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
//	    set_rotation_speed_limit(1.0);
//	    move_to_angle(-179);
//	    move_on_direction(1500, WALL);
	    }
	  break;
//
//
//	case 0:
//	  mechanism_open();
//	  if(timer_delay_nonblocking(2000))
//	    state_main = 10;
//	  break;
//
//	case 10:
//	  mechanism_half_up();
//	  if(timer_delay_nonblocking(2000))
//	    state_main ++;
//	  break;
//
//	case 11:
//	  mechanism_close();
//	  if(timer_delay_nonblocking(2000))
//	    state_main ++;
//	  break;
//
//	case 12:
//	  mechanism_up();
//	  if(timer_delay_nonblocking(2000))
//	    state_main ++;
//	  break;
//
//	case 13:
//	  mechanism_half_down();
//	  if(timer_delay_nonblocking(2000))
//	    state_main ++;
//	  break;
//
//	case 14:
//	  mechanism_open();
//	  if(timer_delay_nonblocking(2000))
//	    state_main ++;
//	  break;
//
//	case 15:
//	  mechanism_down();
//	  if(timer_delay_nonblocking(2000))
//	    state_main ++;
//	  break;

	case 0:
//	  set_translation_speed_limit(0.25);
//	  move_to_xy_offset (2000, 0, WALL, 100);
//	  set_rotation_speed_limit(1.0);
//	  move_to_angle(90);
//	  if (movement_finished () && timer_delay_nonblocking (20))
//	    state_main++;

	  mechanism_open ();
	  solar_in_l ();
	  solar_in_r ();
	  if (positioning_up_yellow (plant_yellow2))
	    state_main++;
	  break;

	case 1:
//	  set_translation_speed_limit(1.0);
//	  set_rotation_speed_limit(1.0);
//	  move_to_angle(75);
//	  move_to_xy_offset (2000, 0, WALL, -100);

//	  if (movement_finished () && timer_delay_nonblocking (20))
	  if (test_tactic_yellow ())
	    state_main = END;
	  break;
//
//	case END:
//	  timer_stop_sys_time ();
//	  stop_right_wheel ();
//	  stop_left_wheel ();
//	  pwm_duty_cycle_left (0);
//	  pwm_duty_cycle_right (0);
//	  hold_position ();
//	  break;
	}
    } // while
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

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
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
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
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOB_CLK_ENABLE();

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
void ispis_displej(uint8_t brojac) {
	uint8_t a;
	a = brojac;

	itoa(a, snum, 10);
	HD44780_Init(2);
	HD44780_Backlight();
	HD44780_Clear();
	HD44780_SetCursor(0, 0);
	HD44780_PrintStr("G3 ROBOTICS");
	HD44780_SetCursor(0, 1);
	HD44780_PrintStr("POINTS:  ");
	HD44780_SetCursor(9, 1);
	HD44780_PrintStr(snum);
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
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
