/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
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

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "gpio.h"
//#include "tim.h"
#include "delay.h"
#include <string.h>
#include <stdio.h>
#include "usart.h"
#include "queue.h"
#include "HX711.h"
#include "oled.h"

#include "iic.h"
#include "ad5933.h"

#include "bmp.h"
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
/* USER CODE BEGIN Variables */
uint32_t realForce=0;
uint8_t forceTXBuffer[200];
uint8_t impTXBuffer[800];
uint8_t phTXBuffer[800];
uint8_t singleSample[100];
uint8_t pair_code;

ImpeType  impe;

/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for myTask02 */
osThreadId_t myTask02Handle;
const osThreadAttr_t myTask02_attributes = {
  .name = "myTask02",
  .stack_size = 1024 * 4,
  .priority = (osPriority_t) osPriorityRealtime,
};
/* Definitions for myTask03 */
osThreadId_t myTask03Handle;
const osThreadAttr_t myTask03_attributes = {
  .name = "myTask03",
  .stack_size = 1024 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for myTask04 */
osThreadId_t myTask04Handle;
const osThreadAttr_t myTask04_attributes = {
  .name = "myTask04",
  .stack_size = 1024 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for myBinarySem01 */
osSemaphoreId_t myBinarySem01Handle;
const osSemaphoreAttr_t myBinarySem01_attributes = {
  .name = "myBinarySem01"
};
/* Definitions for myBinarySem02 */
osSemaphoreId_t myBinarySem02Handle;
const osSemaphoreAttr_t myBinarySem02_attributes = {
  .name = "myBinarySem02"
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
void adg708set_pin_port(uint8_t pin_pair_id);


/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
void StartTask02(void *argument);
void StartTask03(void *argument);
void StartTask04(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* Create the semaphores(s) */
  /* creation of myBinarySem01 */
  myBinarySem01Handle = osSemaphoreNew(1, 1, &myBinarySem01_attributes);

  /* creation of myBinarySem02 */
  myBinarySem02Handle = osSemaphoreNew(1, 1, &myBinarySem02_attributes);

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* creation of myTask02 */
  myTask02Handle = osThreadNew(StartTask02, NULL, &myTask02_attributes);

  /* creation of myTask03 */
  myTask03Handle = osThreadNew(StartTask03, NULL, &myTask03_attributes);

  /* creation of myTask04 */
  myTask04Handle = osThreadNew(StartTask04, NULL, &myTask04_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  for(;;)
  {	
		LL_GPIO_TogglePin(GPIOA,LL_GPIO_PIN_5);
		osDelay(500);
  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_StartTask02 */
/**
* @brief Function implementing the myTask02 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask02 */
void StartTask02(void *argument)
{
	
  /* USER CODE BEGIN StartTask02 */
	uint8_t i;
	uint8_t itemp[50];
	//IIC_Init();	
	//OLED_Init();
	float temperature;
	temperature = Temperature_Test();
	AD5933_FreInit(100000,100);
//	OLED_DrawBMP(0,0,127,7,BMP0);
//	osDelay(3000);
//	OLED_Clear();
//	OLED_ShowString(0,0,"force",12);
//	OLED_ShowString(0,2,"Am",12);
//	OLED_ShowString(0,4,"Ph",12);	
	
//	HAL_Delay(300);
  /* Infinite loop */
	osDelay(4000);
	sprintf(itemp, "%.1f", temperature);
	OLED_ShowString(60,4,itemp,12);
  for(;;)
  {
		osSemaphoreAcquire(myBinarySem02Handle,osWaitForever);
		
		for(i=0, pair_code=0;i<6;i++)
		{
			adg708set_pin_port(pair_code++);
			AD5933_StartTest(0);
			AD5933_ReadImpedance(&impe);
			sprintf(itemp, "%.1f,", impe.Impedance);
			strcat(impTXBuffer,(const char *)itemp);
			sprintf(itemp, "%.3f,", impe.Phase);
			strcat(phTXBuffer,(const char *)itemp);
			osDelay(1);
		}
		
//			AD5933_StartTest(0);
//			AD5933_ReadImpedance(&impe);
//			sprintf(itemp, "%.2f,%.2f\r\n", impe.Impedance,impe.Phase);
//			printString(itemp);
		
//		OLED_ShowNum(50,2,impe.Impedance,5,32);
//		OLED_ShowNum(50,4,impe.Phase,5,32);

		//HandleContinuousReception();
		osDelay(100);
  }
  /* USER CODE END StartTask02 */
}

/* USER CODE BEGIN Header_StartTask03 */
/**
* @brief Function implementing the myTask03 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask03 */
void StartTask03(void *argument)
{
  /* USER CODE BEGIN StartTask03 */
	/*task3 majored in force meter*/
	//pair_code = 0;
	uint8_t ftemp[15];
	uint8_t listLen;
	OLED_Init();
	OLED_DrawBMP(0,0,127,7,BMP0);
	osDelay(3000);
	OLED_Clear();
	OLED_ShowString(0,0,"Force Impedance",12);
	OLED_ShowString(0,2,"force",12);
	OLED_ShowString(0,4,"temp",12);
	StartReception();
	Get_Maopi();
	osDelay(1000);
	//printString("welcome\n");
  /* Infinite loop */
  for(;;)
  {
		osSemaphoreAcquire(myBinarySem01Handle,osWaitForever);
		switch(pycommand)
		{
			case 1://continually add force into the force list
				Get_Weight();
				realForce=Weight_Shiwu;
				sprintf(ftemp, "%d,", realForce);
				OLED_ShowNum(60,2,realForce,4,32);
				strcat(forceTXBuffer,(const char *)ftemp);
			//release sem02 to release task02 to change the mux
				osSemaphoreRelease(myBinarySem02Handle);

				break;
			case 2://continually add force into the force list

			//release sem02 to release task02 to change the mux
			//	osSemaphoreRelease(myBinarySem02Handle);


				break;
			case 3://reset force and impedance list first
				forceTXBuffer[0]='[';
				forceTXBuffer[1]='\0';
				impTXBuffer[0]='[';
				impTXBuffer[1]='\0';
				phTXBuffer[0] = '[';
				phTXBuffer[1] = '\0';
				printString("ok\n");
				pair_code=0;
//				aRXIndication=1;
				break;
			case 4://complete force list construction
				listLen=strlen(forceTXBuffer);
				forceTXBuffer[listLen-1]=']';
				forceTXBuffer[listLen]='\n';
				forceTXBuffer[listLen+1]='\0';
				printString(forceTXBuffer);
				/*set rx indication*/
//				aRXIndication=1;
				break;
			case 5://complete Impedance list construction
				listLen=strlen(impTXBuffer);
				impTXBuffer[listLen-1]=']';
				impTXBuffer[listLen]='\n';
				impTXBuffer[listLen+1]='\0';
				printString(impTXBuffer);
				/*set rx indication*/
//				aRXIndication=1;
				break;
			case 6://complete Impedance list construction
				listLen=strlen(phTXBuffer);
				phTXBuffer[listLen-1]=']';
				phTXBuffer[listLen]='\n';
				phTXBuffer[listLen+1]='\0';
				printString(phTXBuffer);
				/*set rx indication*/
//				aRXIndication=1;
				break;
			case 7://single sampling process
				singleSample[0]='[';
				singleSample[1]='\0';
				impTXBuffer[0]='\0';
				phTXBuffer[0] = '\0';
				Get_Weight();
				realForce=Weight_Shiwu;
				sprintf(ftemp, "%d,", realForce);
				strcat(singleSample,(const char *)ftemp);
				//release sem02 to release task02 to change the mux
				osSemaphoreRelease(myBinarySem02Handle);
				osDelay(50);
				strcat(singleSample, (const char *)impTXBuffer);
				strcat(singleSample, (const char *)phTXBuffer);
				listLen=strlen(singleSample);
				singleSample[listLen-1]=']';
				singleSample[listLen]='\n';
				singleSample[listLen+1]='\0';
				printString(singleSample);
				break;

			default:
//				aRXIndication=1;	
				break;
		}
		
    osDelay(1);
  }

  

  /* USER CODE END StartTask03 */
}

/* USER CODE BEGIN Header_StartTask04 */
/**
* @brief Function implementing the myTask04 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask04 */
void StartTask04(void *argument)
{
  /* USER CODE BEGIN StartTask04 */
	//
	
	
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartTask04 */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */



/**
* @brief set up switchers' array
* PC7 PC6 PC5 PC4 PC3 PC2 PC1 PC0
* ENB B2  B1  B0  ENA A2  A1  A0
* @param pin_pair_id 
* @retval 
* @auther lu jiaxin
*/
void adg708set_pin_port(uint8_t pin_pair_id)
{
	switch(pin_pair_id)
	{
		case 0:
			LL_GPIO_WriteOutputPort(GPIOC, 0x00000098);
			break;
		case 1:
			LL_GPIO_WriteOutputPort(GPIOC, 0x000000a9);
			break;
		case 2:
			LL_GPIO_WriteOutputPort(GPIOC, 0x000000ba);
			break;
		case 3:
			LL_GPIO_WriteOutputPort(GPIOC, 0x000000cb);
		case 4:
			LL_GPIO_WriteOutputPort(GPIOC, 0x000000dc);
		case 5:
			LL_GPIO_WriteOutputPort(GPIOC, 0x0000008d);
			break;
	}

}

/* USER CODE END Application */

