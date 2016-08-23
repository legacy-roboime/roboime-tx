/*
 * NRF24.C
 *
 *  Created on: Nov 16, 2015
 *      Author: lenovoi7
 */
#include "stm32f4xx.h"
#include "stm32f4_discovery.h"
#include "stm32f4xx_gpio.h"
#include "CONFIG.h"
#include "NRF24.h"

NRF::NRF(GPIO_TypeDef* CE_GPIO,uint16_t CE_Pin,
		GPIO_TypeDef* IRQ_GPIO,uint16_t IRQ_Pin,
		SPI_TypeDef* NRF_SPI,GPIO_TypeDef* NRF_CS_GPIO,uint16_t NRF_CS_Pin,
		GPIO_TypeDef* SCK_GPIO,uint16_t SCK_Pin,
		GPIO_TypeDef* MISO_GPIO,uint16_t MISO_Pin,
		GPIO_TypeDef* MOSI_GPIO,uint16_t MOSI_Pin):
	SPI(NRF_SPI,NRF_CS_GPIO,NRF_CS_Pin), NRF_REGISTER_MAP(){
	NRF_CE_GPIO = CE_GPIO;
	NRF_CE_Pin 	= CE_Pin;
	NRF_IRQ_GPIO= IRQ_GPIO;
	NRF_IRQ_Pin = IRQ_Pin;

	//ENABLE the clock of NRF_CE_GPIO
	GPIO_Clock_Cmd(NRF_CE_GPIO,ENABLE);

	//CE GPIO configuration
	GPIO_InitTypeDef GPIO_CE_initstruct;
	GPIO_CE_initstruct.GPIO_Mode	= GPIO_Mode_OUT;
	GPIO_CE_initstruct.GPIO_OType	= GPIO_OType_PP;
	GPIO_CE_initstruct.GPIO_Pin 	= NRF_CE_Pin;
	GPIO_CE_initstruct.GPIO_PuPd 	= GPIO_PuPd_NOPULL;
	GPIO_CE_initstruct.GPIO_Speed	= GPIO_Speed_50MHz;
	GPIO_Init(NRF_CE_GPIO, &GPIO_CE_initstruct);

	/*
	 * faz a inicializa��o do pino de IRQ
	 * configura como input, habilita a interrup��o,
	 * conecta ao EXTI
	 */

	//enables the SYSCFG clock
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG,ENABLE);

	GPIO_Clock_Cmd(NRF_IRQ_GPIO,ENABLE);
	GPIO_InitTypeDef GPIO_IRQ_initstruct;
	GPIO_IRQ_initstruct.GPIO_Pin	= NRF_IRQ_Pin;
	GPIO_IRQ_initstruct.GPIO_Mode	= GPIO_Mode_IN;
	GPIO_IRQ_initstruct.GPIO_OType	= GPIO_OType_PP;
	GPIO_IRQ_initstruct.GPIO_PuPd	= GPIO_PuPd_UP;
	GPIO_IRQ_initstruct.GPIO_Speed	= GPIO_Speed_50MHz;
	GPIO_Init(NRF_IRQ_GPIO,&GPIO_IRQ_initstruct);

	//TODO: experimentar inicializar o clock do SYSCFG depois do clock do IRQ_GPIO

	EXTI_InitTypeDef EXTI_cfg;
	EXTI_cfg.EXTI_Line		= EXTI_Line(NRF_IRQ_Pin);
	EXTI_cfg.EXTI_LineCmd	= ENABLE;
	EXTI_cfg.EXTI_Mode		= EXTI_Mode_Interrupt;
	EXTI_cfg.EXTI_Trigger	= EXTI_Trigger_Falling;
	EXTI_Init(&EXTI_cfg);

	NVIC_InitTypeDef NVIC_cfg;
	NVIC_cfg.NVIC_IRQChannel					= EXTIx_IRQn(NRF_IRQ_Pin);
	NVIC_cfg.NVIC_IRQChannelCmd					= ENABLE;
	NVIC_cfg.NVIC_IRQChannelPreemptionPriority	= 0x0f;	//lower priority, can be preempted
	NVIC_cfg.NVIC_IRQChannelSubPriority			= 0x0f;	//lower priority, can be preempted
	NVIC_Init(&NVIC_cfg);

	SYSCFG_EXTILineConfig(EXTI_PortSource(NRF_IRQ_GPIO),EXTI_PinSource(NRF_IRQ_Pin));

	//MOSI, MISO, SCK GPIO configuration
	GPIO_InitTypeDef GPIO_SPI_Pins_initstruct;
	GPIO_SPI_Pins_initstruct.GPIO_Mode	= GPIO_Mode_AF;
	GPIO_SPI_Pins_initstruct.GPIO_OType = GPIO_OType_PP;
	GPIO_SPI_Pins_initstruct.GPIO_PuPd	= GPIO_PuPd_NOPULL;
	GPIO_SPI_Pins_initstruct.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_Clock_Cmd(SCK_GPIO, ENABLE);
	GPIO_SPI_Pins_initstruct.GPIO_Pin = SCK_Pin;
	GPIO_Init(SCK_GPIO, &GPIO_SPI_Pins_initstruct);

	GPIO_Clock_Cmd(MISO_GPIO, ENABLE);
	GPIO_SPI_Pins_initstruct.GPIO_Pin = MISO_Pin;
	GPIO_Init(MISO_GPIO, &GPIO_SPI_Pins_initstruct);

	GPIO_Clock_Cmd(MOSI_GPIO, ENABLE);
	GPIO_SPI_Pins_initstruct.GPIO_Pin = MOSI_Pin;
	GPIO_Init(MOSI_GPIO, &GPIO_SPI_Pins_initstruct);

	//SPI1 CONFIG MOSI, MISO, SCK
	GPIO_PinAFConfig(SCK_GPIO , GPIO_PinSource(SCK_Pin) , GPIO_AF_SPIx(NRF_SPI));
	GPIO_PinAFConfig(MISO_GPIO, GPIO_PinSource(MISO_Pin), GPIO_AF_SPIx(NRF_SPI));
	GPIO_PinAFConfig(MOSI_GPIO, GPIO_PinSource(MOSI_Pin), GPIO_AF_SPIx(NRF_SPI));

	//finalizada a etapa de configura��o, faz o Power Up
	begin();
}

