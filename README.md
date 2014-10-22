RoboIME's Transmitter
=====================

This is the firmware of our transmitter. This repo comprehends a Keil uVision 5 Project, but can be easilly ported to other IDEs, such as CooCox CoIDE. The hardware is basically composed of an STM32F407VG Discovery board, with an nRF24L01 2.4GHz transceiver attached. The transmitter is capable of sending/receiving data to/from a computer via USB VCP. The nRF24L01 is connected to the board via SPI, and the IRQ signal is not being used.

How to's
--------
- *Connect my transceiver to the board*: all you have to do is follow the pinout defined by the library, no big deal. **Notice that the nRF24 transceiver supports only 3.3V!* 

- *Set channel value dinamically*: the firmware is configured to start at the 109th channel, but you can increment it as you go by clicking the User Button (blue button) while the blue LED is on. After that, the firmware responds by blinking the same amount of times the button was pressed.

- *Activate broadcast mode*: the nRF24L01 has the Enhanced Shockburst mode activated by default. In order to activate the broadcast mode, you need to deactivate the ACK messages from the receiver, as well as from the transmitter. If you want to activate broadcast mode, all you need to do is `#define HORTA_SEM_ACK`.

References and Libraries needed
-------------------------------
1. USB CDC DEVICE: distributed by STM
2. STM32F4-Discovery libraries by Tilen Majerle (available at http://stm32f4-discovery.com)
2.1 TM_STM32F4_disco
2.2 TM_STM32F4_usb_vcp 
2.3 TM_STM32F4_nrf24l01
2.4 TM_STM32F4_spi
2.5 TM_STM32F4_delay
2.6 TM_STM32F4_usart
 



ay   






