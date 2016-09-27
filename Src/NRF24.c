/*
 * NRF24.cpp
 *
 *  Created on: Aug 17, 2016
 *      Author: lenovoi7
 */

#include <NRF24.h>

uint8_t addressRoboList[] = {0XC3, 0XC4, 0XC5, 0XC6, 0XC7, 0XC8, 0XC9,
		0XCA, 0XCB, 0XB3, 0XB4, 0XB5, 0XB6, 0XB7, 0XB8, 0XB9};

void Nrf24_Init(Nrf24 *these) {
  MySpi_Init(&these->NRF_Spi);
  GPIO_Init(&these->NRF_CE, GPIOA, GPIO_PIN_3);
  GPIO_Reset(&these->NRF_CE);
}
int Nrf24_Config(Nrf24 *these){
  uint8_t reg_value;
  if(these->is_rx)
    reg_value=0b00001011;
  else
	reg_value=0b00001010;
  Nrf24_WriteRegister(these, 0x00, &reg_value, 1);
  reg_value=0b11110011;
  Nrf24_WriteRegister(these, 0x04, &reg_value, 1);
  reg_value=0b00000111;
  Nrf24_WriteRegister(these, 0x05, &reg_value, 1);
  reg_value=0b00000011;
  Nrf24_WriteRegister(these, 0x1c, &reg_value, 1);
  reg_value=0b00000110;
  Nrf24_WriteRegister(these, 0x1d, &reg_value, 1);
  Nrf24_FlushRx(these);
  Nrf24_FlushTx(these);
  reg_value=0b01110000;
  Nrf24_WriteRegister(these, (uint8_t) 0x07, &reg_value, 1);
  int i;
  for(i=0;i<0xeeeee2;i++);
  return 1;
}
int Nrf24_WriteRegister(Nrf24 *these, uint8_t adress, uint8_t *new_value, int size){
  uint8_t buf_in[size+1];
  uint8_t buf_out[size+1];
  buf_out[0]=0x20|adress;
  int i;
  for(i=1; i<size+1; i++){
    buf_out[i]=new_value[i-1];
  }
  if(!MySpi_Transfer(&these->NRF_Spi, buf_in, buf_out, (int) size+1))
    return 0;
  //for(int i=0;i<0xee2;i++);
  return 1;
}

int Nrf24_ReadRegister(Nrf24 *these, uint8_t adress, uint8_t *value, int size){
  uint8_t buf_in[size+1];
  uint8_t buf_out[size+1];
  buf_out[0]=adress;
  if(!MySpi_Transfer(&these->NRF_Spi, buf_in, buf_out, (int) size+1))
	return 0;
  int i;
  for(i=0; i<size; i++){
    value[i]=buf_in[i+1];
  }
  return 1;
}
int Nrf24_WritePayload(Nrf24 *these, uint8_t *data, int size){
  uint8_t buf_in[size+1];
  uint8_t buf_out[size+1];
  buf_out[0]=0b10100000;
  int i;
  for(i=1; i<size+1; i++){
	buf_out[i]=data[i-1];
  }
  if(!MySpi_Transfer(&these->NRF_Spi, buf_in, buf_out, (int) size+1))
	return 0;
  return 1;
}
int Nrf24_ReadPayload(Nrf24 *these, uint8_t *data, int size){
  uint8_t buf_in[size+1];
  uint8_t buf_out[size+1];
  buf_out[0]=0b01100001;
  if(!MySpi_Transfer(&these->NRF_Spi, buf_in, buf_out, (int) size+1))
	return 0;
  int i;
  for(i=0; i<size; i++){
    data[i]=buf_in[i+1];
  }
  return 1;
}
int Nrf24_FlushTx(Nrf24 *these){
  uint8_t buf_in[1];
  uint8_t buf_out[1];
  buf_out[0]=0b11100001;
  if(!MySpi_Transfer(&these->NRF_Spi, buf_in, buf_out, (int) 1))
	return 0;
  int i;
  for(i=0;i<0xee2;i++);
  return 1;
}
int Nrf24_FlushRx(Nrf24 *these){
  uint8_t buf_in[1];
  uint8_t buf_out[1];
  buf_out[0]=0b11100010;
  if(!(MySpi_Transfer(&these->NRF_Spi, buf_in, buf_out, (int) 1)))
	return 0;
  int i;
  for(i=0;i<0xee2;i++);
  return 1;
}
int Nrf24_WriteAckPayload(Nrf24 *these, uint8_t *data, int size){
  uint8_t buf_in[size+1];
  uint8_t buf_out[size+1];
  buf_out[0]=0b10101000;
  int i;
  for(i=1; i<size+1; i++){
	buf_out[i]=data[i-1];
  }
  if(!MySpi_Transfer(&these->NRF_Spi, buf_in, buf_out, (int) size+1))
	return 0;
  return 1;
}
int Nrf24_CheckPayloadWidth(Nrf24 *these){
  uint8_t buf_in[2];
  uint8_t buf_out[2];
  buf_out[0]=0b01100000;
  if(!MySpi_Transfer(&these->NRF_Spi, buf_in, buf_out, (int) 2))
	return 0;
  return buf_in[1];
}
int Nrf24_SetId(Nrf24 *these, uint8_t roboId){
  uint8_t roboAddress[5];
  int i;
  for(i=0; i<5; i++){
    roboAddress[i]=addressRoboList[roboId];
  }
  Nrf24_WriteRegister(these, 0x0A, roboAddress, 5);
  Nrf24_WriteRegister(these, 0x10, roboAddress, 5);
  return 0;
}

