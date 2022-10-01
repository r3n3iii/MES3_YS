/*
 * main.c
 *
 *  Created on: Sep 16, 2022
 *      Author: Rene
 */



#include "main.h"
#include "led.h"
#include <string.h>
#include "console.h"

#define TRUE 	1
#define FALSE 	0

#define NOT_FOUND		-1
#define INT16_MAX_STR_LENGTH 8 // -65534: six characters plus a two NULLs
#define INT32_MAX_STR_LENGTH 16
#define NULL_CHAR            '\0'
#define CR_CHAR              '\r'
#define LF_CHAR              '\n'


#define CONSOLE_COMMAND_MAX_COMMAND_LENGTH 10		// command only
#define CONSOLE_COMMAND_MAX_LENGTH 256				// whole command with argument
#define CONSOLE_COMMAND_MAX_HELP_LENGTH 64			// if this is zero, there will be no  help (XXXOPT: RAM reduction)

//Function Prototypes
void SystemClockConfig(void);

void UART2_Init(void);
void Error_handler(void);

static uint32_t ConsoleResetBuffer(char receiveBuffer[], const uint32_t filledLength, uint32_t usedSoFar);

UART_HandleTypeDef huart2;

char *cli_init = "Console Initialized. Type your command.(help, ver) \r\n";

uint8_t mReceiveBuffer[100];
uint8_t recvd_data;
uint8_t count = 0;
uint8_t reception_complete = FALSE;


int main(void){

	HAL_Init();
	SystemClockConfig();

	UART2_Init();

	uint16_t len_of_data = strlen(cli_init);

	if (HAL_UART_Transmit(&huart2,(uint8_t*)cli_init, len_of_data, HAL_MAX_DELAY) != HAL_OK)
	{
		Error_handler();
	}
	if (HAL_UART_Transmit(&huart2,CONSOLE_PROMPT, (uint16_t)strlen(CONSOLE_PROMPT), HAL_MAX_DELAY) != HAL_OK)
		{
			Error_handler();
		}



	while(1){
		while (reception_complete != TRUE){
				HAL_UART_Receive_IT(&huart2, &recvd_data, 1);
			}
		reception_complete = FALSE;
		ConsoleProcess();

		ConsoleResetBuffer(mReceiveBuffer,100, count);
		count =0;
		ConsoleIoSendString(CONSOLE_PROMPT);
	}

	return 0;
}

void SystemClockConfig(void){

}

void UART2_Init(void)
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
		Error_handler();
	}
}
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{

	if(recvd_data == '\r'){
		reception_complete = TRUE;
		mReceiveBuffer[count++] = '\r';
		//HAL_UART_Transmit(&huart2, data_buffer, count, HAL_MAX_DELAY);
		//HAL_UART_Transmit(&huart2,CONSOLE_PROMPT, (uint16_t)strlen(CONSOLE_PROMPT), HAL_MAX_DELAY);
	}else
	{

		mReceiveBuffer[count++] = recvd_data;
	}

}
void Error_handler(void)
{
		while(1);
}

static uint32_t ConsoleResetBuffer(char receiveBuffer[], const uint32_t filledLength, uint32_t usedSoFar)
{
	uint32_t remaining = (filledLength - usedSoFar);
	uint32_t i = 0;

	while (usedSoFar < filledLength)
	{
		receiveBuffer[i] = receiveBuffer[usedSoFar]; // move the end to the start
		i++;
		usedSoFar++;
	}
	for ( /* nothing */ ; i < CONSOLE_COMMAND_MAX_LENGTH ; i++)
	{
		receiveBuffer[i] =  NULL_CHAR;
	}
	return remaining;
}
