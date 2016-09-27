/*
 * GPIO.cpp
 *
 *  Created on: Mar 12, 2016
 *      Author: lenovoi7
 */

#include "stm32f4xx_hal.h"
#include "GPIO.h"

void GPIO_Init(GPIO *thisee, GPIO_TypeDef* Port, uint16_t Pin) {
	if(Port == GPIOA)
		__HAL_RCC_GPIOA_CLK_ENABLE();
	if(Port == GPIOB)
		__HAL_RCC_GPIOB_CLK_ENABLE();
	if(Port == GPIOC)
		__HAL_RCC_GPIOC_CLK_ENABLE();
	if(Port == GPIOD)
		__HAL_RCC_GPIOD_CLK_ENABLE();
	if(Port == GPIOE)
		__HAL_RCC_GPIOE_CLK_ENABLE();
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStructure.Pin = Pin;
	GPIO_InitStructure.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(Port,&GPIO_InitStructure);

	HAL_GPIO_WritePin(Port, Pin, GPIO_PIN_RESET);
	thisee->GPIO_Port = Port;
	thisee->GPIO_Pin = Pin;
}
uint8_t GPIO_Status(GPIO *thisee){
  uint8_t status_bit = HAL_GPIO_ReadPin(thisee->GPIO_Port, thisee->GPIO_Pin);
  if(status_bit==GPIO_PIN_SET)
    return 1;
  return 0;
}
void GPIO_Set(GPIO *thise){
	HAL_GPIO_WritePin(thise->GPIO_Port, thise->GPIO_Pin, GPIO_PIN_SET);
	return;
}
void GPIO_Reset(GPIO *thise){
	HAL_GPIO_WritePin(thise->GPIO_Port, thise->GPIO_Pin, GPIO_PIN_RESET);
	return;
}
void GPIO_Toggle(GPIO *thise){
	if(GPIO_Status(thise)){
	  GPIO_Reset(thise);
	}
	else{
	  GPIO_Set(thise);
	}
}