//Chip Enable ativa o modo RX ou TX
void NRF::ASSERT_CE(int STATE){
	if(STATE==SET){
		GPIO_SetBits(NRF_CE_GPIO,NRF_CE_Pin);//CE=HIGH p�e o NRF em TX
	}else if(STATE==RESET){
		GPIO_ResetBits(NRF_CE_GPIO,NRF_CE_Pin);//CE=LOW p�e o NRF em RX
	}
}

//sup�e que o NRF estava desligado e p�e ele em standby-I
void NRF::begin(){
	Delay_ms(11);

	ASSERT_CS(SET);
	ASSERT_CE(RESET);

	//l� CONFIG
	uint8_t config;

	R_REGISTER(0x00,1, &config);

	//faz PWR_UP=1, todas as interrup��es s�o refletidas no pino de IRQ
	config = 0b00000010;

	//grava o novo valor de CONFIG
	W_REGISTER(0x00,1,&config);
	Delay_ms(1);

	//Renault: escreveu 1 em 3 flags  que precisam ser  limpas no in�cio (RX_DR,TX_DS,MAX_RT)
	uint8_t status = RX_DR_MASK|TX_DS_MASK|MAX_RT_MASK;
	W_REGISTER(0x07,1,&status);

	Delay_ms(2);//tempo de startup
	return;
}

void NRF::stdbyI_to_TX(uint8_t channel){
	uint8_t config;
	R_REGISTER(0x00,1,&config);
	Delay_ms(1);

	config &= 0b11111110;//makes PRIM_RX=0 (transmitter)
	W_REGISTER(0x00,1,&config);
	Delay_ms(1);

	RF_CH_setup(channel);//setup of frequency
	return;
}

