/*
 * detectCliff.c
 *
 * Created: 3/27/2012 E4:51:44 PM
 *  Author: aupah
 */ 

#include <stdio.h>
#include <stdlib.h>
#include <avr/io.h>
#include "open_interface.h"

int old_cl = 0;
int old_cfl = 0;
int old_cfr = 0;
int old_cr = 0;

// Checks each of the cliff sensors and returns 1 if the sensor is triped.
/*
 *@apram sensor_status (structor that holds off os the info about the robot)
 */
int update_cliff(oi_t *sensor_status)
{
	
	int stop = 0;
		
    oi_update(sensor_status);

	if(old_cl != sensor_status->cliff_left)
	{
		old_cl = sensor_status->cliff_left;
		if(old_cl == 1)
		{
			stop = 1;
			oi_set_wheels(0, 0);
		}
	}
	else if(old_cfl != sensor_status->cliff_frontleft)
	{
		old_cfl = sensor_status->cliff_frontleft;
		if(old_cfl == 1)
		{
			stop = 1;
			oi_set_wheels(0, 0);
		}
	}
	else if(old_cfr != sensor_status->cliff_frontright)
	{
		old_cfr = sensor_status->cliff_frontright;
		if(old_cfr == 1)
		{
			stop = 1;
			oi_set_wheels(0, 0);
		}
	}
	else if(old_cr != sensor_status->cliff_right)
	{
		old_cr = sensor_status->cliff_right;
		if(old_cr == 1)
		{
			stop = 1;
			oi_set_wheels(0, 0);
		}
	}
	return stop;
}