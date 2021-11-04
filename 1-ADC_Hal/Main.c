///* Programa para conversión ADC utilizando la librería HAL que ofrece STM para compiladores como Keil, GCC, IAR... etc *////

#include <stdio.h>
#include "stm32f4xx_hal.h"              // Keil::Device:STM32Cube HAL:Common
#include "stm32f4xx.h"                  // Device header
#include "RTE_Components.h"             // Component selection
#include "RTE_Device.h"                 // Keil::Device:STM32Cube Framework:Classic
#include "cmsis_os.h"
#include "stm32f4xx_hal_conf.h"         // Keil::Device:STM32Cube Framework:Classic
#include "stm32f4xx_hal_adc.h"     

ADC_HandleTypeDef    AdcHandle;																																				//Definición de prototipo de configuración de modulo ADC
ADC_ChannelConfTypeDef sConfig;																																				//Definición de prototipo de configuración de pines de ADC

GPIO_InitTypeDef		GPIO_InitStruct;																																	//Definición para uso de GPIOD como salida de conversión
GPIO_InitTypeDef		GPIO_ADC;																																					//Definición para pines ADC-PC0-PC1-PC2

__IO uint16_t CONVERSION = 0;																																					//Variable de conversión ADC
void adc_config()
{
	 AdcHandle.Instance = ADC1;
	 
	__ADC1_CLK_ENABLE();																																								//Habilitación de reloj para ADC1
	__GPIOC_CLK_ENABLE();																																								//Habilitación de reloj para puerto C	

	GPIO_ADC.Pin = GPIO_PIN_0| GPIO_PIN_1|GPIO_PIN_2;																										//Conf-Pines de ADC
	GPIO_ADC.Mode = GPIO_MODE_ANALOG;																																		//Conf- pines como analogos
	HAL_GPIO_Init(GPIOC, &GPIO_ADC);																																		//Inicialización de pines
	
	//*CONFIGIGURACIÓN DE MODULO ADC*//
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

	HAL_ADC_Init(&AdcHandle);																																						//Inicialización de Modulo ADC

	sConfig.Channel = ADC_CHANNEL_12; 																																	//Pin de conversion
	sConfig.Rank = 1;
	sConfig.SamplingTime = ADC_SAMPLETIME_480CYCLES;
	sConfig.Offset = 0;

	HAL_ADC_ConfigChannel(&AdcHandle, &sConfig);																												//Configuración de canal asociado a PIN

}


//*Función para conversión ADC*//
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
	HAL_Init();																																													//Inicialización de Librería HAL
	adc_config();																																												//Configuración ADC
	GPIO_CONFIG();
	

	while(1){
		adc_convert();		
		GPIOA-> ODR=CONVERSION;	
		osDelay(200);
	}
}
