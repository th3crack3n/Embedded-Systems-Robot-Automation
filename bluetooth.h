/*
 * bluetooth.h
 *
 * Created: 3/6/2012 5:15:48 PM
 *  Author: aupah
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#include <ctype.h>

volatile int new, pos;
volatile char received;
volatile char input_arr[10];

ISR (USART0_RX_vect);

int is_new();

void set_new(int num);

char * get_char_array();

void send_string(char * input);

void serial_putc();

void USART_Init();

void clear_string();