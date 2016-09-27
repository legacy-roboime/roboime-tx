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
void GPIO_Init(GPIO *thise, GPIO_TypeDef* Port, uint16_t Pin);
void GPIO_Set(GPIO *thise);
void GPIO_Reset(GPIO *thise);
void GPIO_Toggle(GPIO *thise);
uint8_t GPIO_Status(GPIO *thise);

#endif /* GPIO_H_ */
