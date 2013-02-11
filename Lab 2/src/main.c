	/*!
 @file main.c
 Initializes all the sensors, buttons and LEDs, as well as passes on sensor data
 onto filter modules using the returned result as a paramter to light up
 the LEDs
*/

/* TO-DO
 * use printf to output numbers to console for plotting
 * have plots with different window depths
*/


/********************************************//**
 *  Includes
 ***********************************************/
#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"
#include <stdio.h>
#include "moving_average.h"
#include "init.h"
#include "utils.h"

/********************************************//**
 *  Defines 
 ***********************************************/

 #define CHOICE (1) //!< Setting to 1 runs C, 0 runs Assembly
 
/********************************************//**
 *  Global Variables & Constants 
 ***********************************************/

float temperature;										//!< Window of last DEPTH data samples collected
float tempValue = 0.0;								//!< Window of last DEPTH data samples collected
static volatile uint_fast16_t ticks;	//!< 20Hz Binary toggle that signals new sample availability
float mov_avg = 0.0; 									//!< Window of last DEPTH data samples collected 
uint_fast8_t ledMode = 0; 						//!< Sets LED mode: 0 is gradient scale, 1 is flashing

/********************************************//**
 *  Assembly Prototypes
 ***********************************************/

/**
 * First link to the assembly moving average initialization label and then to
 * the actual filter implementation
 */
extern void moving_average_init(MaState_t *ss);
extern float moving_average(MaState_t *ss, float temperature, float test_vector);

/********************************************//**
 *  Private Functions
 ***********************************************/
 
/**
 * Sets the right LEDs ON
 *
 * Firstly, all LEDs get reset. The 4 LEDs operate in two modes. When ledMode is 1,
 * the lights just flash continuously and meaninglessly. 
 * When ledMode is 0, this function sets the correct LED on
 * according to the latest moving average value. Each LED represenst a 2-degree
 * range, looping around as the temperature overflows the 8 degree range 
 * available using four LEDs in such a configuration.
 * 
 * @param temp latest moving average temperature calculated cast into an int
 */
void outputLight(int temp){
	if(!ledMode) {						// if ledMode is 0
		GPIO_ResetBits(GPIOD,GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);	//Reset all LEDs
		if(temp%8>=0 && temp%8<=1) {	// if current value mod 8 is between 0 and 1, set blue
			GPIO_SetBits(GPIOD,GPIO_Pin_15);
	}	
		if(temp%8>=2 && temp%8<=3) { // if current value mod 8 is between 2 and 3, set green
			GPIO_SetBits(GPIOD,GPIO_Pin_12);
		}
		if(temp%8>=4 && temp%8<=5) { // if current value mod 8 is between 4 and 5, set orange
			GPIO_SetBits(GPIOD,GPIO_Pin_13);
	}
		if(temp%8>=6 && temp%8<=7) {	 // if current value mod 8 is between 6 and 7, set red
			GPIO_SetBits(GPIOD,GPIO_Pin_14);
	}
} else {	//if ledMode is 1
	GPIO_ResetBits(GPIOD,GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);	//reset all LEds
	Delay(5000000L);	//wait
	GPIO_SetBits(GPIOD, GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);	// set all of them
	Delay(5000000L);	//wait
}
}
/**
 * Calls the mvoing average filter function
 *
 * Calls either the assembly or C implementation of the moving average function 
 * depending on whether CHOICE is set or not.
 * 
 * @param temp latest sampled temperature value from the sensor
 * @param *ss	pointer to the state
 * @return m the up-to-date moving average
 */	
float ma_filter(float temp, MaState_t *ss){
// Performance for one moving average at depth = 3
// C   -> 5 microseconds
// ASM -> 3.2 microseconds
				float m;
				if(CHOICE) {
					// C implementation
					m = mov_average(ss, temp);
				}else {
					// Assembly implementation
					m = moving_average(ss, temperature, temp);
				}
				return m;
}
/**
 * Toggles the ledMode variable
 */
void toggle_ledmode(){
  if(ledMode){
		ledMode =  0;
	} else {
	  ledMode =  1;
  }
}

/**
 * Main function for lab 2
 *
 * Main firstly initializes all the sensors, declares a new state for the
 * moving average filter and initializes it. It then defines a SysTick timer
 * to sample data from the ADC and send it to the moving average filter. Once
 * the up-to-date moving average is returned, it is sent to outputLight()
 * so that the LEDs light up appropriatly. 
 */
int main()
{ 
				
	      init_sensor();	// Initialize sensors
				
				// Define Constants required for the Voltage-Celsius conversion formula
				// Defined in Reference Manual pg. 234
	      float V_Sense = 0;
        float const MAX_VOLTAGE = 3000.0;                 
        float const MAX_VALUE = 4096;
				float const V_25 = 760;           //!< V25 = VSENSE value for 25° C
        float const AVG_SLOPE = 2.5;
	
				// Declare a state and create a pointer to it
	      MaState_t s;
				MaState_t * ss;
				ss = &s;
				
				// Chose between C and Assembly implementation for initialization
				if(CHOICE) {
					// C implementation
					mov_average_init(ss);
				}else {
					// Assembly implementation
					moving_average_init(ss);
				}
				
				// reset ticks
				ticks = 0;
				
				// Configure trigger for 20Hz period
				SysTick_Config(50 * SystemCoreClock / 1000);
				
				//Loop forever
				while(1){
				// Wait for an interrupt
				if(GPIOA->IDR & 0x0001){ //if button is pressed
					toggle_ledmode();
					Delay(10000000L);
				}
				outputLight((int)mov_avg); //Set the correct LEDs based on mode and moving average value
				while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET); // Reset ADC
				while(ticks){	//When Systick fires
					 ticks = 0;	//reset ticks
					 ADC_SoftwareStartConv(ADC1); 	// Load value from ADC
					//Convert into celsius
					 V_Sense = (float)ADC_GetConversionValue(ADC1) * MAX_VOLTAGE / MAX_VALUE;
					 temperature = ((V_Sense - V_25) / AVG_SLOPE) + 25;
					// Send to moving average filter
					 mov_avg = ma_filter(temperature, ss);
				}
			}
				return 0;
}

/*!
 Interrupt handler for system tick
 This should happen with frequency of 20Hz
 */
void SysTick_Handler(){
ticks = 1;
}