void NRF::stdbyI_to_RX(uint8_t channel){
	uint8_t config;
	R_REGISTER(0x00,1,&config);
	Delay_ms(1);

	config |= 0b00000001;//makes PRIM_RX=1 (receiver)
	W_REGISTER(0x00,1,&config);
	Delay_ms(1);

	RF_CH_setup(channel);//setup of frequency
	return;
}//WORKED!

//preenche o registrador EN_AA usando o valor ENAA_Px
void NRF::EN_AA_setup(uint8_t ENAA_Px){
	uint8_t en_aa = ENAA_Px;

	W_REGISTER(0x01,1,&en_aa);
	Delay_ms(1);
	
	return;
}//WORKED!

//preenche o registrador EN_RXADDR usando o valor ERX_Px
void NRF::EN_RXADDR_setup(uint8_t ERX_Px){
	uint8_t en_pipes = ERX_Px;

	W_REGISTER(0x02,1,&en_pipes);
	Delay_ms(1);
	
	return;
}//WORKED!

//preenche o registrador SETUP_AW, grava nele o valor AW
//AW deve ser constru�do usando as constantes AW_x_bytes
void NRF::SETUP_AW_setup(uint8_t AW){
	uint8_t address_width = AW;

	W_REGISTER(0x03,1,&address_width);
	Delay_ms(1);
	
	return;
}//WORKED!

//Setup of Automatic Retransmission, fills the SETUP_RETR with ARconfig
//should be filled with one RETR_ARC and, optionally, one RETR_ARDelay value
void NRF::SETUP_RETR_setup(uint8_t setup_retries){
	uint8_t ARconfig = setup_retries;

	W_REGISTER(0x04,1,&ARconfig);
	Delay_ms(1);
	
	return;
}//WORKED!

//preenche o registrador RF_CH, grava o valor rf_channel
void NRF::RF_CH_setup(uint8_t channel){
	uint8_t rf_ch = channel;

	W_REGISTER(0x05,1,&rf_ch);
	Delay_ms(1);
	
	return;
}//WORKED!

//preenche o registrador RF_SETUP, grava nele o valor configuration
void NRF::RF_SETUP_setup(uint8_t configuration){
	uint8_t rf_setup =  configuration;

	W_REGISTER(0x06,1,&rf_setup);
	Delay_ms(1);
	
	return;
}//WORKED!

//preenche o registrador RX_ADDR_Px da pipe associada, usando o(s) byte(s) em pointer
void NRF::RX_ADDR_Px_setup(uint8_t RX_Pipe,uint8_t* pointer){
	if(RX_Pipe == RX_Pipe_0)
		RX_ADDR_P0_setup(pointer);
	else if(RX_Pipe == RX_Pipe_1)
		RX_ADDR_P1_setup(pointer);
	else{
		//TODO:melhorar a portabilidade, adicionar constantes com o endere�o de cada registrador
		W_REGISTER(0x0a+RX_Pipe,1,pointer);
		Delay_ms(1);
	
	}
	return;
}//WORKED!

//preenche o registrador RX_ADDR_P0 com os bytes em pointer, usa o n�mero de bytes especificado por SETUP_AW
void NRF::RX_ADDR_P0_setup(uint8_t* pointer){
	uint8_t size;
	//TODO:melhorar a portabilidade, adicionar constantes com o endere�o de cada registrador
	R_REGISTER(0x03,1,&size);//l� SETUP_AW e armazena em size (OBS:SETUP_AW armazena armazena address_width - 2)

	//corrige os valores de size lidos
	if(size == AW_3_bytes)
		size = 3;
	if(size == AW_4_bytes)
		size = 4;
	if(size == AW_5_bytes)
		size = 5;
	//TODO:melhorar a portabilidade, adicionar constantes com o endere�o de cada registrador
	W_REGISTER(0x0a,size,pointer);
	Delay_ms(1);
	
	return;
}//WORKED!

