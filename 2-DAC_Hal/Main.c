#include "stm32f4xx.h"                  // Device header
#include "RTE_Components.h"             // Component selection
#include "cmsis_os.h"                   // ARM::CMSIS:RTOS:Keil RTX
#include "RTE_Device.h"                 // Keil::Device:STM32Cube Framework:Classic
#include "stm32f4xx_hal_conf.h"         // Keil::Device:STM32Cube Framework:Classic
#include "stm32f4xx_hal.h"              // Keil::Device:STM32Cube HAL:Common 
#include "Board_Buttons.h"              // ::Board Support:Buttons
#include "Board_LED.h"                  // ::Board Support:LED



DAC_HandleTypeDef    DacHandle;
static DAC_ChannelConfTypeDef sConfig;
const uint8_t aEscalator8bit[6] = {0x0, 0x33, 0x66, 0x99, 0xCC, 0xFF};

/* Private function prototypes -----------------------------------------------*/
static void DAC_Ch1_TriangleConfig(void);
static void TIM6_Config(void);
GPIO_InitTypeDef		GPIO_InitStruct;	

static void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;

  /* Enable Power Control clock */
  __HAL_RCC_PWR_CLK_ENABLE();
  
  /* The voltage scaling allows optimizing the power consumption when the device is 
     clocked below the maximum system frequency, to update the voltage scaling value 
     regarding system frequency refer to product datasheet.  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  
  /* Enable HSE Oscillator and activate PLL with HSE as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  HAL_RCC_OscConfig(&RCC_OscInitStruct);
  
  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 
     clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;  
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;  
  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5);

  /* STM32F405x/407x/415x/417x Revision Z devices: prefetch is supported  */
  if (HAL_GetREVID() == 0x1001)
  {
    /* Enable the Flash prefetch */
    __HAL_FLASH_PREFETCH_BUFFER_ENABLE();
  }
}


int main(void)
{
  HAL_Init();
	SystemClock_Config();
  Buttons_Initialize();  
  LED_Initialize();   
	
	
	
	__HAL_RCC_GPIOA_CLK_ENABLE();
	GPIO_InitStruct.Pin = GPIO_PIN_All;
	GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
  /*##-1- Configure the DAC peripheral #######################################*/
  DacHandle.Instance = DAC;
      
  /*##-2- Configure the TIM peripheral #######################################*/
  TIM6_Config();
  
  /* Infinite loop */
  while (1)
  {
    /* If the USER Button is pressed */
    
      HAL_DAC_DeInit(&DacHandle);
      DAC_Ch1_TriangleConfig();
		
			LED_On(1);    
			osDelay(200); 
			LED_Off(1);   
			osDelay(200); 
    
  }
}


static void DAC_Ch1_TriangleConfig(void)
{
	__DAC_CLK_ENABLE();
  HAL_DAC_Init(&DacHandle);
	sConfig.DAC_Trigger = DAC_TRIGGER_T6_TRGO;
  sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;  
	HAL_DAC_ConfigChannel(&DacHandle, &sConfig, DAC_CHANNEL_1);
  HAL_DACEx_TriangleWaveGenerate(&DacHandle, DAC_CHANNEL_1, DAC_TRIANGLEAMPLITUDE_1023);
	HAL_DAC_Start(&DacHandle, DAC_CHANNEL_1);
	HAL_DAC_SetValue(&DacHandle, DAC_CHANNEL_1, DAC_ALIGN_12B_R, 0x100);
}


void TIM6_Config(void)
{
  static TIM_HandleTypeDef  htim;
  TIM_MasterConfigTypeDef sMasterConfig;
  
  /*##-1- Configure the TIM peripheral #######################################*/
  /* Time base configuration */
  htim.Instance = TIM6;
  
  htim.Init.Period = 0x7FF;          
  htim.Init.Prescaler = 0;       
  htim.Init.ClockDivision = 0;    
  htim.Init.CounterMode = TIM_COUNTERMODE_UP; 
  HAL_TIM_Base_Init(&htim);

  /* TIM6 TRGO selection */
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  
  HAL_TIMEx_MasterConfigSynchronization(&htim, &sMasterConfig);
  
  /*##-2- Enable TIM peripheral counter ######################################*/
  HAL_TIM_Base_Start(&htim);
}

