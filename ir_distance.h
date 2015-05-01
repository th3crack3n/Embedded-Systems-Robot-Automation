#include <avr/io.h>
#include <stdio.h>

void ADC_init();

unsigned ADC_read();

int get_distance(int value);

int pos_ave(int new, int * ave);