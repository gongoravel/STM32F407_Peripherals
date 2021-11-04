//Código de configuración y lectura de 3-Axis Digital Compass IC HMC5883L
//Rango de salida -> 0xF800 - 0X07FF Normalizado/Sin normalizar
//

#include "stm32f4xx.h"

char DD;
int i;
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
  GPIOC->AFR[1] |= 0x00008800;  // select AF8 (UART4,5,6) for PA10, PA11 
  UART4->BRR     = 0x1120;      // (0x1120--9600 baud)  (0x0890--19200)(0x0450--38400)(0x02e0--57600)(0x016d--115200)
  UART4->CR1    |= 0x200c;      // Enable UART for TX, RX , UART
  UART4->CR1    |= 0x0020;      // Enable RX interrupt
  //NVIC init
  NVIC_EnableIRQ(UART4_IRQn);   // Enable IRQ for UART4 in NVIC
  // endless loop
	
}

/*uint8_t b;
void ascii_conv(uint8_t eje){
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
}*/

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

void I2C2_WriteChar (char Adr, char Dat)    {
  I2C2->CR1         |= 0x0100;       // send START bit
  while (!(I2C2->SR1 & 0x0001)) {};  // wait for START condition (SB=1) 
  I2C2->DR           = 0x3C;         // slave address   -> DR & write
  while (!(I2C2->SR1 & 0x0002)) {};  // wait for ADDRESS sent (ADDR=1) 
  Status2        = I2C2->SR2;    // read status to clear flag
  I2C2->DR           = Adr;          // Address in chip -> DR & write
  while (!(I2C2->SR1 & 0x0080)) {};  // wait for DR empty (TxE)
  I2C2->DR           = Dat;          // Dat -> DR & write
  while (!(I2C2->SR1 & 0x0080)) {};  // wait for DR empty (TxE) 
  while (!(I2C2->SR1 & 0x0004)) {};  // wait for Byte sent (BTF)
  I2C2->CR1         |= 0x0200;       // send STOP bit
}

int8_t I2C2_ReadChar (char Adr)    {   // procedure: RM0090, pg. 584!
  
  I2C2->CR1         |= 0x0100;       // send START bit
  while (!(I2C2->SR1 & 0x0001)) {};  // wait for START condition (SB=1) 
  I2C2->DR           = 0x3C;         // slave address -> DR      (LSB=1)
  while (!(I2C2->SR1 & 0x0002)) {};  // wait for ADDRESS sent    (ADDR=1) 
  Status2        = I2C2->SR2;    // read SR2 to clear flag
  I2C2->DR           = Adr;          // register in chip -> DR 
  while (!(I2C2->SR1 & 0x0080)) {};  // wait for DR empty        (TxE=1)
  while (!(I2C2->SR1 & 0x0004)) {};  // wait for ByteTransferred (BTF=1) 
  
  I2C2->CR1         |= 0x0100;       // send START bit
  while (!(I2C2->SR1 & 0x0001)) {};  // wait for START condition (SB=1)
  I2C2->DR           = 0x3D;         // slave address -> DR      (LSB=0)
  while (!(I2C2->SR1 & 0x0002)) {};  // wait for ADDRESS sent    (ADDR=1)
  Status4        = I2C2->SR2;    // read status to clear flag
  while (!(I2C2->SR1 & 0x0040)) {};  // wait for ByteReceived    (RxNE=1)
  I2C2->CR1         |= 0x0200;       // send STOP bit

  return ((int8_t)I2C2->DR);           // return byte
}

short I2C2_ReadShort (char Adr)    {
  
  I2C2->CR1         |= 0x0100;       // send START bit
  while (!(I2C2->SR1 & 0x0001)) {};  // wait for START condition (SB=1) 
  I2C2->DR           = 0x3C;         // slave address -> DR      (LSB=1)
  while (!(I2C2->SR1 & 0x0002)) {};  // wait for ADDRESS sent    (ADDR=1) 
  Status2        = I2C2->SR2;    // read SR2 to clear flag
  I2C2->DR           = Adr;          // register in chip -> DR 
  while (!(I2C2->SR1 & 0x0080)) {};  // wait for DR empty        (TxE=1)
  while (!(I2C2->SR1 & 0x0004)) {};  // wait for ByteTransferred (BTF=1) 
  
  I2C2->CR1         |= 0x0100;       // send START bit
  while (!(I2C2->SR1 & 0x0001)) {};  // wait for START condition (SB=1)
  I2C2->DR           = 0x3D;         // slave address -> DR      (LSB=0)
  while (!(I2C2->SR1 & 0x0002)) {};  // wait for ADDRESS sent    (ADDR=1)
  I2C2->CR1         |= 0x0800;       // POS enable
  Status4        = I2C2->SR2;    // read status to clear flag
  while (!(I2C2->SR1 & 0x0004)) {};  // wait for ByteReceived    (BTF=1)
  I2C2->CR1         |= 0x0200;       // send STOP bit
	LECTURA = I2C2->DR;            // safe place
  return ((short)(LECTURA));  // return combined bytes
	
	//76x1 = I2C2->DR << 8;          // safe place
  //x2 = I2C2->DR;			
  //return ((uint8_t)(x1+x2));  // return combined bytes
}


