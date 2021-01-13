/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
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
#include "stm32l4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "stdbool.h"

#define BUTTON_USER_Pin GPIO_PIN_2
#define BUTTON_USER_GPIO_Port GPIOA
#define RS485_RTS_Pin GPIO_PIN_8
#define RS485_RTS_GPIO_Port GPIOA

#define CAN_RES_Pin GPIO_PIN_0
#define CAN_RES_Port GPIOC

#define CAN_POW_Pin GPIO_PIN_1
#define CAN_POW_Port GPIOC

#define RS485_RES_Pin GPIO_PIN_14
#define RS485_RES_Port GPIOB

#define RS485_POW_Pin GPIO_PIN_15
#define RS485_POW_Port GPIOB

#define RS485_DIR_PORT GPIOA
#define RS485_DIR_Pin GPIO_PIN_4
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

#define A111_SPI_HANDLE hspi3
//#define A111_USE_SPI_DMA

//#define A111_CLOCK_BUFFER_ENABLE_Pin GPIO_PIN_0
//#define A111_CLOCK_BUFFER_ENABLE_GPIO_Port GPIOC
//
//#define A111_SPI_SCK_Pin GPIO_PIN_5
//#define A111_SPI_SCK_GPIO_Port GPIOA
//#define A111_SPI_MISO_Pin GPIO_PIN_6
//#define A111_SPI_MISO_GPIO_Port GPIOA
//#define A111_SPI_MOSI_Pin GPIO_PIN_7
//#define A111_SPI_MOSI_GPIO_Port GPIOA
//
#define A111_SENSOR_INTERRUPT_Pin GPIO_PIN_8
#define A111_SENSOR_INTERRUPT_GPIO_Port GPIOC
#define A111_SENSOR_INTERRUPT_EXTI_IRQn EXTI9_5_IRQn
//#define A111_RESET_N_Pin GPIO_PIN_9
//#define A111_RESET_N_GPIO_Port GPIOA
//
#define A111_ENABLE_Pin GPIO_PIN_5
#define A111_ENABLE_GPIO_Port GPIOB
#define A111_CS_N_Pin GPIO_PIN_6
#define A111_CS_N_GPIO_Port GPIOB
//
//#define A111_USE_SPI_DMA


extern TIM_HandleTypeDef htim2;
extern bool tim2_timeout;
/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
