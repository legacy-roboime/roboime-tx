/*
 * CONFIG.cpp
 *
 *  Created on: Nov 16, 2015
 *      Author: lenovoi7
 */
#include "stm32f4xx.h"
#include "stm32f4_discovery.h"
#include "stm32f4xx_gpio.h"
#include "own_libraries/CONFIG.h"

static __IO uint32_t TimingDelay;//número de us que faltam

void TimingDelay_Decrement(void)
{
  if (TimingDelay != 0x00)
  {
    TimingDelay--;

  }
}

/*
 * Por que tive de adicionar extern "C" {} ?
 * ANS : o default handler systick_handler tem atributo weak,
 * logo pode ser "substituído" pelo systick_handler que eu definir (strong por padrão)
 * em C++ isso seria tratado como sobrecarga de função e seriam geradas 2 funções distintas
 * e não haveria substituição. É por isso que precisa do extern "C",
 * para que seja tratado como código C, gere  uma função de mesmo nome, etc.
 * e  por que dava errado se adicionasse no stm32f4xx_it.h
 */
extern "C"{
	void SysTick_Handler(void)
	{
		 TimingDelay_Decrement();
	}
}//fim do extern "C" {}

void Delay_micro_s(uint32_t time)
{
  TimingDelay = time;
  while(TimingDelay != 0);
}

void Delay_ms(uint32_t time)
{
  uint32_t us;
  us=time*1000;
  Delay_micro_s(us);
  /*TimingDelay = time;
  while(TimingDelay != 0);*/
}

void Delay_s(unsigned char s)
{
  uint32_t ms;
  ms=s*1000;
  Delay_ms(ms);
}

void CONFIG_LED() {
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	GPIO_InitTypeDef initstruct;
	initstruct.GPIO_Mode = GPIO_Mode_OUT;
	initstruct.GPIO_OType = GPIO_OType_PP;
	initstruct.GPIO_Pin = GPIO_Pin_12;
	initstruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	initstruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOD, &initstruct);
	GPIO_SetBits(GPIOD, GPIO_Pin_12);
}

/*
 * Enables or disables the peripheral clock associated with GPIOx
 */
int GPIO_Clock_Cmd(GPIO_TypeDef* GPIOx, FunctionalState STATE){
	if(!IS_GPIO_ALL_PERIPH(GPIOx)){
		return -1;
	}

	//ENABLE the clock of GPIOx
	if(GPIOx == GPIOA){
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, STATE);
	}else if(GPIOx == GPIOB){
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, STATE);
	}else if(GPIOx == GPIOC){
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, STATE);
	}else if(GPIOx == GPIOD){
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, STATE);
	}else if(GPIOx == GPIOE){
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, STATE);
	}else if(GPIOx == GPIOF){
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, STATE);
	}else if(GPIOx == GPIOG){
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, STATE);
	}else if(GPIOx == GPIOH){
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOH, STATE);
	}else if(GPIOx == GPIOI){
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOI, STATE);
	}
	return 0;
}

/*
 * Enables or disables the peripheral clock associated with SPIx
 */
int SPI_Clock_Cmd(SPI_TypeDef* SPIx, FunctionalState STATE){
	if(!IS_SPI_ALL_PERIPH(SPIx)){
		return -1;
	}

	if(SPIx == SPI1){
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1,STATE);
	}else if(SPIx == SPI2){
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2,STATE);
	}else if(SPIx == SPI3){
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3,STATE);
	}else if(SPIx == SPI4){
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI4,STATE);
	}else if(SPIx == SPI5){
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI5,STATE);
	}else if(SPIx == SPI6){
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI6,STATE);
	}

	return 0;
}

//retorna a constante GPIO_PinSource correspondente ao GPIO_Pin dado
uint16_t GPIO_PinSource(uint16_t GPIO_Pin){
	if(!IS_GPIO_PIN(GPIO_Pin)){
		return 0xFFFF;
	}

	if(GPIO_Pin == GPIO_Pin_0){
		return GPIO_PinSource0;
	}else if(GPIO_Pin == GPIO_Pin_1){
		return GPIO_PinSource1;
	}else if(GPIO_Pin == GPIO_Pin_2){
		return GPIO_PinSource2;
	}else if(GPIO_Pin == GPIO_Pin_3){
		return GPIO_PinSource3;
	}else if(GPIO_Pin == GPIO_Pin_4){
		return GPIO_PinSource4;
	}else if(GPIO_Pin == GPIO_Pin_5){
		return GPIO_PinSource5;
	}else if(GPIO_Pin == GPIO_Pin_6){
		return GPIO_PinSource6;
	}else if(GPIO_Pin == GPIO_Pin_7){
		return GPIO_PinSource7;
	}else if(GPIO_Pin == GPIO_Pin_8){
		return GPIO_PinSource8;
	}else if(GPIO_Pin == GPIO_Pin_9){
		return GPIO_PinSource9;
	}else if(GPIO_Pin == GPIO_Pin_10){
		return GPIO_PinSource10;
	}else if(GPIO_Pin == GPIO_Pin_11){
		return GPIO_PinSource11;
	}else if(GPIO_Pin == GPIO_Pin_12){
		return GPIO_PinSource12;
	}else if(GPIO_Pin == GPIO_Pin_13){
		return GPIO_PinSource13;
	}else if(GPIO_Pin == GPIO_Pin_14){
		return GPIO_PinSource14;
	}else if(GPIO_Pin == GPIO_Pin_15){
		return GPIO_PinSource15;
	}
}

uint8_t GPIO_AF_SPIx(SPI_TypeDef* SPIx){
	if(!IS_SPI_ALL_PERIPH(SPIx)){
		return 0xFF;
	}

	if(SPIx == SPI1){
		return GPIO_AF_SPI1;
	}else if(SPIx == SPI2){
		return GPIO_AF_SPI2;
	}else if(SPIx == SPI3){
		return GPIO_AF_SPI3;
	}else if(SPIx == SPI4){
		return GPIO_AF_SPI4;
	}else if(SPIx == SPI5){
		return GPIO_AF_SPI5;
	}else if(SPIx == SPI6){
		return GPIO_AF_SPI6;
	}
}
