/*
 * consoleIo.c
 *
 *  Created on: Oct. 1, 2022
 *      Author: Rene
 */


#include "consoleIo.h"
#include <stdio.h>


uint8_t data_buffer[100];

/*
eConsoleError ConsoleIoInit(void)
{
	huart2.Instance = USART2;
	huart2.Init.BaudRate = 115200;
	huart2.Init.WordLength = UART_WORDLENGTH_8B;
	huart2.Init.StopBits = UART_STOPBITS_1;
	huart2.Init.Parity = UART_PARITY_NONE;
	huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;

	huart2.Init.Mode =	UART_MODE_TX_RX;

	if (HAL_UART_Init(&huart2) != HAL_OK)
	{
		UartError_handler();
	}

	return CONSOLE_SUCCESS;
}

void UartError_handler(void)
{
		while(1);
}
// This is modified for the Wokwi RPi Pico simulator. It works fine
// but that's partially because the serial terminal sends all of the
// characters at a time without losing any of them. What if this function
// wasn't called fast enough?
eConsoleError ConsoleIoReceive(uint8_t *buffer, const uint32_t bufferLength, uint32_t *readLength)
{
	uint32_t count = 0;

	uint8_t rcvd_data;
	while(1)
	{
		HAL_UART_Receive(&huart2, &rcvd_data, bufferLength, HAL_MAX_DELAY);
		if(rcvd_data =='\r'){
			break;
		}
		else{
			buffer[count++] = rcvd_data;
		}

	}
	//HAL_UART_Transmit(&huart2, &rcvd_data, 1, HAL_MAX_DELAY);
	*readLength = count;

	return CONSOLE_SUCCESS;
}
*/
eConsoleError ConsoleIoSendString(const char *buffer)
{

	HAL_UART_Transmit(&huart2, buffer,(uint16_t)strlen(buffer), HAL_MAX_DELAY);
	return CONSOLE_SUCCESS;
}


