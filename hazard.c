/*
 * hazard.c
 *
 * Created: 3/27/20XD6 E4:51:44 PM
 *  Author: Raptor Attackz
 */ 

#include <stdio.h>
#include <stdlib.h>
#include <avr/io.h>
#include "open_interface.h"
#include "bluetooth.h"

int old_cl = 0;
int old_cfl = 0;
int old_cfr = 0;
int old_cr = 0;
int old_bump = 0;
int old_wall = 0;


/**
 * Checks each of the cliff sensors and returns 1 if the sensor is tripped and prints to putty which sensor was tripped.
 * @param sensor_status - the robot 
 **/
int check_cliff(oi_t *sensor_status)
{
	
	int stop = 0;
		
    oi_update(sensor_status);

	if(old_cl != sensor_status->cliff_left)
	{
		old_cl = sensor_status->cliff_left;
		if(old_cl == 1)
		{
			oi_set_wheels(0, 0);
			stop = 1;
			send_string("Cliff Left Detected!");
		}
	}
	else if(old_cfl != sensor_status->cliff_frontleft)
	{
		old_cfl = sensor_status->cliff_frontleft;
		if(old_cfl == 1)
		{
			oi_set_wheels(0, 0);
			stop = 1;
			send_string("Cliff Front Left Detected!");
		}
	}
	else if(old_cfr != sensor_status->cliff_frontright)
	{
		old_cfr = sensor_status->cliff_frontright;
		if(old_cfr == 1)
		{
			oi_set_wheels(0, 0);
			stop = 1;
			send_string("Cliff Front Right Detected!");
		}
	}
	else if(old_cr != sensor_status->cliff_right)
	{
		old_cr = sensor_status->cliff_right;
		if(old_cr == 1)
		{
			oi_set_wheels(0, 0);
			stop = 1;
			send_string("Cliff Right Detected!");
		}
	}
	return stop;
}

/**
 * Virtual wall sensor function to detect the edge and prints to the putty that the wall was detected.
 * @param sensor_status - the robot
 **/
int check_virtual_wall(oi_t *sensor_status)
{
	oi_update(sensor_status);
	int status = sensor_status->virtual_wall;
	if (status == 1 && old_wall == 0)
	{
		oi_set_wheels(0,0);
		send_string("Virtual Wall Detected!");
	}
	old_wall = status;
	
	return status;
}


/**
 * Bump sensor function to detect if an object is bumped and prints to the putty which bumper is tripped.
 * @param sensor_status - the robot
 **/
int check_bump_sensor(oi_t *sensor_status)
{
	int bump = oi_bump_status(sensor_status);
	if (bump != 0 && old_bump != bump)
	{
		oi_set_wheels(0,0);
		switch(bump)
		{
			case 1:
				send_string("Right Bump Sensor Detected!");
				break;
			case 2:
				send_string("Left Bump Sensor Detected!");
				break;
			case 3:
				send_string("Both Bump Sensors Detected!");
		}
	}
	old_bump = bump;
	return bump;
}