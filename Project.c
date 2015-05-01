/*
 * Project.c
 *
 * Created: 4/3/2012 4:48:14 PM
 *  Author: Raptor Attackz
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <ctype.h>
#include <math.h>
#include "bluetooth.h"
#include "hazard.h"
#include "ir_distance.h"
#include "lcd.h"
#include "music.h"
#include "open_interface.h"
#include "scanner.h"
#include "util.h"
#include "servo.h"
#include "sonar_distance.h"
#include "util.h"

/// BAUD Rate to connect to Bluetooth
#define BAUD 57600 

/**
 * isAtEnd Determines whether we are at the end of the course or not
 * backup Records how much the robot must back up if it finds a hazard
 */ 
int isAtEnd;
int backup = 0;

int getNumber(char * input);
int checkSensors(oi_t * self);
void movement(volatile char *, oi_t *);

int main(void)
{
    oi_t *robot = oi_alloc();
	int old_h = 0;
	int new_h = 0;
	char* output;
	char * input;
	output = (char *) malloc(100);
	input = (char *) malloc(10);
	isAtEnd = 0; /// variable will change to 1 when we have reached the end of the course
	
	/// Initializes everything we need
	oi_init(robot);
	USART_Init(BAUD);
	lcd_init();
	scanner_init();
	init_push_buttons();
	
	lprintf("Ready...");
	send_string("Ready...");
	
	wait_ms(1000);
	sei();

	lprintf("Start");
	send_string("Start");  /// prints to the putty that our robot is ready to go.
	
	while(!isAtEnd){
		/// Checks for any new hazards
		new_h = checkSensors(robot);
		if (new_h == 1 && old_h == 0)
		{
			send_string("Hazard Detected!");
		}
		old_h = new_h;
		
		/// Checks for any new commands from PuTTY
		if (is_new() == 1)
		{
			input = get_char_array();
			movement(input, robot);
			clear_string();
		}
	}
	send_string("Finished!");
	win();
	free(output);
	free(input);
}



/** Will receive a string from the putty determining what we want our robot to do.
 * m will be move a given distance
 * s will be STOP
 * l will be to scan the area
 * r will be turn a given degree
 * g will display sensor data
 * w will indicate final position found
 *
 *@param input - input string, parsed to find desired function
 *@param self - robot being used
 **/
void movement(volatile char * input, oi_t * self)
{
	int i = 0;
	int val = 0;
	char* output1 = (char *) malloc(50);
	int ir_status, cliff_status;
	char * temp = (char *) malloc(5);
	
	sprintf(output1, "Command: %s", input);
	send_string(output1);
	switch(input[0])
	{
		case('m'): /// moves the robot a given length
			i++;
			
			val = getNumber(input);
			oi_clear_distance(self);
			sprintf(output1, "val: %d", val);
			send_string(output1);
			int dist = oi_current_distance(self);
			if(val > 0) /// when our value is positive we move forward
			{
				oi_set_wheels(250, 250);
				while(dist < (val * 10)) /// we want our distance to be in cm
				{
					if(checkSensors(self))
					{
						lprintf("Distance Traveled: %d cm", backup);
						sprintf(output1, "Distance Traveled: %d cm", backup);
						send_string(output1);
						break;
					}
					dist = oi_current_distance(self);
				}
				oi_set_wheels(0, 0);
			}
			else if(val < 0) /// when our value is negative we move backward
			{
				oi_set_wheels(-250, -250);
				while(dist >= (val * 10) || dist == 0)
				{
					if(checkSensors(self))
					{
						lprintf("Distance Traveled: %d cm", oi_current_distance(self) / 10);
						sprintf(output1, "Distance Traveled: %d", oi_current_distance(self));
						send_string(output1);
						break;
					}
					dist = oi_current_distance(self);
				}
				oi_set_wheels(0, 0);
			}				
				
		break;
		
		case('s'): /// stops the robot.
			oi_set_wheels(0, 0);
			sprintf(output1, "Current distance = %d mm Current Angle = %d deg", oi_current_distance(self), oi_current_angle(self));
			send_string(output1);
		break;
		
		case('l'): /// uses the scan function to scan the area and print to putty.
			if(input[1] == '\0')
			{
				oi_set_wheels(0,0);
				scan();
			}
		break;
		
		case('r'): /// moves the robot a given degree
			i++;
			val = getNumber(input);
			oi_clear_angle(self);
			int turn = oi_current_angle(self);
			lprintf("val = %d", val);
			if(val > 0)  /// rotates right given degrees when the value is positive.
			{
				oi_set_wheels(-250, 250);
				while(turn >= (360 - val) || turn == 0) 
				{
					if(checkSensors(self))
					{
						sprintf(output1, "Angle turned: %d", oi_current_angle(self));
						send_string(output1);
						break;
					}
					turn = oi_current_angle(self);
				}				
				oi_set_wheels(0, 0);
			}	
			else ///rotates left given degrees when the value is negative.
			{
				oi_set_wheels(250, -250);
				while(turn <= (val * -1))
				{
					if(checkSensors(self))
					{
						sprintf(output1, "Angle turned: %d", oi_current_angle(self));
						send_string(output1);
						break;
					}
					turn = oi_current_angle(self);
				}				
				oi_set_wheels(0, 0);
			}			
		break;
			
		case('g'): /// used to display the sensor data.
			if(input[1] == '\0')
			{
				checkSensors(self);
			}
			break;

		case('w'): /// Will be used when we are in the end zone and will exit the while loop.
			if(input[1] == '\0')
			{
				isAtEnd = 1;
			}
		break;
			
		default:
			send_string("Not a valid command");
		break;
	
		}			
		
		free(output1);
			
}

/**
 * Helper function with movement.  Will return the integer val of either the distance or an angle.
 * @param string the string received by putty converts the string to an integer value to be used
 * @return the number extracted from the input string
 **/
int getNumber(char* string){
	int i = 1;
	int num = 0;
	takes short string command
	char * output9 = (char *) malloc(50);
	
	int place = 1;
	//incriments i till end of string
	while(string[i] != '\0'){
		sprintf(output9, "string[%d] = %c", i , string[i]);
		send_string(output9);
		i++;
	}
	//brings i to last charicter of string
	i--;
	//adds nummbers to num in accordance to their place
	while(isdigit(string[i])){
		num = num + place*((int)(string[i])-48);
		i--;
		place = place * 10;
	}
	//multiplies num by -1 if marked as negetive
	if(string[i] == '-'){
		num = num * -1;
	}
	free(output9);
	return num;
}	

/**
 * Checks the cliff sensors, bump sensors and the Virtual Wall sensors to detect a hazard.
 * @param self the robot we are using
 * @return 1 if there is a hazard otherwise 0
 **/
int checkSensors(oi_t * self){
	int hazard = 0;
	int cliff_haz = check_cliff(self);
	int virt_wall_haz = check_virtual_wall(self);
	int bump_haz = check_bump_sensor(self);
	if (cliff_haz == 1 || virt_wall_haz == 1 || bump_haz != 0) /// if we detect some hazard we will back up and go where we started movement.
	{
		hazard = 1;
		backup = oi_current_distance(self);
		oi_clear_distance(self);
		int dist = 0;
		oi_set_wheels(-250, -250);
		while(dist >= (backup * -1))
		{
			dist = oi_current_distance(self);
		}
		oi_set_wheels(0, 0);
	}
	return hazard;
}