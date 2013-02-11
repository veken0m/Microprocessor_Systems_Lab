/*!
 @file moving_average2.c
 Computes an unweighted moving average with real-time data delivered by a sensor
*/

/********************************************//**
 *  Includes
 ***********************************************/

#include <stdio.h>
#include "moving_average.h"

const float d = 1.0/(float)DEPTH;			//!< 1/DEPTH made into a constant for perf. gain


/********************************************//**
 *   Functions
 ***********************************************/

/**
 * Initializes the moving average state
 *
 * Initializes the memory state by setting the first moving average value to 0,
 * pointing the index to the first data point in the window, and initializing every
 * data point in the window to 0.
 * @param *state pointer to the state
 */
void mov_average_init(MaState_t *state) {
	state->ma=0;
	state->index=0; 
	int i = 0;																								
	for (; i <DEPTH;++i){
		state->window[i]=0;
	}

}

/**
 * Recalculates the Moving Average given a new data point
 *
 * Initializes the memory state by setting the first moving average value to 0,
 * pointing the index to the first data point in the window, and initializing every
 * data point in the window to 0.
 * @param *state pointer to the state
 * @param	newData the latest and new sampled data
 * @return the new moving average value
 */
float mov_average(MaState_t *state, float newData) {

	int k = state->index;
	state->ma =state->ma + d*(newData - state->window[k]);  				// formula for moving verage using constant 1/depth
	state->window[state->index] = newData;											   // replace the value of the oldest data point with the newest
	if(state->index == (DEPTH-1))	{																// if we reached the end of the array we wrap around
		 state->index = 0;																				 // point to the start of the array
	} else {																					  				// else point to the next position in the array
			state->index +=1;
	}
	return  state->ma;																					// retun the newly set ma which will be the old ma enxt time...

}
