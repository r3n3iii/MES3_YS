/*
 * led.h
 *1
 *  Created on: Sep 16, 2022
 *      Author: Rene
 */

#ifndef INC_LED_H_
#define INC_LED_H_

#include "stdint.h"
#include "main.h"

#define LED_GREEN		GPIO_PIN_12
#define LED_ORANGE		GPIO_PIN_13
#define LED_RED			GPIO_PIN_14
#define LED_BLUE		GPIO_PIN_15

// function prototypes
void led_Init(void);
void led_Toggle(uint16_t GPIO_Pin);
void led_Write(uint16_t GPIO_Pin, GPIO_PinState PinState);


#endif /* INC_LED_H_ */
