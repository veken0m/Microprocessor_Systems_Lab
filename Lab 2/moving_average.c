/*!
 @file moving_average.c
 Computes an unweighted moving average with real-time data delivered by a sensor
*/

// Includes

#include <stdio.h>

// Private defines

 #define DEPTH (10)

// Private type definitions

 //Will be added to the header file later

typedef struct {

	float ma;
	float window[DEPTH];
	float *oldest;
	int windowSize;

}MaState_t;

// Public variables

// Private variables

const float d = 1/DEPTH;

// Private function prototypes

// Function bodies

MaState_t moving_average_init(MaState_t state) {
	state.ma=0;
	state.oldest=window;
	state.windowSize=0;
	for (int i = 0; i <DEPTH;++i){
		state.window[i]=0;
	}

}

float moving_average(MaState_t state, float newData) {

	if(state.windowSize < DEPTH){
		state.window[state.windowSize]=newData;					// Add the newest data to the current last position in the window
		state.windowSize++;										// increase windowSize by 1
		float sum = 0;											// reset sum
		for(int i=0; i <state.windowSize;++i){
			sum +=state.window[i];								// sum initial windowsSize amount of data points
		}

		state.ma = sum/state.windowSize;						// find the average of those points												// return the new average
	}
	else{
		state.ma+ = d*(newstData - *oldest);					// formula for moving verage using constant 1/depth
		state.*oldest = newestData								// replace the value of the oldest data point with the newest
		if(oldest = &state.window[DEPTH-1])	{					// if we reached the end of the array we wrap around
			state.oldest = window;								// point to the start of the array
		}
		else {													// else point to the next position in the array
				state.oldest = *(state.oldest+1);
		}
	}
	return ma;													// retun the newly set ma which will be the old ma enxt time...

}