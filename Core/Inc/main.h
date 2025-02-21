/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

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

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define LED_HV_Pin GPIO_PIN_2
#define LED_HV_GPIO_Port GPIOA
#define LED_BAL_Pin GPIO_PIN_3
#define LED_BAL_GPIO_Port GPIOA
#define LED_CP_Pin GPIO_PIN_4
#define LED_CP_GPIO_Port GPIOA
#define LED_TC_FLT_Pin GPIO_PIN_15
#define LED_TC_FLT_GPIO_Port GPIOB
#define BTN_TOP_Pin GPIO_PIN_10
#define BTN_TOP_GPIO_Port GPIOC
#define BTN_BCK_Pin GPIO_PIN_11
#define BTN_BCK_GPIO_Port GPIOC
#define BTN_SEL_Pin GPIO_PIN_12
#define BTN_SEL_GPIO_Port GPIOC
#define BTN_BTM_Pin GPIO_PIN_2
#define BTN_BTM_GPIO_Port GPIOD

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
