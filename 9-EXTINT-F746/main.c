#include <stdio.h>
#include "stm32f7xx.h"                  // Device header
static void SystemClock_Config(void);
long i=0;

void EXTI9_5_IRQHandler(void){										// EXTI0 on PB0
	if (EXTI->PR & 0x200) {            
		EXTI->PR |= 0x200; 
		GPIOB->ODR = ~GPIOB->ODR;		
	}
}


void GPIO_Init(void){
  RCC->AHB1ENR  |=  0x00000002; 			// Enable clock for GPIOF
  GPIOB->MODER	|= 0x55555555;
	
	//ENTINT Configuration
	//External Interrupt Config.
	RCC->APB2ENR |= 0x4000;												// Enable SYSCONFIG
	SYSCFG->EXTICR[2] = 0x0040;										// Configuring PE9 as EXTI9
	EXTI->IMR  |= 0x0200;
	EXTI->RTSR |= 0x0200;
	NVIC_EnableIRQ(EXTI9_5_IRQn);	
//	RCC->AHB1ENR  |= 0x00000001;   // Enable clock for GPIOD
//	GPIOA->MODER  |= 0xAAAAAAAA;
	RCC->AHB1ENR  |= 0x00000010;   // Enable clock for GPIOE
	GPIOE->MODER  |= 0xAAAAAAAA;

}


int jj=0;
int main ()  {
	SystemClock_Config();
	GPIO_Init();                 // initialize I2C2 block	
  while(1) {
  };
}


static void SystemClock_Config(void)
{
  RCC->CR |= ((uint32_t)RCC_CR_HSION);                     /* Enable HSI */
  while ((RCC->CR & RCC_CR_HSIRDY) == 0);                  /* Wait for HSI Ready */

  RCC->CFGR = RCC_CFGR_SW_HSI;                             /* HSI is system clock */
  while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSI);  /* Wait for HSI used as system clock */
  RCC->CFGR |= (RCC_CFGR_HPRE_DIV1  |                      /* HCLK = SYSCLK */
                RCC_CFGR_PPRE1_DIV1 |                      /* APB1 = HCLK/2 */
                RCC_CFGR_PPRE2_DIV1  );                    /* APB2 = HCLK/1 */
  RCC->CR &= ~RCC_CR_PLLON;                                /* Disable PLL */
}








