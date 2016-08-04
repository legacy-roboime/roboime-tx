/*
 * CONFIG.h
 *
 *  Created on: Nov 16, 2015
 *      Author: lenovoi7
 */
#include "stm32f4xx.h"
#include "stm32f4_discovery.h"
#include "stm32f4xx_gpio.h"

#ifndef CONFIG_H_
#define CONFIG_H_

void TimingDelay_Decrement(void);
void Delay_ms(uint32_t time);
void Delay_s(unsigned char s);

int GPIO_Clock_Cmd(GPIO_TypeDef* GPIOx, FunctionalState STATE);
int SPI_Clock_Cmd(SPI_TypeDef* SPIx, FunctionalState STATE);
uint16_t GPIO_PinSource(uint16_t GPIO_Pin);
uint8_t GPIO_AF_SPIx(SPI_TypeDef* SPIx);

void CONFIG_LED();

#endif /* CONFIG_H_ */
