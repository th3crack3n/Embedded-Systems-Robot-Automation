/*
 * detect_cliff.h
 *
 * Created: 4/3/2012 4:17:11 PM
 *  Author: aupah
 */ 


#ifndef DETECT_CLIFF_H_
#define DETECT_CLIFF_H_

#include <stdio.h>
#include <stdlib.h>
#include <avr/io.h>
#include "open_interface.h"

/* 
	Updates status of cliff sensor and stops wheels if cliff is detected
	Returns 1 if wheels are stopped, otherwise returns 0
*/
int update_cliff(oi_t *sensor_status);


#endif /* DETECT_CLIFF_H_ */