/*
 * consoleIo.h
 *
 *  Created on: Oct. 1, 2022
 *      Author: Rene
 */

#ifndef INC_CONSOLEIO_H_
#define INC_CONSOLEIO_H_

#include <stdint.h>
#include "main.h"

typedef enum {CONSOLE_SUCCESS = 0u, CONSOLE_ERROR = 1u } eConsoleError;



eConsoleError ConsoleIoInit(void);

eConsoleError ConsoleIoReceive(uint8_t *buffer, const uint32_t bufferLength, uint32_t *readLength);
eConsoleError ConsoleIoSendString(const char *buffer); // must be null terminated
void UartError_handler(void);

UART_HandleTypeDef huart2;


#endif /* INC_CONSOLEIO_H_ */
