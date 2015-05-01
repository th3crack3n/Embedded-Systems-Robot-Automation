/*
 * IncFile1.h
 *
 * Created: 3/24/2012 5:14:37 PM
 *  Author: aupah
 */ 
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>

ISR (TIMER1_CAPT_vect);

int get_sonar_distance();

void sonar_init();