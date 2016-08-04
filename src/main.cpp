/*
 * NOVO:
 * Em vez de se basear em 3 funções diferentes, usa apenas um comando de SPI::CMD()
 * Foi resolvido o problema de esporadicamente repetir o 4º caracter no 5º
 * Para isso, foi alterada a verificação de SPI_I2S_FLAGS antes de setar CS em SPI::CMD()
 * RECENTE:
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

uint32_t TimingDelay;

//void Delay (uint32_t nTime);
//void TimingDelay_Decrement(void);

extern "C" {
 void SysTick_Handler(void);
 void OTG_FS_IRQHandler(void);
 void OTG_FS_WKUP_IRQHandler(void);
}


__ALIGN_BEGIN USB_OTG_CORE_HANDLE  USB_OTG_dev __ALIGN_END;


int main(void)
{
	SysTick_Config(SystemCoreClock/1000);
  /**
  *  IMPORTANT NOTE!
  *  The symbol VECT_TAB_SRAM needs to be defined when building the project
  *  if code has been located to RAM and interrupts are used. 
  *  Otherwise the interrupt table located in flash will be used.
  *  See also the <system_*.c> file and how the SystemInit() function updates 
  *  SCB->VTOR register.  
  *  E.g.  SCB->VTOR = 0x20000000;  
  */


  USBD_Init(&USB_OTG_dev, USB_OTG_FS_CORE_ID, &USR_desc, &USBD_CDC_cb, &USR_cb);

  /* Infinite loop */
  while (1)
  {
  }
}


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
