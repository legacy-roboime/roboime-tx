/*
 * my_spi.cpp
 *
 *  Created on: Aug 17, 2016
 *      Author: lenovoi7
 */
#include "my_spi.h"

#define SPI_TIMEOUT      0xFFFFFFFF

void MySpi_Init(MySpi *these){
  GPIO_Init(&these->CS_Gpio, GPIOA, GPIO_PIN_4);

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
  these->SpiHandle.Instance               = SPI1;
  these->SpiHandle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
  these->SpiHandle.Init.Direction         = SPI_DIRECTION_2LINES;
  these->SpiHandle.Init.CLKPhase          = SPI_PHASE_1EDGE;
  these->SpiHandle.Init.CLKPolarity       = SPI_POLARITY_LOW;
  these->SpiHandle.Init.CRCCalculation    = SPI_CRCCALCULATION_DISABLE;
  these->SpiHandle.Init.CRCPolynomial     = 7;
  these->SpiHandle.Init.DataSize          = SPI_DATASIZE_8BIT;
  these->SpiHandle.Init.FirstBit          = SPI_FIRSTBIT_MSB;
  these->SpiHandle.Init.NSS               = SPI_NSS_SOFT;
  these->SpiHandle.Init.TIMode            = SPI_TIMODE_DISABLE;
  these->SpiHandle.Init.Mode = SPI_MODE_MASTER;
  HAL_SPI_Init(&these->SpiHandle);
  __HAL_SPI_ENABLE(&these->SpiHandle);
  GPIO_Set(&these->CS_Gpio);
 }
int MySpi_Transfer(MySpi *these, uint8_t *Data_IN, uint8_t *Data_OUT, uint16_t size){
  int16_t Spi_TimeOut=0x400;
  GPIO_Reset(&these->CS_Gpio);
  (* (uint8_t *)(Data_IN))= *(__IO uint8_t *)&these->SpiHandle.Instance->DR;

  while((__HAL_SPI_GET_FLAG(&these->SpiHandle, SPI_FLAG_TXE)==0)&&(Spi_TimeOut>0)){
	Spi_TimeOut--;
  }
  Spi_TimeOut=0x400;
  int i;
  for(i=0; i<size; i++){
	these->SpiHandle.Instance->DR = *((uint16_t *)(Data_OUT+i));
	while((__HAL_SPI_GET_FLAG(&these->SpiHandle, SPI_FLAG_TXE)==0)&&(Spi_TimeOut>0)){
	  Spi_TimeOut--;
	}
	Spi_TimeOut=0x400;
	while((__HAL_SPI_GET_FLAG(&these->SpiHandle, SPI_FLAG_BSY)==1)&&(Spi_TimeOut>0)){
	  Spi_TimeOut--;
	}
	Spi_TimeOut=0x400;

	while((__HAL_SPI_GET_FLAG(&these->SpiHandle, SPI_FLAG_RXNE)==0)&&(Spi_TimeOut>0)){
	  Spi_TimeOut--;
	}
	Spi_TimeOut=0x400;
	(* (uint8_t *)(Data_IN+i))= *(__IO uint8_t *)&these->SpiHandle.Instance->DR;
  }
  while((__HAL_SPI_GET_FLAG(&these->SpiHandle, SPI_FLAG_BSY)==1)&&(Spi_TimeOut>0)){
    Spi_TimeOut--;
  }
  Spi_TimeOut=0x400;
  while((__HAL_SPI_GET_FLAG(&these->SpiHandle, SPI_FLAG_TXE))&&(Spi_TimeOut>0)){
    Spi_TimeOut--;
  }
  Spi_TimeOut=0x400;
  GPIO_Set(&these->CS_Gpio);
  return 1;
}
