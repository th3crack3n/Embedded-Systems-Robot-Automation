/*
 * scanner.c
 *
 * Created: 3/31/2012 5:14:38 PM
 *  Author: Raptor Attackz
 */ 

#include <stdio.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <string.h>
#include <math.h>
#include "servo.h"
#include "bluetooth.h"
#include "ir_distance.h"
#include "sonar_distance.h"

/// Structure to help calculate the gap between two objects.
/**
 * dist1 Start of Gap in cm
 * dist2 End of Gap in cm
 * angle1 Starting Angle in degrees
 * angle2 Ending Angle in degrees
 * angled_width Width between angles
 * width Actual Width due to the robot between 2 objects
 */
struct gap
{
	int dist1;
	int dist2;
	int angle1;
	int angle2;
	int angled_width;
	int width;
};

/// Structure to help keep track of the objects detected.
/**
 * start Start of Object in cm
 * end End of Object in cm
 * dist_ave Average distance between 2 objects
 * width Width of the object based distance and angles
 */
struct object
{
	int start;
	int end;
	int dist_ave;
	int width;
};

/// Initialize Servo, ADC, and Sonar
void scanner_init()
{
	servo_init();
	ADC_init();
	sonar_init();
}	

/// Scans the field and sends data back to PuTTY
/**
 * Uses IR and Sonar to scan the area the robot is looking at, printing what is sees to putty.
 */
void scan()
{	
	int i, j;
	int ave[10];
	/// Reading from ADC
	unsigned ADC_reading;
	/// Output stream
	char *output;
	/// IR Distance and Sonar Distance
	int ir_dist = 0;
	int sonar_dist = 0;
	
	int object_found = 0; /// 1 if new object is found
	int state = 0; /// 1 if a gap has finished or a new object has finished
	
	struct object curr = {9001, 0, 0, 0};
	struct gap space = {0, 0, 0, 0, 0, 0};
		
	send_string("Scan!");
	
	output = (char *) malloc(85);
	sprintf(output, "Degrees      IR Distance (cm)    Sonar Distance (cm)");
	send_string(output);
	
	for( i = 0; i <= 90; i++)
	{
		/// Takes 10 ADC readings and averages them
		for(j = 0; j < 10; j++)
		{
			ADC_reading = ADC_read();
			ir_dist = get_distance(pos_ave(ADC_reading, ave));
		}
		/// Gets the Sonar Distance
		sonar_dist = get_sonar_distance();
		/**
		 * If a new object is located initialize the new object and calculate
		 * the gap information from the previous object to current object
		 */
		if(ir_dist != 9001 && object_found == 0)
		{
			object_found = 1;
			curr.dist_ave = 0;
			curr.start = i*2;
			if(space.dist1 == 0)
			{
				space.dist1 = space.dist2;
			}
			space.angle2 = curr.start;
			space.dist2 = ir_dist;
			space.angled_width = sqrt((double)pow((double)space.dist1,2.0)+pow((double)space.dist2,2.0)-2.0*(space.dist1*space.dist2)*cos((double)(space.angle2-space.angle1)*3.141593/180));
			if(space.dist2 > space.dist1)
			{
				space.width = (int)((double)(space.dist1) * tan((double)(space.angle2-space.angle1)*3.141593/180));
			}
			else
			{
				space.width = (int)((double)(space.dist2) * tan((double)(space.angle2-space.angle1)*3.141593/180));
			}
			sprintf(output, "%3d          %4d                %3d   Gap Width = %d, Actual Width = %d", (i*2), ir_dist, sonar_dist, space.width, space.angled_width);
			send_string(output);
			state = 1;
			
		}
		/**
		 * If the object has finished, then we calculate object information and initialize
		 * a new gap structure
		 */
		else if((ir_dist == 9001 || i == 90) && object_found != 0)
		{
			object_found = 0;
			if(i == 90)
			{
				curr.end = 90;
            }
			else
            {
				curr.end = (i*2)-2;
            }
            curr.dist_ave = (int)((double)(curr.dist_ave)/((double)(curr.end-curr.start)/2.0)); 
			curr.width = (int)((double)(curr.dist_ave) * tan((double)(curr.end-curr.start)*3.141593/180));
			space.angle1 = curr.end;
			space.dist1 = curr.dist_ave;
			sprintf(output, "%3d          %4d                %3d   Object Width = %d", (i*2), ir_dist, sonar_dist, curr.width);
			send_string(output);
			state = 1;
		}
		
		/// Adds sonar distance if IR Sensor does not detect an object otherwise add IR Distance
		if(object_found != 0){
			if(sonar_dist < ir_dist + 10 && sonar_dist > ir_dist - 10)
			{
				curr.dist_ave += sonar_dist;	
			}
			else
			{
				curr.dist_ave += ir_dist;
			}							
		}
		
		if(state == 0)
		{
			sprintf(output, "%3d          %4d                %3d", (i*2), ir_dist, sonar_dist);
			send_string(output);
		}
		else
		{
			state = 0;
		}
		/// increment the servo 2 degrees from 0 to 180
		set_servo(i*2);
	}
	sprintf(output, "Done!\n");
	send_string(output);
	set_servo(90);	
	
	free(output);
}
