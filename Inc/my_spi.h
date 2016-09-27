#ifndef MY_SPI
#define MY_SPI
#include "stm32f4xx_hal.h"
#include "GPIO.h"
typedef struct MySpi{
  GPIO CS_Gpio;
  SPI_HandleTypeDef SpiHandle;
}MySpi;
  void MySpi_Init(MySpi *this);
  int MySpi_Transfer(MySpi *this, uint8_t *Data_IN, uint8_t *Data_OUT, uint16_t size);
#endif
