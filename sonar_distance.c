/*
 * sonar_distance.c
 *
 * Created: 3/24/2012 5:13:54 PM
 *  Author: Raptor Attackz
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "util.h"
#include "lcd.h"

/**
 * start Start time of when the sonar returns during the rising edge
 * end End time of when the sonar stops receiving signals during the falling edge
 * done2 Lets user know when the sonar finishes receiving sonar signal
 */
volatile int start;
volatile int end;
volatile int done2 = 0;

/// Interrupt for timer 1.
/**
 * 
 */
ISR (TIMER1_CAPT_vect)
{
	if(TCCR1B & 0x40)//looks for rising edge
	{
		start = ICR1;//sets value of start
		TCCR1B &= 0xBF;
	}
	else//else falling edge
	{
		end = ICR1;//sets value of end
		TCCR1B |= 0x40;
		done2 = 1;//ends while loop in program
	}
}

/// Uses sonar device to determine how far the closest object is away.
/**
 * 
 * @return Distance received by ping sensor.
 */
int get_sonar_distance()
{
	int diff; // time between start and end in timer ticks
	double dms; //double number of ms
	int ms; //int number of ms
	int cm; // distance found from dms
	
	//sends a 1 ms pulse to tell the sonar to start
	DDRD |= _BV(4);
    PORTD |= _BV(4); //sends a 1 ms pulse
	wait_ms(1); // waits for the sonar to send a pulse
	PORTD &= ~_BV(4); // turns off pulse
	DDRD &= ~_BV(4);
	
	done2 = 0;
	while(done2 != 1); // waits for the ISR to detect the falling edge from the ping sensor
	
	diff = end - start; // finds the difference between start and end
	dms = diff /2000.0; // converts from timer ticks to ms
	ms = diff /2000; // converts from timer ticks to printable ms
	dms = dms * 17.0;// converts from ms to cm
	cm = (int)dms;
	
	if(cm <= 0)
	{
		cm = 9001; // if the distance is too far, return a distance of 9001
	}
	return cm;
}


/// Initializes the ping sensor.
/**
 *
 */
void sonar_init()
{
	TCCR1A = 0x00; // initializes to CTC mode(parts in both registers)
	TCCR1B |= 0x8A; // initializes the prescalar to 8
	OCR1A = 65535;
	TIMSK = 0x20; // enables the input capture interrupt
	sei(); // enables Interrupt flags
}