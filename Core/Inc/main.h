/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "../Lib/io/io.h"
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include "../Lib/timer/timer.h"
#include "../Lib/odometry/odometry.h"
#include "../Lib/uart/uart.h"
#include "../Lib/dynamixel/dynamixel.h"
#include "../Lib/tactics/tactics.h"
#include "../Lib/regulation/regulation.h"
#include "../Lib/pwm/pwm.h"
#include "../Lib/encoder/encoder.h"
#include "../Lib/sensors/sensors.h"
#include "../Lib/h-bridge/h-bridge.h"
#include "../Lib/display/liquidcrystal_i2c.h"
#include "../Lib/movement/movement.h"
#include "../Lib/tactics/task_modules.h"
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

void
write_to_display (uint8_t points, char tactic[10]);
void
write_to_display_time (uint8_t points, uint8_t time);

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