//preenche o registrador RX_ADDR_P1 com os bytes em pointer, usa o n�mero de bytes especificado por SETUP_AW
void NRF::RX_ADDR_P1_setup(uint8_t* pointer){
	uint8_t size;
	//TODO:melhorar a portabilidade, adicionar constantes com o endere�o de cada registrador
	R_REGISTER(0x03,1,&size);//l� SETUP_AW e armazena em size (OBS:SETUP_AW armazena armazena address_width - 2)

	//corrige os valores de size lidos
	if(size == AW_3_bytes)
		size = 3;
	if(size == AW_4_bytes)
		size = 4;
	if(size == AW_5_bytes)
		size = 5;
	//TODO:melhorar a portabilidade, adicionar constantes com o endere�o de cada registrador
	W_REGISTER(0x0b,size,pointer);
	Delay_ms(1);
	
	return;
}//WORKED!

//escreve o registrador RX_PW_Px correspondente a pipe, grava payload_width
void NRF::RX_PW_Px_setup(uint8_t RX_Pipe, uint8_t payload_width){
	uint8_t pw = payload_width;

	//TODO:melhorar a portabilidade, adicionar constantes com o endere�o de cada registrador
	W_REGISTER(0x11+RX_Pipe,1,&pw);
	Delay_ms(1);
	
	return;
}//WORKED!

void NRF::FEATURE_setup(uint8_t FEATURE){
	uint8_t feature = FEATURE;

	W_REGISTER(0x1d,1,&feature);
	Delay_ms(1);

	return;
}

void NRF::DYNPD_setup(uint8_t DYNPD){
	uint8_t dynpd = DYNPD;

	W_REGISTER(0x1c,1,&dynpd);
	Delay_ms(1);

	return;
}

//retorna 1 se o NRF24 recebeu alguma coisa, retorna 0 se ainda n�o recebeu nada
uint8_t NRF::DATA_READY(void){
	uint8_t rx_empty=0;

	R_REGISTER(0x17,1,&rx_empty);
	rx_empty &= RX_EMPTY_MASK;

	uint8_t rx_dr;
	R_REGISTER(0x07,1,&rx_dr);
	STD_ITER_DELAY
	rx_dr &= RX_DR_MASK;

	if(rx_dr || !rx_empty){
		return 1;
	}
	else
		return 0;
}

//retorna 1 se o NRF24 enviou alguma coisa(recebeu o ACK, caso esteja habilitado), retorna 0 se ainda n�o conseguiu enviar(ou n�o recebeu o ACK, caso esteja habilitado)
uint8_t NRF::TRANSMITTED(void){
	uint8_t fifo_status;
	uint8_t status;

	R_REGISTER(FIFO_STATUS_ADDRESS,1,&fifo_status);
	for (int i=0;i<0x1dc4;i++);
	fifo_status &= TX_EMPTY_MASK;

	R_REGISTER(STATUS_ADDRESS,1,&status);
	for (int i=0;i<0x1dc4;i++);

	//if((status & TX_DS_MASK) || !(fifo_status & TX_EMPTY_MASK))
	if(status & TX_DS_MASK){
		ASSERT_CE(RESET);//para evitar problemas (novas interrup��es) e pq W_REGISTER() s� pode ser chamada com CE=LOW
		status |= TX_DS_MASK;//reseta a flag TX_DS
		W_REGISTER(STATUS_ADDRESS,1,&status);
		STD_ITER_DELAY
		return 1;
	}
	else
		return 0;
}

//tenta a transmiss�o
//data: ponteiro para os bytes a serem transmitidos; size: n�mero de bytes a enviar
//retorna 1 se o NRF24 enviou alguma coisa(recebeu o ACK, caso esteja habilitado), retorna 0 se ainda n�o conseguiu enviar(ou n�o recebeu o ACK, caso esteja habilitado)
uint8_t NRF::SEND(uint8_t* data, uint8_t size){
	W_TX_PAYLOAD(data,size);
	for (int i=0;i<0x1dc4;i++);

	//pulse on CE to start transmission
	ASSERT_CE(SET);
	for (int i=0;i<0x1dc4;i++);//minimum pulse width = 10us, here we use 100us
	ASSERT_CE(RESET);

	//TODO consertar o valor de retorno de NRF::SEND
	return TRANSMITTED();
}

