/*
 * GPIO.h
 *
 *  Created on: Mar 12, 2016
 *      Author: lenovoi7
 */
#include "stm32f4xx_hal.h"

#ifndef GPIO_H_
#define GPIO_H_
typedef struct GPIO{
  GPIO_TypeDef* GPIO_Port;
  uint16_t GPIO_Pin;
} GPIO;
void GPIO_Init(GPIO *this, GPIO_TypeDef* Port, uint16_t Pin);
void GPIO_Set(GPIO *this);
void GPIO_Reset(GPIO *this);
void GPIO_Toggle(GPIO *this);
uint8_t GPIO_Status(GPIO *this);

#endif /* GPIO_H_ */
