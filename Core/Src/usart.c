/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usart.c
  * @brief   This file provides code for the configuration
  *          of the USART instances.
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
	
#include <stdlib.h>
#include <string.h>


/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "usart.h"

/* USER CODE BEGIN 0 */
uint8_t ubButtonPress;
/**
  * @brief RX buffers for storing received data
  */
	
uint8_t aRXBuffer[RX_BUFFER_SIZE];

__IO uint32_t     uwNbReceivedChars;
//__IO uint8_t     aRXIndication=0;


uint8_t pycommand=0;

/* USER CODE END 0 */

/* USART2 init function */

void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  LL_USART_InitTypeDef USART_InitStruct = {0};

  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* Peripheral clock enable */
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_USART2);

  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
  /**USART2 GPIO Configuration
  PA2   ------> USART2_TX
  PA3   ------> USART2_RX
  */
  GPIO_InitStruct.Pin = USART_TX_Pin|USART_RX_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_7;
  LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* USART2 interrupt Init */
  NVIC_SetPriority(USART2_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),8, 0));
  NVIC_EnableIRQ(USART2_IRQn);

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  USART_InitStruct.BaudRate = 115200;
  USART_InitStruct.DataWidth = LL_USART_DATAWIDTH_8B;
  USART_InitStruct.StopBits = LL_USART_STOPBITS_1;
  USART_InitStruct.Parity = LL_USART_PARITY_NONE;
  USART_InitStruct.TransferDirection = LL_USART_DIRECTION_TX_RX;
  USART_InitStruct.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
  USART_InitStruct.OverSampling = LL_USART_OVERSAMPLING_16;
  LL_USART_Init(USART2, &USART_InitStruct);
  LL_USART_ConfigAsyncMode(USART2);
  LL_USART_Enable(USART2);
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/* USER CODE BEGIN 1 */
/**
  * @brief  Send Txt information message on USART Tx line (to PC Com port).
  * @param  None
  * @retval None
  */
void printInfo(uint8_t *senString, uint32_t Size)
{
  uint32_t index = 0;
  uint8_t *pchar; 
	pchar = senString;
  
  /* Send characters one per one, until last char to be sent */
  for (index = 0; index < Size; index++)
  {
    /* Wait for TXE flag to be raised */
    while (!LL_USART_IsActiveFlag_TXE(USART2))
    {
    }

    /* Write character in Transmit Data register.
       TXE flag is cleared by writing data in DR register */
    LL_USART_TransmitData8(USART2, *pchar++);
  }

  /* Wait for TC flag to be raised for last char */
  while (!LL_USART_IsActiveFlag_TC(USART2))
  {
  }
}



/**
  * @brief  Send String on USART Tx line (to PC Com port).
  * @param  None
  * @retval None
  */
void printString(uint8_t *senString)
{
  //uint32_t index = 0;
  uint8_t *pchar;
	pchar=senString;
  
  /* Send characters one per one, until last char to be sent */
  while(1)
  {
    /* Wait for TXE flag to be raised */
    while (!LL_USART_IsActiveFlag_TXE(USART2))
    {
    }

    /* Write character in Transmit Data register.
       TXE flag is cleared by writing data in DR register */
    LL_USART_TransmitData8(USART2, *pchar);
		
		if((*pchar)=='\n')break;
		pchar++;
		
  }
	
  /* Wait for TC flag to be raised for last char */
  while (!LL_USART_IsActiveFlag_TC(USART2))
  {
  }
}

/**
  * @brief  This function prints user info on PC com port and initiates RX transfer
  * @param  None
  * @retval None
  */
void StartReception(void)
{
  /* Initializes Buffer swap mechanism :
     - 2 physical buffers aRXBufferA and aRXBufferB (RX_BUFFER_SIZE length)
     
  */
  uwNbReceivedChars = 0;
  //aRXIndication = 1;

  /* Print user info on PC com port */
//  PrintInfo(aTextInfoStart, sizeof(aTextInfoStart));

  /* Clear Overrun flag, in case characters have already been sent to USART */
  LL_USART_ClearFlag_ORE(USART2);

  /* Enable RXNE and Error interrupts */
  LL_USART_EnableIT_RXNE(USART2);
  LL_USART_EnableIT_ERROR(USART2);
}






void USART_CharReceptionContinuous_Callback(uint8_t endByte)//continues receive bytes until end byte 
{
	uint8_t ctemp;

  /* Read Received character. RXNE flag is cleared by reading of DR register */

  ctemp = LL_USART_ReceiveData8(USART2);
	aRXBuffer[uwNbReceivedChars++]=ctemp;
  /* Checks if Buffer full indication has been set */
	
  if (ctemp == endByte)
  {
		aRXBuffer[uwNbReceivedChars-1] = '\0';		
		pycommand=atoi(aRXBuffer);
		osSemaphoreRelease(myBinarySem01Handle);
    /* Set Buffer swap indication */
    //aRXIndication = 0;
    uwNbReceivedChars = 0;
  }

}

/* USER CODE END 1 */