void NRF::W_ACK_PAYLOAD(uint8_t pipe,uint8_t* pointer,uint8_t number){
	if(pipe>5)
		return;
	SPI::W_ACK_PAYLOAD(pipe,pointer,number);
	return;

}

void NRF::FLUSH_TX(){
	SPI::FLUSH_TX();
	return;
}

void NRF::FLUSH_RX(){
	SPI::FLUSH_RX();
	return;
}

void NRF::start_listen(){
	ASSERT_CE(SET);
	return;
}

void NRF::stop_listen(){
	ASSERT_CE(RESET);
	return;
}

/*
 * Armazena em data a mensagem mais recente
 * l� todos os  pacotes na RX_FIFO, apenas o mais recente � passado
 * data: ponteiro que armazenar� os bytes recebidos;
 * retorna 1 se o NRF24 recebeu alguma coisa, retorna 0 se ainda n�o conseguiu receber
 */
uint8_t NRF::RECEIVE(uint8_t* data){
	//passa aqui

	if(DATA_READY()){//verifica se recebeu algo

		ASSERT_CE(RESET);//para evitar problemas (novas interrup��es)

		read_top_of_fifo:
		READ_RX_FIFO(data);

		uint8_t status,fifo_status;
		R_REGISTER(STATUS_ADDRESS,1,&status);
		STD_ITER_DELAY

		//com CE=LOW, reseta a flag RX_DR, conforme a product specification
		//if write status in the register, we'd clear any flag, we only "write" 0 in the bits we don't want to change in order to protect they from being changed
		status &= ~(TX_DS_MASK|MAX_RT_MASK);
		W_REGISTER(STATUS_ADDRESS,1,&status);
		STD_ITER_DELAY

		//verifica se h� outros pacotes para ler, conforme a product specification
		R_REGISTER(FIFO_STATUS_ADDRESS,1,&fifo_status);
		STD_ITER_DELAY
		if((fifo_status & RX_EMPTY_MASK)==0){
			goto read_top_of_fifo;
		}

		return 1;
	}
	else{
		return 0;
	}
}

//l� a payload no topo da RX_FIFO
//pointer: local aonde a mensagem ser� gravada
void NRF::READ_RX_FIFO(uint8_t* pointer){
	//descobre qual pipe recebeu o pacote
	uint8_t RX_Pipe = GET_PIPE_FOR_READING();

	//TODO:fix NRF_R_RX_PAYLOAD()
	//l� o RX_PW_Px correspondente �quela pipe
	uint8_t payload_length;
	R_REGISTER(0x11+RX_Pipe,1,&payload_length);
	STD_ITER_DELAY

	CMD(0x61,payload_length,0x00,pointer);//comando R_RX_PLD
	STD_ITER_DELAY

	return;
}

//retorna a pipe que recebeu o pacote dispon�vel para leitura
uint8_t NRF::GET_PIPE_FOR_READING(void){
	uint8_t status = NOP();
	uint8_t pipe_number = (status && RX_P_NO_MASK) >> 1;
	return  pipe_number;
}

