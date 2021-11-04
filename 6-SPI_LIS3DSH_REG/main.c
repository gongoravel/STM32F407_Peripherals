#include "stm32f4xx.h"
long i=0;

char DD;
void UART4_IRQHandler(void)     {  
  // TX IRQ part
  if (UART4->SR & 0x0080) {     // If TXE flag in SR is on then
     // UART4->CR1 &= ~0x0080;    // disable TX interrupt
        GPIOD->ODR |=  0x8000;
  };
  // RX IRQ part
  if (UART4->SR & 0x0020) {     // if RXNE flag in SR is on then
   DD = UART4->DR;       // save received character & clear flag
       if ( DD== 'a')
                 GPIOD->ODR |=  0x4080;   // if 'a' => LED on
  }
}

void Serial_config(void){

 int a=0;																//Variable para loop de espera
 RCC->CR |= 0x10000;                    //Inicialización de Bit 16 de registro CR para habilitar el High Speed External Clock 
 while((RCC->CR & 0x20000)==0);         //Espera hasta que la configuración del reloj en HSE esté lista
 RCC->APB1ENR = 0x10000000;             //Habilitación de interfaz de potencia para el reloj     
 RCC->CFGR = 0x00009400;                //Prescaler de alta velocidad APB2= Valor de reloj AHB dividido en 2
																				//Prescaler de baja velocidad APB1= Valor de reloj AHB dividido en 4
 
	RCC->PLLCFGR = 0x07405408;            //Division de frecuencia PLL entre 7  
																				//HSE seleccionado como entrada para PLL y PLLI2S
																				//Factor de division del PLL principal configurado en 8
																				//Frecuencia de salida VCO calculada a partir de PLLN
																				//Reloj principal del sistema PLLP entre 2	
	
 RCC->CR |= 0x01000000;                 //PLL principal encendido     
 while((RCC->CR & 0x02000000)==0);      //Espera hasta que la configuración del reloj en HSE esté lista
 FLASH->ACR = (0x00000605);             //Tiempo de acceso a la memoria flash a partir del periodo de reloj de la máquina = 5 estados de espera     
 RCC->CFGR |= 2;                        //Caché de datos habilitado y Caché de instrucción habilitado
 for (a=0;a<=500;a++){};								//Waste time for configuration

	
	// GPIO clock enable, digital pin definitions
  RCC->AHB1ENR  |= 0x00000008;   // Enable clock for GPIOD
  GPIOD->MODER  |= 0x55550000; 
  //GPIOC->MODER  |= 0x00900000;  // PC10, PC11 => AF mode
  GPIOC->AFR[0] |= 0x00000010;  // select AF1 (TIM2) for PA01 -> TIM2_CH2
   
  //UART4 initialization
  RCC->APB1ENR  |= 0x00080000;  // Enable clock for UART4
  RCC->AHB1ENR  |= 0x00000004;  // Enable clock for GPIOC
  GPIOC->MODER  |= 0x00a00000;  // PC10, PC11 => AF mode
  GPIOC->AFR[1] |= 0x00008800;  // select AF8 (UART4,5,6) for PA10, PA11 
  UART4->BRR     = 0x1120;      // (0x1120--9600 baud)  (0x0890--19200)(0x0450--38400)(0x02e0--57600)(0x016d--115200)
  UART4->CR1    |= 0x200c;      // Enable UART for TX, RX , UART
  UART4->CR1    |= 0x0020;      // Enable RX interrupt
  //NVIC init
  NVIC_EnableIRQ(UART4_IRQn);   // Enable IRQ for UART4 in NVIC
  // endless loop
	
}


void SPI_Config(void){
	//GPIOA & GPIOE Pin Configuration
	RCC->AHB1ENR  |=  0x00000019; // Enable clock for GPIOA, GPIOD and GPIOE
  GPIOA->AFR[0] |=  0x55500000; // select AF5 (SPI1) for PA5,PA6,PA7
  GPIOA->MODER  |=  0x0000A800; // PA5,PA6,PA7 => alternate functions PA0=>Input NSS Pin
  GPIOA->OTYPER |=  0x00000000; // Push-pull PA5->PA7
	GPIOA->OSPEEDR |= 0x55555555;
  GPIOA->PUPDR  |=  0x55555555;	// No pull-up, no pull-down PA5->PA7
	
	/////Configuracion GPIOE//
	GPIOE->MODER   |=  0x00000040; 	// PE3 as Output	
	GPIOE->OTYPER	  =	 0;						// Push Pull en PE3
	GPIOE->OSPEEDR |=	 0x00000080;	// 50 MHz
	GPIOE->PUPDR   |=  0x00000040;	// Pull-up resistor to guarantee '1' logic state at CS pin (PE3)
	GPIOE->ODR		  =  0x0008;
	
	GPIOD->MODER  |=  0x55555555; // PD as Output
	
	//SPI1 Configuration = PA7-MOSI, PA6-MISO, PA5-SCK, PE3-SEL
  RCC->APB2ENR  |= 0x00001000;
	SPI1->CR1			|= 0x0000;
	SPI1->CR1			|= 0x0107;			//Bidirectional, 8 bit transmission, Full-duplex, MSB transmist first, OpFreq= fpclk/2, Polarity High, Phase 2 edge
	SPI1->CR2 		= 0x0004;
	SPI1->CRCPR 	= 0;
}
uint8_t prueba;
uint8_t SPI_Write (uint8_t Adr, uint8_t Dat)    {
  GPIOE->ODR				 = 0x0000;
	SPI1->CR1          |= 0x0040;       	// Enable transmission SPE=1  
	SPI1->DR					 = Adr;
	while (!(SPI1->SR & 0x0002)) {};		// TXE = 1
	while (!(SPI1->SR & 0x0001)) {};
	SPI1->DR					 = Dat;
	while (!(SPI1->SR & 0x0002)) {};		// TXE = 1
	while ((SPI1->SR & 0x0080)) {};		// BSY = 0 /////////REVISAR!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	while (!(SPI1->SR & 0x0001)) {};		
	GPIOE->ODR				= 0x0008;
			
	return ((uint8_t)prueba);
}

