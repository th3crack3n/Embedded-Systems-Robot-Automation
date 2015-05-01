/*
 * bluetooth.c
 *
 * Created: 3/6/2012 5:15:38 PM
 *  Author: Raptor Attackz
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <ctype.h>
#include "lcd.h"
#include "util.h"
#include "bluetooth.h"

/**
 * received The character received from PuTTY
 * new becomes 1 if there is a new character being received from PuTTY
 * input_arr An array of characters used to store the command being input
 * pos The position in input_arr we are currently writing to
 */
volatile char received;
volatile int new = 0;
volatile char input_arr[10];
volatile int pos = 0;

///ISR for receive complete
/**
 * 
 */
ISR (USART0_RX_vect){
	received = UDR0;
	if (received == '\r'){
		new = 1;
		pos = 0;
	}
	else{
		input_arr[pos] = received;
		pos++; 		
	}	
	serial_putc();
}

///sends characters to the data buffer if it is clear and sends \n when \r is pressed
/**
 * 
 */
void serial_putc() {
	/// Wait for empty transmit buffer by checking the UDRE bit 
	while ((UCSR0A & 0b00100000) == 0);
	/// Put data into transmit buffer; sends the data 
	UDR0 = received;
	if(received == '\r'){
		while ((UCSR0A & 0b00100000) == 0);
		UDR0 = '\n';
		while ((UCSR0A & 0b00100000) == 0);
		UDR0 = '\r';
	}	
}

/// Clears the string array.
/**
 * 
 */
void clear_string(){
	new = 0;
	int i = 0;
	for(i = 0; i < 10; i++){
		input_arr[i]= '\0';
	}
}

/// Returns 1 if there is a new char array ready
/**
 * 
 * @return new
 */
int is_new()
{
	return new;
}

/// Returns the new char array.
/**
 * @return input_arr
 */
char * get_char_array()
{
	return input_arr;
}

/// Sends a string of characters to Putty's screen via bluetooth
/**
 * 
 * @param input (String of charicters to be sent)
 */
void send_string(char * input)
{
	int i = 0;
	while(input[i] != '\0')
	{
		while ((UCSR0A & 0b00100000) == 0);
		UDR0 = input[i];
		i++;
	}
	while ((UCSR0A & 0b00100000) == 0);
	UDR0 = '\n';
	while ((UCSR0A & 0b00100000) == 0);
	UDR0 = '\r';
}

///Initializes USART.
/**
 * 
 * @param baud (baud rate of the serial conection)
 */
void USART_Init(unsigned long baud)
{
	wait_ms(1000);
	baud = ((16000000 / 8 / baud) - 1);
	/// baud = (F_CPU / 16 / baud) ? 1; // if not using double speed mode
	/// Set baud rate
	UBRR0H = (unsigned char) (baud >> 8);
	UBRR0L = (unsigned char)baud;
	/// Enable double speed mode 
	UCSR0A = 0b00000010;
	/// Set frame format: 8 data bits, 2 stop bits
	UCSR0C = 0b00001110;
	/// Enable receiver and transmitter 
	UCSR0B = 0b10011000;
	sei();
}