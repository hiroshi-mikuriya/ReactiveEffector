/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
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
#include "stm32f7xx_hal.h"

#include "stm32f7xx_ll_adc.h"
#include "stm32f7xx_ll_crc.h"
#include "stm32f7xx_ll_dma.h"
#include "stm32f7xx_ll_i2c.h"
#include "stm32f7xx_ll_rcc.h"
#include "stm32f7xx_ll_bus.h"
#include "stm32f7xx_ll_system.h"
#include "stm32f7xx_ll_exti.h"
#include "stm32f7xx_ll_cortex.h"
#include "stm32f7xx_ll_utils.h"
#include "stm32f7xx_ll_pwr.h"
#include "stm32f7xx_ll_spi.h"
#include "stm32f7xx_ll_tim.h"
#include "stm32f7xx_ll_usart.h"
#include "stm32f7xx_ll_gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "cmsis_os.h"
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
#define SRAM_SPI SPI2
#define NP_SPI SPI3
#define RPI_SPI SPI4
#define RPI_UART USART1
#define IR_UART USART2
#define RPI_SPI_MOSI_Pin LL_GPIO_PIN_6
#define RPI_SPI_MOSI_GPIO_Port GPIOE
#define SRAM_SPI_MISO_Pin LL_GPIO_PIN_2
#define SRAM_SPI_MISO_GPIO_Port GPIOC
#define SRAM_SPI_MOSI_Pin LL_GPIO_PIN_3
#define SRAM_SPI_MOSI_GPIO_Port GPIOC
#define LEFT_VOL_ADC_Pin LL_GPIO_PIN_1
#define LEFT_VOL_ADC_GPIO_Port GPIOA
#define RIGHT_VOL_ADC_Pin LL_GPIO_PIN_2
#define RIGHT_VOL_ADC_GPIO_Port GPIOA
#define TP13_Pin LL_GPIO_PIN_3
#define TP13_GPIO_Port GPIOA
#define TP14_Pin LL_GPIO_PIN_4
#define TP14_GPIO_Port GPIOA
#define GPIO_IN_INT_SW2_N_Pin LL_GPIO_PIN_5
#define GPIO_IN_INT_SW2_N_GPIO_Port GPIOA
#define GPIO_IN_INT_SW2_N_EXTI_IRQn EXTI9_5_IRQn
#define GPIO_IN_INT_MPU_N_Pin LL_GPIO_PIN_6
#define GPIO_IN_INT_MPU_N_GPIO_Port GPIOA
#define GPIO_IN_INT_MPU_N_EXTI_IRQn EXTI9_5_IRQn
#define GPIO_IN_INT_SW_N_Pin LL_GPIO_PIN_7
#define GPIO_IN_INT_SW_N_GPIO_Port GPIOA
#define GPIO_IN_INT_SW_N_EXTI_IRQn EXTI9_5_IRQn
#define RPI_SPI_NSS_Pin LL_GPIO_PIN_11
#define RPI_SPI_NSS_GPIO_Port GPIOE
#define RPI_SPI_NSS_EXTI_IRQn EXTI15_10_IRQn
#define RPI_SPI_SCK_Pin LL_GPIO_PIN_12
#define RPI_SPI_SCK_GPIO_Port GPIOE
#define RPI_SPI_MISO_Pin LL_GPIO_PIN_13
#define RPI_SPI_MISO_GPIO_Port GPIOE
#define SRAM_SPI_SCK_Pin LL_GPIO_PIN_10
#define SRAM_SPI_SCK_GPIO_Port GPIOB
#define SRAM_SPI_NSS_Pin LL_GPIO_PIN_12
#define SRAM_SPI_NSS_GPIO_Port GPIOB
#define LED_ACT_Pin LL_GPIO_PIN_7
#define LED_ACT_GPIO_Port GPIOC
#define RPI_UART_TX_Pin LL_GPIO_PIN_9
#define RPI_UART_TX_GPIO_Port GPIOA
#define UNUSED_SPI_SCK_Pin LL_GPIO_PIN_10
#define UNUSED_SPI_SCK_GPIO_Port GPIOC
#define NEOPIXEL_DT_Pin LL_GPIO_PIN_12
#define NEOPIXEL_DT_GPIO_Port GPIOC
#define PCM_RST_Pin LL_GPIO_PIN_0
#define PCM_RST_GPIO_Port GPIOD
#define IR_UART_TX_Pin LL_GPIO_PIN_5
#define IR_UART_TX_GPIO_Port GPIOD
#define IR_UART_RX_Pin LL_GPIO_PIN_6
#define IR_UART_RX_GPIO_Port GPIOD
#define RPI_UART_RX_Pin LL_GPIO_PIN_7
#define RPI_UART_RX_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */
extern osThreadId usbTxTaskHandle;
extern osThreadId i2cTaskHandle;
extern osThreadId neoPixelTaskHandle;
extern osThreadId appTaskHandle;
extern osThreadId soundTaskHandle;
extern osThreadId adcTaskHandle;
extern osThreadId i2cMonitorTaskHandle;
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
