/*!
 @file utils.h
 A Library of random utilities that are widely used. This file will grow with subsequent labs and the final project.
*/

// Multiple-include guard
#ifndef __utils_H_
#define __utils_H_

/********************************************//**
 *   Prototypes
 ***********************************************/

/**
 * Equivalent to a Busy Sleep
 *
 * Forces the processor to sleep or wait for a variable amount of time defined by 
 * nCount. (Busy Wait)
 * @param nCount number of clock cycles processor is sleeping for
 */
void Delay(__IO uint32_t nCount);

#endif
