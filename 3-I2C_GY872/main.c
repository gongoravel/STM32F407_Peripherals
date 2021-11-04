#include "stm32f4xx.h"

char DD;
int i;
void Serial_config(void){

 int a=0;
 RCC->CR |= 0x10000;                         
 while((RCC->CR & 0x20000)==0);              
 RCC->APB1ENR = 0x10000000;                  
 RCC->CFGR = 0x00009400;                     
 RCC->PLLCFGR = 0x07405408;                  
 RCC->CR |= 0x01000000;                      
 while((RCC->CR & 0x02000000)==0);           
 FLASH->ACR = (0x00000605);                  
 RCC->CFGR |= 2;                             
 for (a=0;a<=500;a++){};

	
	// GPIO clock enable, digital pin definitions
  RCC->AHB1ENR  |= 0x00000008;   // Enable clock for GPIOD
  GPIOD->MODER  |= 0x55550000; 
  //GPIOC->MODER  |= 0x00900000;  // PC10, PC11 => AF mode
  GPIOC->AFR[0] |= 0x00000010;  // select AF1 (TIM2) for PA01 -> TIM2_CH2
   
  //UART4 initialization
  RCC->APB1ENR  |= 0x00080000;  // Enable clock for UART4
  RCC->AHB1ENR  |= 0x00000004;  // Enable clock for GPIOC
  GPIOC->MODER  |= 0x00a00000;  // PC10, PC11 => AF mode
  GPIOC->AFR[1] |= 0x00008800;  // select AF8 (UART4,5,6) for PC10-RX, PC11-TX 
  UART4->BRR     = 0x1120;      // (0x1120--9600 baud)  (0x0890--19200)(0x0450--38400)(0x02e0--57600)(0x016d--115200)
  UART4->CR1    |= 0x200c;      // Enable UART for TX, RX , UART
  UART4->CR1    |= 0x0020;      // Enable RX interrupt
  //NVIC init
  NVIC_EnableIRQ(UART4_IRQn);   // Enable IRQ for UART4 in NVIC
  // endless loop
	
}

