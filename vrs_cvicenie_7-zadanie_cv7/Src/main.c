/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
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

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "dma.h"
#include "usart.h"
#include "gpio.h"
#include <stdlib.h>
#include <string.h>

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);


/* Function processing DMA Rx data. Counts how many capital and small letters are in sentence.
 * Result is supposed to be stored in global variable of type "letter_count_" that is defined in "main.h"
 *
 * @param1 - received sign
 */
void proccesDmaData(const uint8_t* data, uint16_t len);


/* Space for your global variables. */

uint8_t index_buffera=0;


uint8_t message[]= "Buffer capacity: %d bytes, occupied memory: %d bytes, load [in %]: %f%\n";

uint8_t buffer[35];
char buffer_capacity[10];
char buffer_memory[10];
char buffer_load[10];

uint16_t small_letter=0, capital_letter=0;

double load;

 int flag=0;

	// type your global variables here:

int main(void)
{
  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SYSCFG);
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);
  NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);


  /* Configure the system clock */
  SystemClock_Config();
  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_USART2_UART_Init();

  /* Space for your local variables, callback registration ...*/

  USART2_RegisterCallback(proccesDmaData);



  while (1){
	  /* Periodic transmission of information about DMA Rx buffer state.
	   * Transmission frequency - 5Hz.
	   * Message format - "Buffer capacity: %d bytes, occupied memory: %d bytes, load [in %]: %f%"
	   * Example message (what I wish to see in terminal) - Buffer capacity: 1000 bytes, occupied memory: 231 bytes, load [in %]: 23.1%
	   */
	  LL_mDelay(5000);

	  for(uint8_t j=0; j<=67; j++){
		  if(j==17){
			  itoa(DMA_USART2_BUFFER_SIZE, buffer_capacity, 10);
			  for(uint8_t i=0; i<10; i++){
				  LL_USART_TransmitData8(USART2, buffer_capacity[i]);
				  LL_mDelay(1);

			  }

			  j++;
			  memset(buffer_capacity,'\000',10);
		  }
		  else if(j==44){
			  itoa(DMA_USART2_BUFFER_SIZE-LL_DMA_GetDataLength(DMA1, LL_DMA_CHANNEL_6), buffer_memory, 10);
			  for(uint8_t i=0; i<10; i++){
				  LL_USART_TransmitData8(USART2, buffer_memory[i]);
				  LL_mDelay(1);
			  }
			  j++;

			  memset(buffer_memory,'\000',10);

		  }
		  else if(j==65){
			  load=(double)(DMA_USART2_BUFFER_SIZE - LL_DMA_GetDataLength(DMA1, LL_DMA_CHANNEL_6));
				  load=(100*load)/DMA_USART2_BUFFER_SIZE;
				  sprintf(buffer_load, "%f" ,load);
				  for(uint8_t i=0; i<sizeof(buffer_load); i++){
					  LL_USART_TransmitData8(USART2, buffer_load[i]);
					  LL_mDelay(1);
				  }
				  memset(buffer_load,'\000',6);
			  //LL_USART_TransmitData8(USART2, 100*(DMA_USART2_BUFFER_SIZE - LL_DMA_GetDataLength(DMA1, LL_DMA_CHANNEL_6))/(DMA_USART2_BUFFER_SIZE));
		  }
		  else{
			  LL_USART_TransmitData8(USART2, message[j]);
		  }
		  LL_mDelay(1);
	  }
  }
}


void SystemClock_Config(void)
{
  LL_FLASH_SetLatency(LL_FLASH_LATENCY_0);

  if(LL_FLASH_GetLatency() != LL_FLASH_LATENCY_0)
  {
  Error_Handler();  
  }
  LL_RCC_HSI_Enable();

   /* Wait till HSI is ready */
  while(LL_RCC_HSI_IsReady() != 1)
  {
    
  }
  LL_RCC_HSI_SetCalibTrimming(16);
  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
  LL_RCC_SetAPB2Prescaler(LL_RCC_APB1_DIV_1);
  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_HSI);

   /* Wait till System clock is ready */
  while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_HSI)
  {
  
  }
  LL_Init1msTick(8000000);
  LL_SYSTICK_SetClkSource(LL_SYSTICK_CLKSOURCE_HCLK);
  LL_SetSystemCoreClock(8000000);
}

/*
 * Implementation of function processing data received via USART.
 */

void vypisMainMessage(){

}


void proccesDmaData(const uint8_t* data, uint16_t n)
{

	for(uint8_t i = 0; i<n; i++){
		if (*(data+i) == 35){
			flag=1;
		}


		if(flag){

			buffer[i]=*(data+i);

			if (*(data+i) == 36){

				for(uint8_t j = 0; j<sizeof(buffer); j++){
					if((buffer[j] > 96) && (buffer[j] < 123)){
						small_letter++;
					}
					if((buffer[j] > 64) && (buffer[j]<91)){
						capital_letter++;
					}
				}
				memset(buffer,'\000',sizeof(buffer));
				flag=0;
			}

			if(i==n-1){
				flag=0;
				memset(buffer,'\000',sizeof(buffer));
			}

		}
	}
}


void Error_Handler(void)
{

}



#ifdef  USE_FULL_ASSERT

void assert_failed(char *file, uint32_t line)
{ 

}

#endif

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
