#include "stm32f4xx.h"
long i=0;
uint8_t j=0;
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
  UART4->BRR     = 0x016d;      // (0x1120--9600 baud)  (0x0890--19200)(0x0450--38400)(0x02e0--57600)(0x016d--115200)
  UART4->CR1    |= 0x200c;      // Enable UART for TX, RX , UART
  UART4->CR1    |= 0x0020;      // Enable RX interrupt
  //NVIC init
  NVIC_EnableIRQ(UART4_IRQn);   // Enable IRQ for UART4 in NVIC
  // endless loop
	
}

void ESP8622_Config(void){
	UART4->DR = 'A';
	for (i=0; i<20000; i++)       {};
	UART4->DR = 'T';
	for (i=0; i<20000; i++)       {};
	UART4->DR = '+';
	for (i=0; i<20000; i++)       {};
	UART4->DR = 'C';
	for (i=0; i<20000; i++)       {};
	UART4->DR = 'W';
	for (i=0; i<20000; i++)       {};
	UART4->DR = 'M';
	for (i=0; i<20000; i++)       {};
	UART4->DR = 'O';
	for (i=0; i<20000; i++)       {};
	UART4->DR = 'D';	
	for (i=0; i<20000; i++)       {};
	UART4->DR = 'E';
	for (i=0; i<20000; i++)       {};
	UART4->DR = '=';	
	for (i=0; i<20000; i++)       {};
	UART4->DR = '3';
	for (i=0; i<20000; i++)       {};
	UART4->DR = 13;
	for (i=0; i<20000; i++)       {};
	UART4->DR = 10;
	for (i=0; i<20000; i++)       {};
		
	for (i=0; i<2000000; i++)       {};
		
		
	UART4->DR = 'A';
	for (i=0; i<20000; i++)       {};
	UART4->DR = 'T';
	for (i=0; i<20000; i++)       {};
	UART4->DR = '+';
	for (i=0; i<20000; i++)       {};
	UART4->DR = 'C';
	for (i=0; i<20000; i++)       {};
	UART4->DR = 'I';
	for (i=0; i<20000; i++)       {};
	UART4->DR = 'P';
	for (i=0; i<20000; i++)       {};
	UART4->DR = 'M';
	for (i=0; i<20000; i++)       {};
	UART4->DR = 'U';	
	for (i=0; i<20000; i++)       {};
	UART4->DR = 'X';
	for (i=0; i<20000; i++)       {};
	UART4->DR = '=';	
	for (i=0; i<20000; i++)       {};
	UART4->DR = '1';
	for (i=0; i<20000; i++)       {};
	UART4->DR = 13;
	for (i=0; i<20000; i++)       {};
	UART4->DR = 10;
	for (i=0; i<20000; i++)       {};
		
	
	for (i=0; i<2000000; i++)       {};	
		
	UART4->DR = 'A';
	for (i=0; i<20000; i++)       {};
	UART4->DR = 'T';
	for (i=0; i<20000; i++)       {};
	UART4->DR = '+';
	for (i=0; i<20000; i++)       {};
	UART4->DR = 'C';
	for (i=0; i<20000; i++)       {};
	UART4->DR = 'I';
	for (i=0; i<20000; i++)       {};
	UART4->DR = 'P';
	for (i=0; i<20000; i++)       {};
	UART4->DR = 'S';
	for (i=0; i<20000; i++)       {};
	UART4->DR = 'E';	
	for (i=0; i<20000; i++)       {};
	UART4->DR = 'R';
	for (i=0; i<20000; i++)       {};
	UART4->DR = 'V';	
	for (i=0; i<20000; i++)       {};
	UART4->DR = 'E';
	for (i=0; i<20000; i++)       {};
	UART4->DR = 'R';
	for (i=0; i<20000; i++)       {};
	UART4->DR = '=';
	for (i=0; i<20000; i++)       {};
	UART4->DR = '1';
	for (i=0; i<20000; i++)       {};
	UART4->DR = ',';	
	for (i=0; i<20000; i++)       {};
	UART4->DR = '8';
	for (i=0; i<20000; i++)       {};
	UART4->DR = '0';
	for (i=0; i<20000; i++)       {};
	UART4->DR = 13;
	for (i=0; i<20000; i++)       {};
	UART4->DR = 10;
	for (i=0; i<20000; i++)       {};
		
	
	for (i=0; i<2000000; i++)       {};
}