uint8_t b;
void ascii_conv(uint16_t eje){
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

void I2C2_Init (void)   {
  RCC->APB1ENR  |= 0x00400000;       // Enable clock for I2C2
  I2C2->CR2     |= 0x0008;           // clock == 8MHz!   
  I2C2->CCR     |= 0x0028;           // clock control register (270kHz)
  I2C2->TRISE   |= 0x0009;           // rise time register
  I2C2->CR1     |= 0x0001;           // I2C2 enable
}

int Status2, Status4;
uint8_t x1,x2;
uint8_t LECTURA;

void I2C2_WriteBAR (char Adr, char Dat)    {
  I2C2->CR1         |= 0x0100;       // send START bit
  while (!(I2C2->SR1 & 0x0001)) {};  // wait for START condition (SB=1) 
  I2C2->DR           = 0XEE;         // slave address   -> DR & write
  while (!(I2C2->SR1 & 0x0002)) {};  // wait for ADDRESS sent (ADDR=1) 
	Status2        = I2C2->SR2;    // read status to clear flag
  I2C2->DR           = Adr;          // Address in chip -> DR & write
  while (!(I2C2->SR1 & 0x0080)) {};  // wait for DR empty (TxE)
  I2C2->DR           = Dat;          // Dat -> DR & write
  while (!(I2C2->SR1 & 0x0080)) {};  // wait for DR empty (TxE) 
  while (!(I2C2->SR1 & 0x0004)) {};  // wait for Byte sent (BTF)
  I2C2->CR1         |= 0x0200;       // send STOP bit
}

int8_t I2C2_ReadBAR (char Adr)    {   // procedure: RM0090, pg. 584!
  
  I2C2->CR1         |= 0x0100;       // send START bit
  while (!(I2C2->SR1 & 0x0001)) {};  // wait for START condition (SB=1) 
  I2C2->DR           = 0XEE;         // slave address -> DR      (LSB=1)
  while (!(I2C2->SR1 & 0x0002)) {};  // wait for ADDRESS sent    (ADDR=1) 
  Status2        = I2C2->SR2;    // read SR2 to clear flag
  I2C2->DR           = Adr;          // register in chip -> DR 
  while (!(I2C2->SR1 & 0x0080)) {};  // wait for DR empty        (TxE=1)
  while (!(I2C2->SR1 & 0x0004)) {};  // wait for ByteTransferred (BTF=1) 
  
  I2C2->CR1         |= 0x0100;       // send START bit
  while (!(I2C2->SR1 & 0x0001)) {};  // wait for START condition (SB=1)
	I2C2->DR           = 0XEF;         // slave address -> DR      (LSB=0)
  while (!(I2C2->SR1 & 0x0002)) {};  // wait for ADDRESS sent    (ADDR=1)
  Status4        = I2C2->SR2;    // read status to clear flag
  while (!(I2C2->SR1 & 0x0040)) {};  // wait for ByteReceived    (RxNE=1)
  I2C2->CR1         |= 0x0200;       // send STOP bit

  return ((int8_t)I2C2->DR);           // return byte
}

void I2C2_WriteGIR (char Adr, char Dat)    {
  I2C2->CR1         |= 0x0100;       // send START bit
  while (!(I2C2->SR1 & 0x0001)) {};  // wait for START condition (SB=1) 
  I2C2->DR           = 0XD0;         // slave address   -> DR & write
  while (!(I2C2->SR1 & 0x0002)) {};  // wait for ADDRESS sent (ADDR=1) 
	Status2        = I2C2->SR2;    // read status to clear flag
  I2C2->DR           = Adr;          // Address in chip -> DR & write
  while (!(I2C2->SR1 & 0x0080)) {};  // wait for DR empty (TxE)
  I2C2->DR           = Dat;          // Dat -> DR & write
  while (!(I2C2->SR1 & 0x0080)) {};  // wait for DR empty (TxE) 
  while (!(I2C2->SR1 & 0x0004)) {};  // wait for Byte sent (BTF)
  I2C2->CR1         |= 0x0200;       // send STOP bit
}

int8_t I2C2_ReadGIR (char Adr)    {   // procedure: RM0090, pg. 584!
  
  I2C2->CR1         |= 0x0100;       // send START bit
  while (!(I2C2->SR1 & 0x0001)) {};  // wait for START condition (SB=1) 
  I2C2->DR           = 0XD0;         // slave address -> DR      (LSB=1)
  while (!(I2C2->SR1 & 0x0002)) {};  // wait for ADDRESS sent    (ADDR=1) 
  Status2        = I2C2->SR2;    // read SR2 to clear flag
  I2C2->DR           = Adr;          // register in chip -> DR 
  while (!(I2C2->SR1 & 0x0080)) {};  // wait for DR empty        (TxE=1)
  while (!(I2C2->SR1 & 0x0004)) {};  // wait for ByteTransferred (BTF=1) 
  
  I2C2->CR1         |= 0x0100;       // send START bit
  while (!(I2C2->SR1 & 0x0001)) {};  // wait for START condition (SB=1)
	I2C2->DR           = 0XD1;         // slave address -> DR      (LSB=0)
  while (!(I2C2->SR1 & 0x0002)) {};  // wait for ADDRESS sent    (ADDR=1)
  Status4        = I2C2->SR2;    // read status to clear flag
  while (!(I2C2->SR1 & 0x0040)) {};  // wait for ByteReceived    (RxNE=1)
  I2C2->CR1         |= 0x0200;       // send STOP bit

  return ((int8_t)I2C2->DR);           // return byte
}


float CF = 0.00389105058;

uint8_t B2,B3,B4,B5,BE,BF,BC,BD,F6,F7,KK;
//float AC5,AC6,MD,MC,UT;
uint16_t AC5,AC6;
int16_t MC,MD;
long UT;
long T,X1,X2,B55;
uint16_t Tuin;

int8_t AXL,AXH,AYL,AYH,AZL,AZH,TL,TH;
int16_t AX,AY,AZ;
char Tt;
float TT;


void Temp_Read (void){
	B2 = I2C2_ReadBAR(0xB2);  // MSB 
	for (i=0; i<30000; i++) {};
	B3 = I2C2_ReadBAR(0xB3);  // LSB
	for (i=0; i<30000; i++) {};
	B4 = I2C2_ReadBAR(0xB4);  // MSB 
	for (i=0; i<30000; i++) {};
	B5 = I2C2_ReadBAR(0xB5);  // LSB
	for (i=0; i<30000; i++) {};
	BE = I2C2_ReadBAR(0xBE);  // MSB 
	for (i=0; i<30000; i++) {};
	BF = I2C2_ReadBAR(0xBF);  // LSB
	for (i=0; i<30000; i++) {};
	BC = I2C2_ReadBAR(0xBC);  // MSB 
	for (i=0; i<30000; i++) {};
	BD = I2C2_ReadBAR(0xBD);  // LSB
	for (i=0; i<30000; i++) {};
	
	I2C2_WriteBAR(0xF4,0x2E);
	for (i=0; i<30000; i++) {};
	
	F6 = I2C2_ReadBAR(0xF6);  // MSB 
	for (i=0; i<30000; i++) {};
	F7 = I2C2_ReadBAR(0xF7);  // LSB
	for (i=0; i<30000; i++) {};
		
	AC5=((B2 << 8) | B3);
	AC6=((B4 << 8) | B5);
	MD=((BE << 8) | BF);
	MC=((BC << 8) | BD);
	UT=((F6 << 8) | F7);

	//Calculo de temperatura
	X1=(UT-AC6)*AC5/32768;
	X2=MC*2048/(X1+MD);
	B55=X1+X2;
	T=(B55+8)/16;
	Tuin=T;
	UART4->DR = 'T';
	for (i=0; i<30000; i++) {};
	UART4->DR = ':';
	for (i=0; i<30000; i++) {};
	ascii_conv(Tuin);
	for (i=0; i<30000; i++) {};
	UART4->DR = 'e';
	for (i=0; i<30000; i++) {};
	UART4->DR = '^';
	for (i=0; i<30000; i++) {};
	UART4->DR = '-';
	for (i=0; i<30000; i++) {};
	UART4->DR = '1';
	for (i=0; i<30000; i++) {};
	UART4->DR = 248;
	for (i=0; i<30000; i++) {};
	UART4->DR = 'C';
	for (i=0; i<30000; i++) {};
	UART4->DR = 10;
	for (i=0; i<30000; i++) {};
	UART4->DR = 13;
	for (i=0; i<30000; i++) {};
}
void ACCEL_Read (void){
	//KK = I2C2_ReadGIR(0x75);
	I2C2_WriteGIR(0x6B,0x18);
	for (i=0; i<30000; i++) {};
	I2C2_WriteGIR(0x1C,0x00);
	for (i=0; i<30000; i++) {};
	
	AXL=I2C2_ReadGIR(0x3C);
	for (i=0; i<30000; i++) {};
	AXH=I2C2_ReadGIR(0x3B);
	for (i=0; i<30000; i++) {};
	
	AYL=I2C2_ReadGIR(0x3E);
	for (i=0; i<30000; i++) {};
	AYH=I2C2_ReadGIR(0x3D);
	for (i=0; i<30000; i++) {};

	AZL=I2C2_ReadGIR(0x40);
	for (i=0; i<30000; i++) {};
	AZH=I2C2_ReadGIR(0x3F);
	for (i=0; i<30000; i++) {};		
		
	AX=(~((AXH << 8) | AXL))+1;
 	AY=(~((AYH << 8) | AYL))+1;
	AZ=(~((AZH << 8) | AZL))+1;
	
		
	TH=I2C2_ReadGIR(0x41);
	for (i=0; i<300000; i++) {};
	TL=I2C2_ReadGIR(0x42);
	for (i=0; i<300000; i++) {};
	TT=(((TH << 8) | TL)/340)+36.53;
	Tt=(char)TT;
	for (i=0; i<300000; i++) {};
	//UART4->DR = KK;	
	//for (i=0; i<30000; i++) {};
	//AC5=((B2 << 8) | B3);
	UART4->DR = 'W';
	for (i=0; i<30000; i++) {};
	UART4->DR = 'A';
	for (i=0; i<30000; i++) {};
	UART4->DR = 'I';
	for (i=0; i<30000; i++) {};
	UART4->DR = ':';
	for (i=0; i<30000; i++) {};
	ascii_conv(Tt);
	UART4->DR = 10;
	for (i=0; i<30000; i++) {};
	UART4->DR = 13;
	for (i=0; i<30000; i++) {};	
	
}

int main ()  {
	Serial_config();
  RCC->AHB1ENR  |=  0x00000002; // Enable clock for GPIOB
  GPIOB->AFR[1] |=  0x00004400; // select AF4 (I2C) for PB10-SCL,11-SDA -> I2C2
  GPIOB->MODER  |=  0x00a00000; // PB10,11 => alternate functions
  GPIOB->OTYPER |=  0x0c00;     // use open-drain output on these pins!
  I2C2_Init();                  // initialize I2C2 block
  while (1) {
		Temp_Read();
		ACCEL_Read ();		
		for (i=0; i<3000000; i++) {};   // waste time, ~100ms				
  };
}


