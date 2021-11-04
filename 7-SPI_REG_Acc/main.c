//Lectura de ADC utilizando los registros- Sin muestra de los datos por UART

#include "stm32f4xx.h"
long i=0;
void SPI_Config(void){
	//GPIOA & GPIOE Pin Configuration
	RCC->AHB1ENR  |=  0x00000019; // Enable clock for GPIOA, GPIOD and GPIOE
  GPIOA->AFR[0] |=  0x55500000; // select AF5 (SPI1) for PA5,PA6,PA7
  GPIOA->MODER  |=  0x0000A800; // PA5,PA6,PA7 => alternate functions PA0=>Input NSS Pin
	//GPIOA->MODER  |=  0x00008800; // PA5,PA6,PA7 => alternate functions PA0=>Input NSS Pin
  GPIOA->OTYPER |=  0x00000000; // Push-pull PA5->PA7
	GPIOA->OSPEEDR |= 0x55555555;
  GPIOA->PUPDR  |=  0x55555555;	// No pull-up, no pull-down PA5->PA7
	//GPIOA->PUPDR  |=  0x00001000;	// No pull-up, no pull-down PA5,PA7| Pull up PA6
	
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
//uint8_t SPI_Write (uint8_t Adr, uint8_t Dat)    {
//	while (!(SPI1->SR & 0x0002)) {};
//  GPIOE->ODR				 = 0x0000;
//	SPI1->CR1          |= 0x0040;       	// Enable transmission SPE=1  
//	SPI1->DR					 = Adr;
//	while (!(SPI1->SR & 0x0002)) {};		// TXE = 1
//	SPI1->DR					 = Dat;
//	while (!(SPI1->SR & 0x0001)) {};		// RXE = 1 
//	prueba						 = SPI1->DR;
//	while (!(SPI1->SR & 0x0002)) {};		// TXE = 1 
//	//while (!(~SPI1->SR & 0x0080)) {};		// BSY = 0 /////////REVISAR!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//	SPI1->CR1         = 0x0000;       	// Enable transmission SPE=0 
//	RCC->APB2ENR  = 0x00000000;
//	GPIOE->ODR				= 0x0008;
//	
//	
//		
//	return ((uint8_t)prueba);
//}

int8_t SPI_Write (uint16_t Adr)    {
  //GPIOE->ODR				 = 0x0000;
	SPI1->CR1          |= 0x0040;       	// Enable transmission SPE=1  
	SPI1->DR					 = Adr;
	while (!(SPI1->SR & 0x0002)) {};		// TXE = 1
	while ((SPI1->SR & 0x0080)) {};		// BSY = 0 /////////REVISAR!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	while (!(SPI1->SR & 0x0001)) {};		
	//GPIOE->ODR				 = 0x0008;
	return (SPI1->DR);
}

int8_t Data;
//short SPI_Read (char Adr)    {
//	Adr = 0x80+Adr;
//	SPI_Config();
//	while (!(SPI1->SR & 0x0002)) {};
//  GPIOE->ODR				 = 0x0000;
//	SPI1->CR1          |= 0x0040;       	// Enable transmission SPE=1  
//	SPI1->DR					 = Adr;
//	while (!(SPI1->SR & 0x0002)) {};		// TXE = 1
//	while (!(SPI1->SR & 0x0001)) {};		// RXE = 1 
//	prueba						 = SPI1->DR;
//	while (!(SPI1->SR & 0x0002)) {};		// TXE = 1
//	while (!(~SPI1->SR & 0x0080)) {};		// BSY = 0 /////////REVISAR!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//	SPI1->CR1         = 0x0000;       	// Enable transmission SPE=0 
//	RCC->APB2ENR  = 0x00000000;
//	GPIOE->ODR				= 0x0008;
//	
//	SPI_Config();
//	GPIOE->ODR				 = 0x0000;
//	SPI1->CR1          |= 0x0400;       	// Enable transmission SPE=1  BIDIOE=1
//	SPI1->CR1          |= 0x0040;       	// Enable transmission SPE=1  BIDIOE=1
//	while (!(SPI1->SR & 0x0001)) {};		// RXE = 1 
//	Data 							 = SPI1->DR;
//	SPI1->CR1         = 0x0000;       	// Enable transmission SPE=0 
//	GPIOE->ODR				= 0x0008;	
//	return ((short)Data);
//	
//}

int8_t SPI_Read (char Adr)    {
	GPIOE->ODR				 = 0x0000;
	SPI1->CR1          |= 0x0040;       	// Enable transmission SPE=1  
	SPI1->DR					 = Adr;
	while (!(SPI1->SR & 0x0002)) {};		// TXE = 1
	while ((SPI1->SR & 0x0080)) {};		// BSY = 0 /////////REVISAR!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	while (!(SPI1->SR & 0x0001)) {};		
	GPIOE->ODR				 = 0x0008;
			
	GPIOE->ODR				 = 0x0000;
	SPI1->CR1          |= 0x0400;       	// Enable transmission SPE=1  
	while (!(SPI1->SR & 0x0001)) {};		
	Data							 = 	SPI1->DR;
	GPIOE->ODR				 = 0x0008;
	return (Data);
}

void ACC_Config(void){
	GPIOE->ODR				 = 0x0000;
	SPI_Write(0x20);
	SPI_Write(0x17);
	GPIOE->ODR				 = 0x0008;
	
	GPIOE->ODR				 = 0x0000;
	SPI_Write(0x24);
	SPI_Write(0x00);
	GPIOE->ODR				 = 0x0008;
}

uint8_t XL,XH,YL,YH,ZL,ZH;
uint16_t X,Y,Z;
void Lectura(void){
	GPIOE->ODR				 = 0x0000;
	SPI_Write(0xA8);
	XL= SPI_Write(0x00);
	GPIOE->ODR				 = 0x0008;

	GPIOE->ODR				 = 0x0000;
	SPI_Write(0xA9);
	XH= SPI_Write(0x00);
	GPIOE->ODR				 = 0x0008;

	GPIOE->ODR				 = 0x0000;
	SPI_Write(0xAA);
	YL= SPI_Write(0x00);
	GPIOE->ODR				 = 0x0008;

	GPIOE->ODR				 = 0x0000;
	SPI_Write(0xAB);
	YH= SPI_Write(0x00);
	GPIOE->ODR				 = 0x0008;

	GPIOE->ODR				 = 0x0000;
	SPI_Write(0xAC);
	ZL= SPI_Write(0x00);
	GPIOE->ODR				 = 0x0008;

	GPIOE->ODR				 = 0x0000;
	SPI_Write(0xAD);
	ZH= SPI_Write(0x00);
	GPIOE->ODR				 = 0x0008;
	
	X=(XH << 8| XL);
	Y=(YH << 8| YL);
	Z=(ZH << 8| ZL);	
	
}

int main ()  {

  SPI_Config();
	ACC_Config();
	for (i=0; i<30000; i++)       {};
  while (1) {
		Lectura();
		/*for (i=0; i<1000; i++)       {}; 
		GPIOD->ODR = 0x5555;
    for (i=0; i<800000; i++)       {};             // waste time, ~30ms
		GPIOD->ODR = 0x0000;
		for (i=0; i<800000; i++)       {};             // waste time, ~30ms*/
  };
}