uint8_t b;
void Data (void){
	UART4->DR = 'A';
	for (i=0; i<20000; i++)       {};
	UART4->DR = 'T';
	for (i=0; i<20000; i++)       {};
	UART4->DR = '+';
	for (i=0; i<20000; i++)       {};
	UART4->DR = 'C';
	for (i=0; i<20000; i++)       {};
	UART4->DR = 'I';
	for (i=0; i<20000; i++)       {};
	UART4->DR = 'P';
	for (i=0; i<20000; i++)       {};
	UART4->DR = 'S';
	for (i=0; i<20000; i++)       {};
	UART4->DR = 'E';	
	for (i=0; i<20000; i++)       {};
	UART4->DR = 'N';
	for (i=0; i<20000; i++)       {};
	UART4->DR = 'D';
	for (i=0; i<20000; i++)       {};
	UART4->DR = '=';	
	for (i=0; i<20000; i++)       {};
	UART4->DR = '0';
	for (i=0; i<20000; i++)       {};
	UART4->DR = ',';
	for (i=0; i<20000; i++)       {};
	UART4->DR = '3';
	for (i=0; i<20000; i++)       {};
	/*UART4->DR = '0';
	for (i=0; i<20000; i++)       {};*/
	UART4->DR = 13;
	for (i=0; i<20000; i++)       {};
	UART4->DR = 10;
	for (i=0; i<20000; i++)       {};
	
	
	for (i=0; i<2000000; i++)       {};	
	
		
	b=j%1000/100+48;
	UART4->DR = b;
	for (i=0; i<30000; i++) {};
	b=j%100/10+48;
	for (i=0; i<30000; i++) {};
	UART4->DR = b;
	for (i=0; i<30000; i++) {};
	b=j%10+48;
	UART4->DR = b;
	for (i=0; i<30000; i++) {};
	
	
	
	for (i=0; i<2000000; i++)       {};
		
	UART4->DR = 'A';
	for (i=0; i<20000; i++)       {};
	UART4->DR = 'T';
	for (i=0; i<20000; i++)       {};
	UART4->DR = '+';
	for (i=0; i<20000; i++)       {};
	UART4->DR = 'C';
	for (i=0; i<20000; i++)       {};
	UART4->DR = 'I';
	for (i=0; i<20000; i++)       {};
	UART4->DR = 'P';
	for (i=0; i<20000; i++)       {};
	UART4->DR = 'C';
	for (i=0; i<20000; i++)       {};
	UART4->DR = 'L';	
	for (i=0; i<20000; i++)       {};
	UART4->DR = 'O';
	for (i=0; i<20000; i++)       {};
	UART4->DR = 'S';	
	for (i=0; i<20000; i++)       {};
	UART4->DR = 'E';
	for (i=0; i<20000; i++)       {};
	UART4->DR = '=';	
	for (i=0; i<20000; i++)       {};
	UART4->DR = '0';
	for (i=0; i<20000; i++)       {};
	UART4->DR = 13;
	for (i=0; i<20000; i++)       {};
	UART4->DR = 10;
	for (i=0; i<20000; i++)       {};
	
		
	
	for (i=0; i<2000000; i++)       {};
}


int main ()  {

	Serial_config();
	ESP8622_Config();
	for (i=0; i<30000; i++)       {};
  while (1) {
		j=j+1;
		Data();
		
		
		GPIOD->ODR = 0x5555;
    for (i=0; i<800000; i++)       {};             // waste time, ~30ms
		GPIOD->ODR = 0x0000;
		for (i=0; i<800000; i++)       {};             // waste time, ~30ms
  };
}
