/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#define IN_HVIL_ACUM_Pin GPIO_PIN_1
#define IN_HVIL_ACUM_GPIO_Port GPIOA
#define LED_HV_Pin GPIO_PIN_2
#define LED_HV_GPIO_Port GPIOA
#define LED_BAL_Pin GPIO_PIN_3
#define LED_BAL_GPIO_Port GPIOA
#define CP_SIG_Pin GPIO_PIN_4
#define CP_SIG_GPIO_Port GPIOA
#define IN_HVIL_CHAR_Pin GPIO_PIN_5
#define IN_HVIL_CHAR_GPIO_Port GPIOA
#define IN_HVIL_F_SW_Pin GPIO_PIN_6
#define IN_HVIL_F_SW_GPIO_Port GPIOA
#define IN_HVIL_ESTOP_Pin GPIO_PIN_7
#define IN_HVIL_ESTOP_GPIO_Port GPIOA
#define IN_BMS_FLT_LED_Pin GPIO_PIN_1
#define IN_BMS_FLT_LED_GPIO_Port GPIOB
#define IN_IMD_FLT_LED_Pin GPIO_PIN_12
#define IN_IMD_FLT_LED_GPIO_Port GPIOB
#define THERM_2_SIG_Pin GPIO_PIN_13
#define THERM_2_SIG_GPIO_Port GPIOB
#define THERM_1_SIG_Pin GPIO_PIN_14
#define THERM_1_SIG_GPIO_Port GPIOB
#define LED_ELCON_FLT_Pin GPIO_PIN_15
#define LED_ELCON_FLT_GPIO_Port GPIOB
#define SHUNT_SIG_Pin GPIO_PIN_6
#define SHUNT_SIG_GPIO_Port GPIOC
#define LED_TSAL_FLT_Pin GPIO_PIN_7
#define LED_TSAL_FLT_GPIO_Port GPIOC
#define HVIL_CTRL_Pin GPIO_PIN_9
#define HVIL_CTRL_GPIO_Port GPIOC
#define BTN_1_Pin GPIO_PIN_10
#define BTN_1_GPIO_Port GPIOC
#define BTN_2_Pin GPIO_PIN_11
#define BTN_2_GPIO_Port GPIOC
#define BTN_3_Pin GPIO_PIN_12
#define BTN_3_GPIO_Port GPIOC
#define BTN_4_Pin GPIO_PIN_2
#define BTN_4_GPIO_Port GPIOD
#define FAN_CRTL_Pin GPIO_PIN_4
#define FAN_CRTL_GPIO_Port GPIOB
#define RTC_SW_Pin GPIO_PIN_5
#define RTC_SW_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
