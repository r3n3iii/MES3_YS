/*
 * myUart.c
 *
 *  Created on: Jan 16, 2020
 *      Author: Rene
 */

/* DriverLib Includes */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

#include <xdc/runtime/Log.h>                                                    // For any Log_info() call
#include <xdc/runtime/System.h>

/* Standard Includes */
#include <stdint.h>
#include <string.h>
//***** Header Files **********************************************************
                                                          // Standard integer types
#include <stddef.h>                                       // Standard variable types and macros
#include <stdio.h>                                        // For printf()

// TI-RTOS Kernel Header Files
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/drivers/UART.h>
#include "ti_drivers_config.h"
#include <ti/drivers/Board.h>

UART_Handle uart0;
UART_Handle uart1;

void UART00_IRQHandler(UART_Handle handle, void *buffer, size_t num);
void UART01_IRQHandler(UART_Handle handle, void *buffer, size_t num);

void myUart_init()
{
    UART_init();

    UART_Params uartParamsA0;
    UART_Params uartParamsA1;
    //UART_Params uartParamsA2;
    // Create a UART with data processing off.
    UART_Params_init(&uartParamsA0);
    uartParamsA0.writeDataMode = UART_DATA_BINARY;
    uartParamsA0.readDataMode = UART_DATA_BINARY;
    uartParamsA0.readReturnMode = UART_RETURN_FULL;
    uartParamsA0.readEcho = UART_ECHO_OFF;
    uartParamsA0.baudRate = 115200;
    uartParamsA0.stopBits = UART_STOP_ONE;
     uartParamsA0.parityType = UART_PAR_NONE;
     uartParamsA0.dataLength = UART_LEN_8;
    uartParamsA0.readMode = UART_MODE_CALLBACK; // the uart uses a read interrupt
    uartParamsA0.readCallback = &UART00_IRQHandler; // function called when the uart interrupt fires
    uart0 = UART_open(CONFIG_UART_0, &uartParamsA0);

    UART_Params_init(&uartParamsA1);
    uartParamsA1.writeDataMode = UART_DATA_BINARY;
    uartParamsA1.readDataMode = UART_DATA_BINARY;
    uartParamsA1.readReturnMode = UART_RETURN_FULL;
    uartParamsA1.readEcho = UART_ECHO_OFF;
    uartParamsA1.baudRate = 9600;
    uartParamsA1.readMode =UART_MODE_BLOCKING; // the uart uses a read interrupt
    uartParamsA1.stopBits = UART_STOP_ONE;
     uartParamsA1.parityType = UART_PAR_NONE;
     uartParamsA1.dataLength = UART_LEN_8;
    //uartParamsA1.readCallback = &UART01_IRQHandler; // function called when the uart interrupt fires
    uart1 = UART_open(CONFIG_UART_1, &uartParamsA1);


    if (uart0 == NULL) {
      System_abort("Error opening the UART0");
    }
    if (uart1 == NULL) {
      System_abort("Error opening the UART1");
    }
    // while (1) {
          // UART_read(uart, &input, 1); // prime the uart bus to read the first character, non blocking
        //   Semaphore_pend(SEM_uart_rx, BIOS_WAIT_FOREVER); // when a character is received via UART, the interrupt handler will release the binary semaphore
           // in my case: I get an interrupt for a single character, no need to loop.

          // GPIO_toggle(Board_LED1); // LED B - visual clue that we've received a request over USB

           //UART_write(uart, &input, 1);  // as a test, let's just echo the char

         //  GPIO_toggle(Board_LED1); // LED B - visual clue off
    //   }

   // printf("UART has been initialized\n");
}
void UART00_IRQHandler(UART_Handle handle, void *buffer, size_t num)
{
    //Semaphore_post(SEM_uart_rx);
}
void UART01_IRQHandler(UART_Handle uart1, void *buffer, size_t num)
{
   // Semaphore_post(SEM_uart_rx);
}

