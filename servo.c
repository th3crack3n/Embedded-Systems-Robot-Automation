/*
 * servo.c
 *
 * Created: 3/6/2012 4:34:10 PM
 *  Author: Raptor Attackz
 */ 
#include <avr/io.h>
#include <avr/interrupt.h>
#include <math.h>
#include "util.h"
#include "servo.h"

int current_pos;

/// Initializes the servo.
/**
 * 
 */
void servo_init()
{
	TCCR3A = 0b10101011;
	TCCR3B = 0b11011010;
	OCR3A = 40000;
	OCR3B = 2500; // 90 degree position
	current_pos = 90;
	DDRE |= 0x10;
}

/// resets servo to original position.
/**
 *
 * @param pos a double between 0.0 and 180.0 degrees that indicates what position to turn the servo
 */
void set_servo(double pos)//(pos = position in degrees)
{
	if(pos > 180)
	{
		pos = 180;
	}
	if(pos < 0)
	{
		pos = 0;
	}

	int set = 0;
	int posi = pos;
	pos = pos / 180.;
	pos = pos * 3400.;
	set = (int)(pos + 800);
	OCR3B = set;
	wait_ms(abs((current_pos-posi)*2));
}