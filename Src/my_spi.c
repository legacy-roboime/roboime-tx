/*
 * my_spi.cpp
 *
 *  Created on: Aug 17, 2016
 *      Author: lenovoi7
 */
#include "my_spi.h"

#define SPI_TIMEOUT      0xFFFFFFFF

void MySpi_Init(MySpi *this){
  GPIO_Init(&this->CS_Gpio, GPIOA, GPIO_PIN_4);

  //MOSI, MISO, SCK GPIO configuration
  __HAL_RCC_GPIOA_CLK_ENABLE();
  GPIO_InitTypeDef GPIO_SPI_Pins_initstruct;
  GPIO_SPI_Pins_initstruct.Mode	= GPIO_MODE_AF_PP;
  GPIO_SPI_Pins_initstruct.Pull	= GPIO_NOPULL;
  GPIO_SPI_Pins_initstruct.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_SPI_Pins_initstruct.Pin = GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7;
  GPIO_SPI_Pins_initstruct.Alternate = GPIO_AF5_SPI1;
  HAL_GPIO_Init(GPIOA, &GPIO_SPI_Pins_initstruct);

  __HAL_RCC_SPI1_CLK_ENABLE();
  this->SpiHandle.Instance               = SPI1;
  this->SpiHandle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
  this->SpiHandle.Init.Direction         = SPI_DIRECTION_2LINES;
  this->SpiHandle.Init.CLKPhase          = SPI_PHASE_1EDGE;
  this->SpiHandle.Init.CLKPolarity       = SPI_POLARITY_LOW;
  this->SpiHandle.Init.CRCCalculation    = SPI_CRCCALCULATION_DISABLE;
  this->SpiHandle.Init.CRCPolynomial     = 7;
  this->SpiHandle.Init.DataSize          = SPI_DATASIZE_8BIT;
  this->SpiHandle.Init.FirstBit          = SPI_FIRSTBIT_MSB;
  this->SpiHandle.Init.NSS               = SPI_NSS_SOFT;
  this->SpiHandle.Init.TIMode            = SPI_TIMODE_DISABLE;
  this->SpiHandle.Init.Mode = SPI_MODE_MASTER;
  HAL_SPI_Init(&this->SpiHandle);
  __HAL_SPI_ENABLE(&this->SpiHandle);
  GPIO_Set(&this->CS_Gpio);
 }
int MySpi_Transfer(MySpi *this, uint8_t *Data_IN, uint8_t *Data_OUT, uint16_t size){
  int16_t Spi_TimeOut=0x400;
  GPIO_Reset(&this->CS_Gpio);
  (* (uint8_t *)(Data_IN))= *(__IO uint8_t *)&this->SpiHandle.Instance->DR;

  while((__HAL_SPI_GET_FLAG(&this->SpiHandle, SPI_FLAG_TXE)==0)&&(Spi_TimeOut>0)){
	Spi_TimeOut--;
  }
  Spi_TimeOut=0x400;
  int i;
  for(i=0; i<size; i++){
	this->SpiHandle.Instance->DR = *((uint16_t *)(Data_OUT+i));
	while((__HAL_SPI_GET_FLAG(&this->SpiHandle, SPI_FLAG_TXE)==0)&&(Spi_TimeOut>0)){
	  Spi_TimeOut--;
	}
	Spi_TimeOut=0x400;
	while((__HAL_SPI_GET_FLAG(&this->SpiHandle, SPI_FLAG_BSY)==1)&&(Spi_TimeOut>0)){
	  Spi_TimeOut--;
	}
	Spi_TimeOut=0x400;

	while((__HAL_SPI_GET_FLAG(&this->SpiHandle, SPI_FLAG_RXNE)==0)&&(Spi_TimeOut>0)){
	  Spi_TimeOut--;
	}
	Spi_TimeOut=0x400;
	(* (uint8_t *)(Data_IN+i))= *(__IO uint8_t *)&this->SpiHandle.Instance->DR;
  }
  while((__HAL_SPI_GET_FLAG(&this->SpiHandle, SPI_FLAG_BSY)==1)&&(Spi_TimeOut>0)){
    Spi_TimeOut--;
  }
  Spi_TimeOut=0x400;
  while((__HAL_SPI_GET_FLAG(&this->SpiHandle, SPI_FLAG_TXE))&&(Spi_TimeOut>0)){
    Spi_TimeOut--;
  }
  Spi_TimeOut=0x400;
  GPIO_Set(&this->CS_Gpio);
  return 1;
}
MySpi_Error(){

}