int Nrf24_Test(Nrf24 *these){
  uint8_t new_value=0x57;
  uint8_t value=0;
  Nrf24_WriteRegister(these, 0x0F, &new_value, 1);
  Nrf24_ReadRegister(these, 0x0F, &value, 1);
  return (value==new_value);
}
int Nrf24_DataSent(Nrf24 *these){
  uint8_t value=0;
  Nrf24_ReadRegister(these, REG_STATUS, &value, 1);
  return (value&0b00100000);
}
int Nrf24_DataReady(Nrf24 *these){
  uint8_t value=0;
  Nrf24_ReadRegister(these, REG_STATUS, &value, 1);
  return (value&0b01000000);
}
int Nrf24_MaxRt(Nrf24 *these){
  uint8_t value=0;
  Nrf24_ReadRegister(these, REG_STATUS, &value, 1);
  return (value&0b00010000);
}
int Nrf24_CleanDataSent(Nrf24 *these){
  uint8_t new_value=0b00100000;
  return Nrf24_WriteRegister(these, REG_STATUS, &new_value, 1);
}
int Nrf24_CleanDataReady(Nrf24 *these){
  uint8_t new_value=0b01000000;
  return Nrf24_WriteRegister(these, REG_STATUS, &new_value, 1);
}
int Nrf24_CleanMaxRt(Nrf24 *these){
  uint8_t new_value=0b00010000;
  return Nrf24_WriteRegister(these, REG_STATUS, &new_value, 1);
}
int Nrf24_RxEmpty(Nrf24 *these){
  uint8_t value=0;
  Nrf24_ReadRegister(these, REG_FIFO_STATUS, &value, 1);
  return (value&0b00000001);
}
int Nrf24_RxFull(Nrf24 *these){
  uint8_t value=0;
  Nrf24_ReadRegister(these, REG_FIFO_STATUS, &value, 1);
  return (value&0b00000010);
}
int Nrf24_TxEmpty(Nrf24 *these){
  uint8_t value=0;
  Nrf24_ReadRegister(these, REG_FIFO_STATUS, &value, 1);
  return (value&0b00010000);
}
int Nrf24_TxFull(Nrf24 *these){
  uint8_t value=0;
  Nrf24_ReadRegister(these, REG_FIFO_STATUS, &value, 1);
  return (value&0b00100000);
}
