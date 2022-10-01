/*
 * msp.c
 *
 *  Created on: Sep 16, 2022
 *      Author: Rene
 */

#include "msp.h"


void HAL_MspInit(void)
{
 // Low Level processor specific initialization
	// 1. Set the priority grouping of the processor
	HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);
	// 2. Enable the required system exceptions of the processor
	SCB->SHCSR |= (0x7 << 16); //Enabled USAGE Fault MEMFAULT, BUS FAULT system exception

	// 3. configure the priority for the system exceptions.
	HAL_NVIC_SetPriority(MemoryManagement_IRQn, 0, 0);
	HAL_NVIC_SetPriority(UsageFault_IRQn, 0, 0);
	HAL_NVIC_SetPriority(BusFault_IRQn, 0, 0);

}

void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
	GPIO_InitTypeDef gpio_uart;

	// 1. Enable clock for the USART2 peripheral and GPIOA
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_USART2_CLK_ENABLE();
	// 2. Do the pin muxing configurations.
	gpio_uart.Pin = GPIO_PIN_2;
	gpio_uart.Mode = GPIO_MODE_AF_PP;
	gpio_uart.Pull = GPIO_PULLUP;
	gpio_uart.Speed = GPIO_SPEED_FREQ_LOW;
	gpio_uart.Alternate = GPIO_AF7_USART2; //UART2_tx
	HAL_GPIO_Init(GPIOA, &gpio_uart);

	gpio_uart.Pin = GPIO_PIN_3;
	HAL_GPIO_Init(GPIOA, &gpio_uart); //Uart2_tx

	// 3. Enable the IRQ and set up the priority ( NVIC settings)
	HAL_NVIC_EnableIRQ(USART2_IRQn);
	HAL_NVIC_SetPriority(USART2_IRQn, 15,  0);

}
