/*!
 @init.h
 Initializes the board's peripherals by starting up and setting up LEDs, the 
 Temperature sensor and the User Button.
*/

// Multiple-include guard
#ifndef __init_H_
#define __init_H_

/********************************************//**
 *  Prototypes
 ***********************************************/
 
/**
 * Initializes LEDs, Temperature Sensor and User Button
 *
 * Starts by enabling power to the peripherals, initializing the LEDs and then the
 * button and temperature sensor.
 *
 */
void init_sensor();

#endif
