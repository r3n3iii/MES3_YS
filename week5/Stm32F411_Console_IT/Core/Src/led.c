/*
 * led.c
 *
 *  Created on: Sep 16, 2022
 *      Author: Rene
 */


#include "led.h"

void led_Init(void){

	GPIO_InitTypeDef GPIO_InitStruct = {0};
	__HAL_RCC_GPIOD_CLK_ENABLE();
	GPIO_InitStruct.Pin = LED_ORANGE|LED_RED|LED_BLUE|LED_GREEN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
}

void led_Toggle(uint16_t GPIO_Pin)
{
	HAL_GPIO_TogglePin(GPIOD, GPIO_Pin);
}

void led_Write(uint16_t GPIO_Pin, GPIO_PinState PinState)
{
	HAL_GPIO_WritePin(GPIOD, GPIO_Pin, PinState);
}
