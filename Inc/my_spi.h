#ifndef MY_SPI
#define MY_SPI
#include "stm32f4xx_hal.h"
#include "GPIO.h"
#ifdef __cplusplus
 extern "C" {
#endif

typedef struct MySpi{
  GPIO CS_Gpio;
  SPI_HandleTypeDef SpiHandle;
}MySpi;
  void MySpi_Init(MySpi *these);
  int MySpi_Transfer(MySpi *these, uint8_t *Data_IN, uint8_t *Data_OUT, uint16_t size);
#endif

#ifdef __cplusplus
 }
#endif
