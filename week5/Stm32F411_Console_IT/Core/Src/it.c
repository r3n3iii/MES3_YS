/*
 * it.c
 *
 *  Created on: Sep 16, 2022
 *      Author: Rene
 */
#include "it.h"

extern UART_HandleTypeDef huart2;

void SysTick_Handler(void){
	HAL_IncTick();
	HAL_SYSTICK_IRQHandler();

}

void USART2_IRQHandler(void)
{
	HAL_UART_IRQHandler(&huart2);
}
