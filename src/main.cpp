/*
 * NOVO:
 * testa quantos pacotes consegue enviar sem travar, NÃO USA USB
 * RECENTE:
 * Em vez de se basear em 3 funções diferentes, usa apenas um comando de SPI::CMD()
 * Foi resolvido o problema de esporadicamente repetir o 4º caracter no 5º
 * Para isso, foi alterada a verificação de SPI_I2S_FLAGS antes de setar CS em SPI::CMD()
 * Nesse projeto mantém-se a configuração de SPI divida em duas partes:
 * 		uma parte fica no construtor da classe SPI e faz a inicialização do SCK,MOSI e MISO
 * 		a outra parte fica no construtor da classe NRF, e inicializa o CSN
 * o pino VDD do NRF fica ligado direto no VDD da discovery, como na versão USB+TX_sem_SPI_CONFIG_WORKED_3
 * a função NRF::begin() já é chamada dentro do construtor do nRF
 *
 * Author: Renan Pícoli
 */

/* Includes */
#include "main.h"

#include "NRF24.h"
#include "CONFIG.h"
#include "SPI_interface.h"

uint32_t TimingDelay;

//void Delay (uint32_t nTime);
//void TimingDelay_Decrement(void);

extern "C" {
 void SysTick_Handler(void);
 void OTG_FS_IRQHandler(void);
 void OTG_FS_WKUP_IRQHandler(void);
}

/*
 * The USB data must be 4 byte aligned if DMA is enabled. This macro handles
 * the alignment, if necessary (it's actually magic, but don't tell anyone).
 * See: http://electronics.stackexchange.com/questions/134474/how-to-get-usart-and-usb-serial-working-on-the-stm32f4discovery-board
 * See also usb_conf.h
 */
__ALIGN_BEGIN USB_OTG_CORE_HANDLE  USB_OTG_dev __ALIGN_END;

/**
**===========================================================================
**
**  Abstract: main program
**
**===========================================================================
*/

//envia sempre os mesmos pacotes
uint8_t send(NRF* radio_ptr);

int main(void)
{
	SysTick_Config(SystemCoreClock/1000000);
  /**
  *  IMPORTANT NOTE!
  *  The symbol VECT_TAB_SRAM needs to be defined when building the project
  *  if code has been located to RAM and interrupts are used. 
  *  Otherwise the interrupt table located in flash will be used.
  *  See also the <system_*.c> file and how the SystemInit() function updates 
  *  SCB->VTOR register.  
  *  E.g.  SCB->VTOR = 0x20000000;  
  */

  STM_EVAL_LEDInit(LED3);
  STM_EVAL_LEDInit(LED4);
  STM_EVAL_LEDInit(LED5);
  STM_EVAL_LEDInit(LED6);

  NRF radio;//inicializa o NRF com os pinos default, deixa em POWER_UP

  radio.REFRESH();//TODO remover após debug

  radio.TX_configure();//faz uma configuração default para o modo TX
  radio.REFRESH();//TODO remover após debug

  //inicialização do USB
  USBD_Init(&USB_OTG_dev, USB_OTG_FS_CORE_ID, &USR_desc, &USBD_CDC_cb, &USR_cb);

  uint64_t packets_sent=0;
  /* Infinite loop */
  while (1)
  {
			if(send(&radio)){
					packets_sent++;
					STM_EVAL_LEDToggle(LED6);//AZUL:indicador de sucesso
					//Delay_ms(05);
			}
			else{
					STM_EVAL_LEDToggle(LED5);//VERMELHO:indicador de falha
			}
  }
}

uint8_t send(NRF* radio_ptr){
	  uint8_t symbol;
	  uint8_t i;
	  uint8_t  buffer[]={'a',43,43,43,43};

	  uint8_t  ack_payload[]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

	  	 //TODO REMOVER PARA VOLTAR A EXECUTAR O LOOP
		if(radio_ptr->SEND(buffer)){
				return 1;//indicador de sucesso
#ifdef USE_AUTOACK
				//TODO: include verificação de chegada de mensagem; imprimir ack payload
/*				while(radio_ptr->RECEIVE(ack_payload));//espera até que tenha chegado algo na RX-fifo
				VCP_send_buffer(ack_payload,5);*/
#endif
		}
		else{
				return 0;//indicador de falha
		}

	  while (0)//TODO REMOVER o ZERO PARA VOLTAR A EXECUTAR O LOOP
	  {
		if(VCP_get_char(&symbol))//se RECEBE um char a partir do computador
		{
			//verifica se o símbolo recebido indica nova sequência de velocidades,
			//caso afirmativo, os próximos 4 uint8_t são tratados como velocidades e são enviados
			if(symbol == 'a'){//TODO: change to 0xff
			start_of_buffering://start of buffering
				i=0;
				buffer[i]='a';//TODO: change to 0xff
				while (i<4){
					if(VCP_get_char(&symbol)){
						if(symbol == 'a')//TODO: change to 0xff
							goto start_of_buffering;
						buffer[i+1] = symbol;//preenche o buffer
						i++;
					}
				}
				VCP_send_buffer(buffer,5);//TODO remover após debug
				if(radio_ptr->SEND(buffer)){
						return 1;//indicador de sucesso
				}
				else{
						return 0;//indicador de falha
				}
			}
		}
	  }
}

/*void Delay( uint32_t nTime)
{
  TimingDelay = nTime;

  while(TimingDelay != 0); //gustavo
}*/

/*
void TimingDelay_Decrement(void)
{
  if (TimingDelay != 0x00)
  {
    TimingDelay--;
  }
}
extern "C" {
void SysTick_Handler(void)
{
  TimingDelay_Decrement();
}
}
*/

void OTG_FS_IRQHandler(void)
{
  USBD_OTG_ISR_Handler (&USB_OTG_dev);
}

void OTG_FS_WKUP_IRQHandler(void)
{
  if(USB_OTG_dev.cfg.low_power)
  {
    *(uint32_t *)(0xE000ED10) &= 0xFFFFFFF9 ;
    SystemInit();
    USB_OTG_UngateClock(&USB_OTG_dev);
  }
  EXTI_ClearITPendingBit(EXTI_Line18);
}

/*
 * Callback used by stm32f4_discovery_audio_codec.c.
 * Refer to stm32f4_discovery_audio_codec.h for more info.
 */
extern "C" void EVAL_AUDIO_TransferComplete_CallBack(uint32_t pBuffer, uint32_t Size){
  /* TODO, implement your code here */
  return;
}

/*
 * Callback used by stm324xg_eval_audio_codec.c.
 * Refer to stm324xg_eval_audio_codec.h for more info.
 */
extern "C" uint16_t EVAL_AUDIO_GetSampleCallBack(void){
  /* TODO, implement your code here */
  return -1;
}
