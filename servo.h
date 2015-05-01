/*
 * servo.h
 *
 * Created: 3/6/2012 4:35:09 PM
 *  Author: aupah
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#include <math.h>

void servo_init();

void set_servo(double pos);