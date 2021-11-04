///* Programa para conversi�n ADC utilizando la librer�a HAL que ofrece STM para compiladores como Keil, GCC, IAR... etc *////

#include <stdio.h>
#include "stm32f4xx_hal.h"              // Keil::Device:STM32Cube HAL:Common
#include "stm32f4xx.h"                  // Device header
#include "RTE_Components.h"             // Component selection
#include "RTE_Device.h"                 // Keil::Device:STM32Cube Framework:Classic
#include "cmsis_os.h"
#include "stm32f4xx_hal_conf.h"         // Keil::Device:STM32Cube Framework:Classic
#include "stm32f4xx_hal_adc.h"     

ADC_HandleTypeDef    AdcHandle;																																				//Definici�n de prototipo de configuraci�n de modulo ADC
ADC_ChannelConfTypeDef sConfig;																																				//Definici�n de prototipo de configuraci�n de pines de ADC

GPIO_InitTypeDef		GPIO_InitStruct;																																	//Definici�n para uso de GPIOD como salida de conversi�n
GPIO_InitTypeDef		GPIO_ADC;																																					//Definici�n para pines ADC-PC0-PC1-PC2

__IO uint16_t CONVERSION = 0;																																					//Variable de conversi�n ADC
void adc_config()
{
	 AdcHandle.Instance = ADC1;
	 
	__ADC1_CLK_ENABLE();																																								//Habilitaci�n de reloj para ADC1
	__GPIOC_CLK_ENABLE();																																								//Habilitaci�n de reloj para puerto C	

	GPIO_ADC.Pin = GPIO_PIN_0| GPIO_PIN_1|GPIO_PIN_2;																										//Conf-Pines de ADC
	GPIO_ADC.Mode = GPIO_MODE_ANALOG;																																		//Conf- pines como analogos
	HAL_GPIO_Init(GPIOC, &GPIO_ADC);																																		//Inicializaci�n de pines
	
	//*CONFIGIGURACI�N DE MODULO ADC*//
	AdcHandle.Init.ClockPrescaler = ADC_CLOCKPRESCALER_PCLK_DIV2;
	AdcHandle.Init.Resolution = ADC_RESOLUTION_12B;
	AdcHandle.Init.ScanConvMode = DISABLE;
	AdcHandle.Init.ContinuousConvMode = ENABLE;
	AdcHandle.Init.DiscontinuousConvMode = DISABLE;
	AdcHandle.Init.NbrOfDiscConversion = 0;
	AdcHandle.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
	AdcHandle.Init.ExternalTrigConv = ADC_EXTERNALTRIGCONV_T1_CC1;
	AdcHandle.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	AdcHandle.Init.NbrOfConversion = 1;
	AdcHandle.Init.DMAContinuousRequests = DISABLE;
	AdcHandle.Init.EOCSelection = DISABLE;

	HAL_ADC_Init(&AdcHandle);																																						//Inicializaci�n de Modulo ADC

	sConfig.Channel = ADC_CHANNEL_12; 																																	//Pin de conversion
	sConfig.Rank = 1;
	sConfig.SamplingTime = ADC_SAMPLETIME_480CYCLES;
	sConfig.Offset = 0;

	HAL_ADC_ConfigChannel(&AdcHandle, &sConfig);																												//Configuraci�n de canal asociado a PIN

}


//*Funci�n para conversi�n ADC*//
int adc_convert(){
		HAL_ADC_Start(&AdcHandle);																																				//Start the conversion
		HAL_ADC_PollForConversion(&AdcHandle,100);														  														//Processing the conversion
		CONVERSION= HAL_ADC_GetValue(&AdcHandle); 																												//Return the converted data
		return (CONVERSION);		
}

void GPIO_CONFIG(){
	__HAL_RCC_GPIOA_CLK_ENABLE();
	GPIO_InitStruct.Pin = GPIO_PIN_All;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

int main (void){
	HAL_Init();																																													//Inicializaci�n de Librer�a HAL
	adc_config();																																												//Configuraci�n ADC
	GPIO_CONFIG();
	

	while(1){
		adc_convert();		
		GPIOA-> ODR=CONVERSION;	
		osDelay(200);
	}
}