//produz uma  struct de  configura��o com "valores default"
void NRF::RX_configure(){
	uint8_t myAddress[]={0xe7,0xe7,0xe7,0xe7,0xe7};

	config_Struct configuration;
	configuration.AW_x_bytes			=AW_5_bytes;
	configuration.ERX_Px				=ERX_P0;
	configuration.RETR_ARC_and_ARD		=RETR_ARC_DISABLE_RETRANSMIT;//TODO: alterar AutoACK option
	configuration.RF_SETUP				=RF_SETUP_Data_Rate_2Mbps;
	configuration.channel				=0x02;
	configuration.RX_ADDR_P0			=myAddress;
	configuration.payload_width_pipe_0	=5;
	configuration.TX_ADDR				=myAddress;//TX_ADDR=RX_ADDR_P0 para permitir autoACK com o Enhanced Shockburst ativado

#ifdef USE_AUTOACK
	configuration.ENAA_Px=ENAA_P0;//write 1 in some bit of EN_AA will force high on EN_CRC bit
	configuration.FEATURE=FEATURE_EN_ACK_PAY|FEATURE_EN_DPL;
	configuration.DYNPD  =DYNPD_DPL_P0;
#else
	configuration.ENAA_Px=ENAA_Disable_All_Pipes;
#endif

	RX_configure(&configuration);

	return;
}

//recebe uma struct de configura��o do modo de recep��o e altera os valores do registradores correspondentes
//TODO: incluir a configura��o dos endere�o da(s) pipe(s)
void NRF::RX_configure(config_Struct* pointer){
	stdbyI_to_RX(pointer->channel);//writes CONFIG and RF_CH registers

	EN_AA_setup(pointer->ENAA_Px);

	EN_RXADDR_setup(pointer->ERX_Px);

	SETUP_AW_setup(pointer->AW_x_bytes);

	SETUP_RETR_setup(pointer->RETR_ARC_and_ARD);

	RF_SETUP_setup(pointer->RF_SETUP);

	//TODO: fazer a verifica��o de quais as pipes precisam ser configuradas e quais as payloads de cada uma
	uint8_t* payload_width_pointer = &(pointer->payload_width_pipe_0);
	int i;
	for(i=RX_Pipe_0;i<=RX_Pipe_5;i++){
		if((pointer->ERX_Px) & (1 << i)){//verifica se a pipe i est� habilitada
			RX_PW_Px_setup(i, *payload_width_pointer);//configura o tamanho da payload da pipe i
		}
		payload_width_pointer++;//passa a apontar para a pr�xima vari�vel payload_width_pipe_x da struct
	}

	uint8_t** RX_ADDR_Px_pointer = &(pointer->RX_ADDR_P0);//RX_ADDR_Px_pointer � ponteiro para os RX_ADDR_Px

	//TODO: melhorar a implementa��o, para s� escrever os registradores que realmente precisam ser configurados
	for(i=RX_Pipe_0;i<=RX_Pipe_5;i++){
		if((pointer->ERX_Px) & (1 << i)){//verifica se a pipe i est� habilitada
			RX_ADDR_Px_setup(i,*RX_ADDR_Px_pointer);//configura o endere�o da pipe i
		}
		RX_ADDR_Px_pointer++;//passa a apontar para o pr�ximo RX_ADDR_Px da struct
	}

	TX_ADDR_setup(pointer->TX_ADDR);

	//tenta escrever no registrador FEATURE, se falhar, precisa ser ativado
	uint8_t feat=0b111;
	W_REGISTER(0x1d,1,&feat);
	Delay_ms(1);
	R_REGISTER(0x1d,1,&feat);
	Delay_ms(1);
	if(!feat){//se feat permanece nulo
		ACTIVATE();
	}

	FEATURE_setup(pointer->FEATURE);
	DYNPD_setup(pointer->DYNPD);

	return;
}

