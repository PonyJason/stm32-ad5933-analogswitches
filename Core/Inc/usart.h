/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usart.h
  * @brief   This file contains all the function prototypes for
  *          the usart.c file
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
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
#ifndef __USART_H__
#define __USART_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

#include "FreeRTOS.h"
#include "cmsis_os.h"

/* USER CODE END Includes */

/* USER CODE BEGIN Private defines */
#define RX_BUFFER_SIZE   30

extern  uint8_t ubButtonPress;



/**
  * @brief RX buffers for storing received data
  */
extern uint8_t aRXBuffer[RX_BUFFER_SIZE];

extern __IO uint32_t     uwNbReceivedChars;
//extern __IO uint8_t     aRXIndication;

extern uint8_t pycommand;
extern osSemaphoreId_t myBinarySem01Handle;

/* USER CODE END Private defines */

void MX_USART2_UART_Init(void);

/* USER CODE BEGIN Prototypes */

void USART_CharReceptionContinuous_Callback(uint8_t endByte);
void StartReception(void);
void printInfo(uint8_t *senString, uint32_t Size);
void printString(uint8_t *senString);


/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __USART_H__ */