float CF = 0.00389105058;

int8_t X1,X2,Y1,Y2,Z1,Z2;
uint8_t STATUS;
short X,Y,Z;
float XT,YT,ZT;
//uint8_t XS, YS, ZS;
uint16_t XS, YS, ZS;
int main ()  {
	Serial_config();
  RCC->AHB1ENR  |=  0x00000002; // Enable clock for GPIOB
  GPIOB->AFR[1] |=  0x00004400; // select AF4 (I2C) for PB10,11 -> I2C2
  GPIOB->MODER  |=  0x00a00000; // PB10,11 => alternate functions
  GPIOB->OTYPER |=  0x0c00;     // use open-drain output on these pins!

  // Init accelerometer chip
  I2C2_Init();                  // initialize I2C2 block
  I2C2_WriteChar(0x00,0x10);    // wake-up!
	for (i=0; i<30000; i++) {};
  I2C2_WriteChar(0x01,0x20);    // Gyro full scale = +/-1000 deg/s 
	for (i=0; i<30000; i++) {};
	I2C2_WriteChar(0x02,0x00);
	for (i=0; i<300000; i++) {};
  // endless loop
  while (1) {
		STATUS = I2C2_ReadChar(0x09);  // read acc in z axis
		for (i=0; i<30000; i++) {};
		X1 = I2C2_ReadChar(0x03);  // read acc in z axis 
		for (i=0; i<30000; i++) {};
		X2 = I2C2_ReadChar(0x04);  // read acc in z axis 
		for (i=0; i<30000; i++) {};
		Y1 = I2C2_ReadChar(0x05);  // read acc in z axis 
		for (i=0; i<30000; i++) {};
		Y2 = I2C2_ReadChar(0x06);  // read acc in z axis 
		for (i=0; i<30000; i++) {};
		Z1 = I2C2_ReadChar(0x07);  // read acc in z axis 
		for (i=0; i<30000; i++) {};
		Z2 = I2C2_ReadChar(0x08);  // read acc in z axis 
		for (i=0; i<300000; i++) {};

				
		X=((X1 << 8) | X2);
		Y=((Y1 << 8) | Y2);
		Z=((Z1 << 8) | Z2);
			
		if (X < 0){
			X = -1*X;
			XS = (uint16_t)X;		
			UART4->DR = 'X';
			for (i=0; i<30000; i++) {};
			UART4->DR = ':';
			for (i=0; i<30000; i++) {};
			UART4->DR = '-';
			for (i=0; i<30000; i++) {};
			ascii_conv(XS);
			for (i=0; i<30000; i++) {};
			UART4->DR = 9;
			for (i=0; i<30000; i++) {};
		}else{ 
			XS = (uint16_t)X;
			UART4->DR = 'X';
			for (i=0; i<30000; i++) {};
			UART4->DR = ':';
			for (i=0; i<30000; i++) {};
			UART4->DR = '+';
			for (i=0; i<30000; i++) {};
			ascii_conv(XS);
			for (i=0; i<30000; i++) {};
			UART4->DR = 9;
			for (i=0; i<30000; i++) {};
		}	
		if (Y < 0){
			Y = -1*Y;
			YS = (uint8_t)Y;	
			UART4->DR = 'Y';
			for (i=0; i<30000; i++) {};
			UART4->DR = ':';
			for (i=0; i<30000; i++) {};
			UART4->DR = '-';
			for (i=0; i<30000; i++) {};
			ascii_conv(YS);
			for (i=0; i<30000; i++) {};
			UART4->DR = 9;
			for (i=0; i<30000; i++) {};
		}else {
			Y = (uint8_t)Y;
			UART4->DR = 'Y';
			for (i=0; i<30000; i++) {};
			UART4->DR = ':';
			for (i=0; i<30000; i++) {};
			UART4->DR = '+';
			for (i=0; i<30000; i++) {};
			ascii_conv(YS);
			for (i=0; i<30000; i++) {};
			UART4->DR = 9;
			for (i=0; i<30000; i++) {};
		}
		if (Z < 0){
			Z = -1*Z;
			ZS = (uint8_t)Z;	
			UART4->DR = 'Z';
			for (i=0; i<30000; i++) {};
			UART4->DR = ':';
			for (i=0; i<30000; i++) {};
			UART4->DR = '-';
			for (i=0; i<30000; i++) {};
			ascii_conv(ZS);
			for (i=0; i<30000; i++) {};
			UART4->DR = 10;
			for (i=0; i<30000; i++) {};
			UART4->DR = 13;
			for (i=0; i<30000; i++) {};
		}else {
			Z = (uint8_t)Z;	
			UART4->DR = 'Z';
			for (i=0; i<30000; i++) {};
			UART4->DR = ':';
			for (i=0; i<30000; i++) {};
			UART4->DR = '+';
			for (i=0; i<30000; i++) {};
			ascii_conv(ZS);
			for (i=0; i<30000; i++) {};
			UART4->DR = 10;
			for (i=0; i<30000; i++) {};
			UART4->DR = 13;
			for (i=0; i<30000; i++) {};
		}
//**********Conversion a Byte****************//		
		/*X=(X1 << 8 | X2);
		if (X > 0x7FF){
			XT = X*-255/-2048; 
		}else XT=X*255/2047;			
		Y=(Y1 << 8 | Y2);
		if (Y > 0x7FF){
			YT = Y*-255/-2048; 
		}else YT=Y*255/2047;
		Z=(Z1 << 8 | Z2);
		if (Z > 0x7FF){
			ZT = Z*-255/-2048; 
		}else ZT=Z*255/2047;
			
		if (XT < 0){
			XT = -1*XT;
			XS = (uint8_t)XT;		
			UART4->DR = 'X';
			for (i=0; i<30000; i++) {};
			UART4->DR = ':';
			for (i=0; i<30000; i++) {};
			UART4->DR = '-';
			for (i=0; i<30000; i++) {};
			ascii_conv(XS);
			for (i=0; i<30000; i++) {};
			UART4->DR = 9;
			for (i=0; i<30000; i++) {};
		}else{ 
			XS = (uint8_t)XT;
			UART4->DR = 'X';
			for (i=0; i<30000; i++) {};
			UART4->DR = ':';
			for (i=0; i<30000; i++) {};
			UART4->DR = '+';
			for (i=0; i<30000; i++) {};
			ascii_conv(XS);
			for (i=0; i<30000; i++) {};
			UART4->DR = 9;
			for (i=0; i<30000; i++) {};
		}	
		if (YT < 0){
			YT = -1*YT;
			YS = (uint8_t)YT;	
			UART4->DR = 'Y';
			for (i=0; i<30000; i++) {};
			UART4->DR = ':';
			for (i=0; i<30000; i++) {};
			UART4->DR = '-';
			for (i=0; i<30000; i++) {};
			ascii_conv(YS);
			for (i=0; i<30000; i++) {};
			UART4->DR = 9;
			for (i=0; i<30000; i++) {};
		}else {
			YS = (uint8_t)YT;
			UART4->DR = 'Y';
			for (i=0; i<30000; i++) {};
			UART4->DR = ':';
			for (i=0; i<30000; i++) {};
			UART4->DR = '+';
			for (i=0; i<30000; i++) {};
			ascii_conv(YS);
			for (i=0; i<30000; i++) {};
			UART4->DR = 9;
			for (i=0; i<30000; i++) {};
		}
		if (ZT < 0){
			ZT = -1*ZT;
			ZS = (uint8_t)ZT;	
			UART4->DR = 'Z';
			for (i=0; i<30000; i++) {};
			UART4->DR = ':';
			for (i=0; i<30000; i++) {};
			UART4->DR = '-';
			for (i=0; i<30000; i++) {};
			ascii_conv(ZS);
			for (i=0; i<30000; i++) {};
			UART4->DR = 10;
			for (i=0; i<30000; i++) {};
			UART4->DR = 13;
			for (i=0; i<30000; i++) {};
		}else {
			ZS = (uint8_t)ZT;	
			UART4->DR = 'Z';
			for (i=0; i<30000; i++) {};
			UART4->DR = ':';
			for (i=0; i<30000; i++) {};
			UART4->DR = '+';
			for (i=0; i<30000; i++) {};
			ascii_conv(ZS);
			for (i=0; i<30000; i++) {};
			UART4->DR = 10;
			for (i=0; i<30000; i++) {};
			UART4->DR = 13;
			for (i=0; i<30000; i++) {};
		}*/
		
		for (i=0; i<3000000; i++) {};   // waste time, ~100ms		

		
  };
}


