/*
 * ir_distance.c
 *
 * Created: 3/31/2012 5:14:38 PM
 *  Author: Raptor Attackz
 */ 

#include <avr/io.h>
#include <stdio.h>
#include "lcd.h"
#include "util.h"
#include "ir_distance.h"

/// Initializes ADC.
void ADC_init()
{
     ADMUX = 0b11000010;
	 ADCSRA = 0b10000111;
	 
}

/// reads the ADC input
unsigned ADC_read()
{
     ADCSRA |= _BV(ADSC);
     while(ADCSRA & _BV(ADSC));
     return ADCW;
}

/**
 * Returns the distance detected.
 * @param value - the ADC input
 **/
int get_distance(int value)
{
	int dist = 0;
	
	// better efficiency in if-else statement
	if(value > 0x390) dist = 9001;
	else if(value >= 0x2C0) dist = 15;
	else if(value >= 0x21C) dist = 20;
	else if(value >= 0x1B8) dist = 25;
	else if(value >= 0x175) dist = 30;
	else if(value >= 0x142) dist = 35;
	else if(value >= 0x118) dist = 40;
	else if(value >= 0x108) dist = 45;
	else if(value >= 0x0E1) dist = 50;
	else if(value >= 0x0CF) dist = 55;
	else if(value >= 0x0BF) dist = 60;
	else if(value >= 0x0B0) dist = 65;
	else if(value >= 0x0A7) dist = 70;
	else if(value >= 0x0A0) dist = 75;
	else if(value >= 0x094) dist = 80;
	else if(value >= 0x08D) dist = 85;
	else if(value >= 0x083) dist = 90;
	else if(value >= 0x07C) dist = 95;
	else if(value >= 0x074) dist = 100;
	else if(value < 0x074) dist = 9001;
	
	return dist;	 
}


///  Helper function to calculate the actual distance.
int pos_ave(int new, int * ave){
	int i = 0;
	for(i = 0; i < 9; i++){
		ave[i] = ave[i+1];
	}
	ave[9] = new;
	return (ave[0]+ave[1]+ave[2]+ave[3]+ave[4]+ave[5]+ave[6]+ave[7]+ave[8]+ave[9])/10; 
}