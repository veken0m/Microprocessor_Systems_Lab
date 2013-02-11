/*!
 @file moving_average.h
 Computes an unweighted moving average with real-time data delivered by a sensor
*/

// Multiple-include guard
#ifndef __moving_average_H_
#define __moving_average_H_

/********************************************//**
 *  Defines 
 ***********************************************/

 #define DEPTH (7)	//!< Depth for moving average window

/********************************************//**
 *   Variables & Constants
 ***********************************************/

/*! \brief Typedef representing the state of the moving average filter
 *  This typedef represents the memory state used by a moving average filter. It
 *  holds the value for the latest computed moving average value, a window of the
 * 	DEPTH last data points sampled and an index that points to the oldest data
 *	point.
 */
typedef struct {

	float ma;										//!< Latest computed moving average value
	float window[DEPTH];				//!< Window of last DEPTH data samples collected
	int index;									//!< Index pointing to the oldest data sample point

} MaState_t;

/********************************************//**
 *   Prototypes
 ***********************************************/

/**
 * Initializes the moving average state
 *
 * Initializes the memory state by setting the first moving average value to 0,
 * pointing the index to the first data point in the window, and initializing every
 * data point in the window to 0.
 * @param *state pointer to the state
 */
void mov_average_init(MaState_t *state);

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
float mov_average(MaState_t *state, float newData);

#endif
