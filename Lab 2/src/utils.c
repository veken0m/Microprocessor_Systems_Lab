/*!
 @file utils.c
 A Library of random utilities that are widely used. This file will grow with
 subsequent labs and the final project.
*/

/********************************************//**
 *  Includes
 ***********************************************/

#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"
#include "utils.h"


/********************************************//**
 *   Functions
 ***********************************************/

/**
 * Equivalent to a Busy Sleep
 *
 * Forces the processor to sleep or wait for a variable amount of time defined by 
 * nCount. (Busy Wait)
 * @param nCount number of clock cycles processor is sleeping for
 */
void Delay(__IO uint32_t nCount){
	while(nCount--){
	}
}

