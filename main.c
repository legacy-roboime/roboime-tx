#include "defines.h"
#include "stm32f4xx.h"

#include "tm_stm32f4_nrf24l01.h"
#include "tm_stm32f4_disco.h"
#include "tm_stm32f4_delay.h"
#include "tm_stm32f4_usart.h"
#include "tm_stm32f4_usb_vcp.h"
#include "tm_stm32f4_spi.h"
#include <stdio.h>


#define START_MSG_BYTE 0xFE
#define BASE_CHANNEL 109
#define TX_TIMEOUT_microsec 10

// My address
uint8_t MyAddress[] = {
    0xE7,
    0xE7,
    0xE7,
    0xE7,
    0xE7
};
// Receiver address
uint8_t TxAddress[] = {
    0x7E,
    0x7E,
    0x7E,
    0x7E,
    0x7E
};

uint8_t dataOut[32], dataIn[32];
uint32_t minimo_delay_botao, tempo_antes, tempo_agora, canal, temp_var, timeElapsedTX;
char apertou = 0x00;

static uint8_t pacote[32];
static uint8_t indexPacote = 0;
const uint32_t timeOutTX = TX_TIMEOUT_microsec;


int main(void) {
	uint8_t c;
	TM_NRF24L01_Transmit_Status_t transmissionStatus;
	tempo_antes = 0;
	tempo_agora = 0;
	minimo_delay_botao = 5000;
	canal = BASE_CHANNEL;
	
	SystemInit();
	TM_DELAY_Init();
	TM_DISCO_LedInit();
	TM_DISCO_ButtonInit();
	TM_USB_VCP_Init();
	
	
  
	// Channel config:
	TM_DISCO_LedOn(LED_BLUE);
	TM_DELAY_SetTime(0);
	
	while(TM_DELAY_Time() < 5000000) {
		if(TM_DISCO_ButtonPressed()) {
			tempo_antes = tempo_agora;
			tempo_agora = TM_DELAY_Time();
			if((tempo_agora - tempo_antes)>800000) {
				TM_DISCO_LedOn(LED_RED);
				Delayms(500);
				TM_DISCO_LedOff(LED_RED);
				canal++;
			}
		}
	}
	TM_DISCO_LedOff(LED_BLUE);
	Delayms(500);
	TM_DISCO_LedOn(LED_BLUE);
  Delayms(1200);
	TM_DISCO_LedOff(LED_BLUE);
	Delayms(500);
	
	// Blinking the number of increments to the channel
	for(temp_var = BASE_CHANNEL; temp_var < canal; temp_var++) {
		TM_DISCO_LedOn(LED_RED);
		Delayms(500);
		TM_DISCO_LedOff(LED_RED);
		Delayms(500);
	}
		
	TM_NRF24L01_Init(canal, 32);
	TM_NRF24L01_SetRF(TM_NRF24L01_DataRate_2M, TM_NRF24L01_OutputPower_M18dBm);

	TM_NRF24L01_SetMyAddress(MyAddress);
	TM_NRF24L01_SetTxAddress(TxAddress);
	
	while (1) {
		// USB and drivers are OK!
		if (TM_USB_VCP_GetStatus() == TM_USB_VCP_CONNECTED) {
			TM_DISCO_LedOn(LED_GREEN);
			TM_DISCO_LedOff(LED_RED);
			
			
			if (TM_USB_VCP_Getc(&c) == TM_USB_VCP_DATA_OK) {
				TM_DISCO_LedOn(LED_ORANGE);
				if(c == START_MSG_BYTE) {
					indexPacote = 0;
				}
				pacote[indexPacote] = c;
				indexPacote++;
				TM_DISCO_LedOff(LED_ORANGE);
				
				if(indexPacote == 32) {				
					TM_NRF24L01_Transmit(pacote);
					indexPacote = 0;
					 
					TM_DELAY_SetTime(0);
					
					
					//Wait for data to be sent
					do {
						transmissionStatus = TM_NRF24L01_GetTransmissionStatus();
						
						if(transmissionStatus == TM_NRF24L01_Transmit_Status_Lost) {
							TM_DISCO_LedOn(LED_RED);
						} else if(transmissionStatus == TM_NRF24L01_Transmit_Status_Sending) {
							TM_DISCO_LedOn(LED_ORANGE);
						} else if(transmissionStatus ==  TM_NRF24L01_Transmit_Status_Ok) {
							TM_DISCO_LedOn(LED_BLUE);
						}
						TM_DISCO_LedOff(LED_RED);
						TM_DISCO_LedOff(LED_ORANGE);
						TM_DISCO_LedOff(LED_BLUE);
					//#ifndef HORTA_SEM_ACK
					//} while ((transmissionStatus == TM_NRF24L01_Transmit_Status_Sending) && (TM_DELAY_Time() <= timeOutTX));
					//#elif
				  } while (transmissionStatus == TM_NRF24L01_Transmit_Status_Sending );
					//#endif
					
					//TODO: it just doesn't show up
					if((transmissionStatus == TM_NRF24L01_Transmit_Status_Sending) && (TM_DELAY_Time() > timeOutTX)) {
						//houve perda por atraso!!
						TM_DISCO_LedOn(LED_RED);
						TM_DISCO_LedOff(LED_RED);
					}
				}
			}
		} else {
			// No USB
			TM_DISCO_LedOff(LED_GREEN);
			TM_DISCO_LedOn(LED_RED);
		}
	}
} 