//produz uma  struct de  configura��o com "valores default"
void NRF::TX_configure(){
	uint8_t ReceiverAddress[]={0xe7,0xe7,0xe7,0xe7,0xe7};

	config_Struct configuration;
	configuration.AW_x_bytes=AW_5_bytes;
	configuration.RF_SETUP=RF_SETUP_Data_Rate_2Mbps;
	configuration.channel=0x02;
	configuration.TX_ADDR=ReceiverAddress;
	configuration.payload_width_pipe_0=5;

	#ifdef USE_AUTOACK
		configuration.ENAA_Px=ENAA_P0;
		configuration.ERX_Px=ERX_P0;
		configuration.RETR_ARC_and_ARD=RETR_ARC_15_RETRANSMIT|RETR_ARD_wait_4000_us;//at� 15 tentativas, 1 a cada 4ms
		configuration.RX_ADDR_P0=configuration.TX_ADDR;//para permitir AutoACK
		configuration.FEATURE=FEATURE_EN_ACK_PAY|FEATURE_EN_DPL;
		configuration.DYNPD  =DYNPD_DPL_P0;
	#else //se o AUTOACK N�O estiver sendo usado
		configuration.ENAA_Px=ENAA_Disable_All_Pipes;
		configuration.ERX_Px=ERX_Disable_All_Pipes;
		configuration.RETR_ARC_and_ARD=RETR_ARC_DISABLE_RETRANSMIT;
	#endif

	TX_configure(&configuration);

	return;
}

//recebe uma struct de configura��o do modo de transmiss�o e altera os valores do registradores correspondentes
void NRF::TX_configure(config_Struct* pointer){
	stdbyI_to_TX(pointer->channel);//writes CONFIG and RF_CH registers

	EN_AA_setup(pointer->ENAA_Px);

	EN_RXADDR_setup(pointer->ERX_Px);

	SETUP_AW_setup(pointer->AW_x_bytes);

	SETUP_RETR_setup(pointer->RETR_ARC_and_ARD);

	RF_SETUP_setup(pointer->RF_SETUP);

	TX_ADDR_setup(pointer->TX_ADDR);

	//TODO: fazer a verifica��o de quais as pipes precisam ser configuradas e quais as payloads de cada uma
	uint8_t* payload_width_pointer = &(pointer->payload_width_pipe_0);
	uint8_t i;
	for(i=RX_Pipe_0;i<=RX_Pipe_5;i++){
		if((pointer->ERX_Px) & (1 << i)){//verifica se a pipe i est� habilitada
			RX_PW_Px_setup(i, *payload_width_pointer);//configura o tamanho da payload da pipe i
		}
		payload_width_pointer++;//passa a apontar para a pr�xima vari�vel payload_width_pipe_x da struct
	}

	uint8_t** RX_ADDR_Px_pointer = &(pointer->RX_ADDR_P0);//RX_ADDR_Px_pointer � ponteiro para os RX_ADDR_Px

	//TODO: melhorar a implementa��o, para s� escrever os registradores que realmente precisam ser configurados
	for(i=RX_Pipe_0;i<=RX_Pipe_5;i++){
		if((pointer->ERX_Px) & (1 << i)){//verifica se a pipe i est� habilitada
			RX_ADDR_Px_setup(i,*RX_ADDR_Px_pointer);//configura o endere�o da pipe i
		}
		RX_ADDR_Px_pointer++;//passa a apontar para o pr�ximo RX_ADDR_Px da struct
	}

	//tenta escrever no registrador FEATURE, se falhar, precisa ser ativado
	uint8_t feat=0b111;
	W_REGISTER(0x1d,1,&feat);
	Delay_ms(1);
	R_REGISTER(0x1d,1,&feat);
	Delay_ms(1);
	if(!feat){//se feat permanece nulo
		ACTIVATE();
	}

	FEATURE_setup(pointer->FEATURE);
	DYNPD_setup(pointer->DYNPD);

	return;
}

//TODO: melhorar a implementa��o, para s� escrever os registradores que realmente precisam ser configurados
void NRF::TX_ADDR_setup(uint8_t* pointer){
	W_REGISTER(0x10,5,pointer);
	Delay_ms(1);
	
	return;
}

//TODO: levar em conta a possibilidade de o registrador ter bits somente de leitura
void NRF::RESET_ALL_REGISTERS(){
	uint8_t i=0;
	REGISTER* current_register = &(CONFIG);
	for(i=0x00;i<=0x19;i++){
		W_REGISTER(current_register->get_address(),current_register->get_size(),current_register->content);
		Delay_ms(1);
	
		current_register++;
	}

	return;
}//WORKED! but fails with read-only registers