uint8_t SPI_Rw (uint8_t Adr)    {
	SPI1->CR1          |= 0x0040;       	// Enable transmission SPE=1  
	SPI1->DR					 = Adr;
	while (!(SPI1->SR & 0x0002)) {};		// TXE = 1
	while ((SPI1->SR & 0x0080)) {};		// BSY = 0 /////////REVISAR!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	while (!(SPI1->SR & 0x0001)) {};		
	return (SPI1->DR);
}

uint8_t value;
uint8_t SPI_Read (uint8_t Adr)    {
	Adr = (0x80|Adr); 
	GPIOE->ODR				 = 0x0000;
	SPI_Rw (Adr);
	value = SPI_Rw (0x00);
	GPIOE->ODR				 = 0x0008;
	return (value);
}

void ACC_Config(void){
	SPI_Write(0x20,0x17);
	SPI_Write(0x24,0x00);
}

uint8_t XL,XH,YL,YH,ZL,ZH;
uint16_t X,Y,Z;
void Lectura(void){
	XL=SPI_Read(0x28);
	XH=SPI_Read(0x29);
	YL=SPI_Read(0x2A);
	YH=SPI_Read(0x2B);
	ZL=SPI_Read(0x2C);
	ZH=SPI_Read(0x2D);
	X=(XH << 8| XL);
	Y=(YH << 8| YL);
	Z=(ZH << 8| ZL);		
}

uint8_t b;
void ascii_conv(uint16_t eje){
	b=eje%100000/10000+48;
	UART4->DR = b;
	for (i=0; i<30000; i++) {};
	b=eje%10000/1000+48;
	UART4->DR = b;
	for (i=0; i<30000; i++) {};
	b=eje%1000/100+48;
	UART4->DR = b;
	for (i=0; i<30000; i++) {};
	b=eje%100/10+48;
	for (i=0; i<30000; i++) {};
	UART4->DR = b;
	for (i=0; i<30000; i++) {};
	b=eje%10+48;
	UART4->DR = b;
	for (i=0; i<30000; i++) {};
}


uint16_t XS, YS, ZS;
void Data_Prompt (void){
		UART4->DR = 'X';
		for (i=0; i<30000; i++) {};
		UART4->DR = ':';
		for (i=0; i<30000; i++) {};
		UART4->DR = '+';
		for (i=0; i<30000; i++) {};
		ascii_conv(X);
		for (i=0; i<30000; i++) {};
		UART4->DR = 9;
		for (i=0; i<30000; i++) {};

		UART4->DR = 'Y';
		for (i=0; i<30000; i++) {};
		UART4->DR = ':';
		for (i=0; i<30000; i++) {};
		UART4->DR = '+';
		for (i=0; i<30000; i++) {};
		ascii_conv(Y);
		for (i=0; i<30000; i++) {};
		UART4->DR = 9;
		for (i=0; i<30000; i++) {};

		UART4->DR = 'Z';
		for (i=0; i<30000; i++) {};
		UART4->DR = ':';
		for (i=0; i<30000; i++) {};
		UART4->DR = '+';
		for (i=0; i<30000; i++) {};
		ascii_conv(Z);
		for (i=0; i<30000; i++) {};
		UART4->DR = 10;
		for (i=0; i<30000; i++) {};
		UART4->DR = 13;
		for (i=0; i<30000; i++) {};
		
}


int main ()  {

  SPI_Config();
	ACC_Config();
	Serial_config();
	for (i=0; i<30000; i++)       {};
  while (1) {
		Lectura();
		Data_Prompt();
		/*for (i=0; i<1000; i++)       {}; 
		GPIOD->ODR = 0x5555;
    for (i=0; i<800000; i++)       {};             // waste time, ~30ms
		GPIOD->ODR = 0x0000;
		for (i=0; i<800000; i++)       {};             // waste time, ~30ms*/
  };
}