/*!
 @file init.c
 Initializes the board's peripherals by starting up and setting up LEDs, the 
 Temperature sensor and the User Button.
*/

/********************************************//**
 *  Includes
 ***********************************************/
#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"
#include <stdio.h>
#include "init.h"


/********************************************//**
 *  Functions
 ***********************************************/

/**
 * Initializes LEDs, Temperature Sensor and User Button
 *
 * Starts by enabling power to the peripherals, initializing the LEDs and then the
 * button and temperature sensor.
 *
 */
void init_sensor(){

	// Initialize LEDS
	GPIO_InitTypeDef gpio_init_led;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

	GPIO_StructInit(&gpio_init_led);
	gpio_init_led.GPIO_Pin   = GPIO_Pin_12 | GPIO_Pin_13| GPIO_Pin_14| GPIO_Pin_15;
	gpio_init_led.GPIO_Mode  = GPIO_Mode_OUT;     // Set as output
	gpio_init_led.GPIO_Speed = GPIO_Speed_100MHz; // Don't limit slew rate
	gpio_init_led.GPIO_OType = GPIO_OType_PP;     // Push-pull
	gpio_init_led.GPIO_PuPd  = GPIO_PuPd_NOPULL;  // Not input, don't pull
	
	// Actually configure that pin
	GPIO_Init(GPIOD, &gpio_init_led);
	
	// Initialize Button
	GPIO_InitTypeDef gpio_init_button;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

	GPIO_StructInit(&gpio_init_button);
	gpio_init_button.GPIO_Pin   = GPIO_Pin_0;
	gpio_init_button.GPIO_Mode  = GPIO_Mode_IN;     // Set as output
	gpio_init_button.GPIO_Speed = GPIO_Speed_50MHz; // Don't limit slew rate
	gpio_init_button.GPIO_OType = GPIO_OType_PP;     // Push-pull
	gpio_init_button.GPIO_PuPd  = GPIO_PuPd_DOWN;  // Not input, don't pull
	
	// Actually configure that pin
	GPIO_Init(GPIOA, &gpio_init_button);
	
	// Initialize Temperature sensor
	ADC_InitTypeDef       adc_init_s;
	ADC_CommonInitTypeDef adc_common_init_s;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	adc_common_init_s.ADC_Mode = ADC_Mode_Independent;
	adc_common_init_s.ADC_Prescaler = ADC_Prescaler_Div2;
	adc_common_init_s.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
	adc_common_init_s.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
	ADC_CommonInit(&adc_common_init_s);
	adc_init_s.ADC_Resolution = ADC_Resolution_12b;
	adc_init_s.ADC_ScanConvMode = DISABLE;
	adc_init_s.ADC_ContinuousConvMode = DISABLE;
	adc_init_s.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
	adc_init_s.ADC_DataAlign = ADC_DataAlign_Right;
	adc_init_s.ADC_NbrOfConversion = 1;
	ADC_Init(ADC1, &adc_init_s);
	ADC_Cmd(ADC1, ENABLE);
	ADC_TempSensorVrefintCmd(ENABLE);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_16, 1, ADC_SampleTime_480Cycles);
	ADC_SoftwareStartConv(ADC1);

}
