/*
 * open_interface.c
 *
 * Created: 3/6/2012 5:15:38 PM
 *  Author: Raptor Attackz
 */ 

#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "open_interface.h"

volatile uint16_t led0_state = 0;

// Allocated a new oi
oi_t *oi_alloc(void) {
	oi_t *self;
	self = calloc(1, sizeof(oi_t));

	self->dist_m = 0;
	self->dist_mm = 0;
	self->angle = 0;
	return self;
}



// Destroy a oi
void oi_free(oi_t *self) {
	free(self);
}



// Initialize the Create
void oi_init(oi_t *self) {
	// Setup the Cerebot II - iRobot Create Interface Serial Port
	{
		// Set serial baud rate to 57600
		// int BAUD=57600;
		// int MYUBRR=(FOSC/16/BAUD-1);
		int MYUBRR=16;
		UBRR1H = (unsigned char) (MYUBRR>>8);
		UBRR1L = (unsigned char) MYUBRR;

		// Enable serial transmit and recieve
		UCSR1B = 0x18;
		// Set to 8-bit data
		UCSR1C = 0x06;
	}

	// Turn on the create
	oi_power_on();

	// Wait 2.5 seconds. Allow for the bootloader to startup
	wait_ms(2500);

	// Starts the SCI. Must be sent first
	oi_byte_tx(OI_OPCODE_START);

	wait_ms(500);

	// Set the baud rate on the Create
	{
		// baud opcode
		oi_byte_tx(OI_OPCODE_BAUD);

		// Set the transmit flag
		UCSR1A |= 0x40;

		// baud code for 28800
		oi_byte_tx(8); 

		// Wait until the flag is cleared
		while(!(UCSR1A & 0x40));

		// Wait 100ms before sending more commands at the new baud rate.
		wait_ms(100);
	}

	// Set the baud rate on the Cerebot II to match the Create's baud
	{
		// Disables interrupts
		cli();

		// Set new baud rate for the mind control
		// int BAUD=28800;
		// int MYUBRR=(FOSC/16/BAUD-1);
		int MYUBRR=33;
		UBRR1H = (unsigned char)(MYUBRR>>8);
		UBRR1L = (unsigned char)MYUBRR;

		// Enable interrupts
		sei();

		wait_ms(100);
	}


	// Use Full mode, unrestricted control
	oi_byte_tx(OI_OPCODE_FULL);
	wait_ms(500);
}



// Update the Create. This will update all the sensor data.
void oi_update(oi_t *self) {
	uint8_t i;
	unsigned char buf[26];
	uint8_t byte;
	int16_t dist, angle;

	// clear the receive buffer
	while(UCSR1A & 0x80) 
		i = UDR1;

	// Query a list of sensor values
	oi_byte_tx(OI_OPCODE_SENSORS);

	// Send the sensor packet ID
	oi_byte_tx(OI_SENSOR_PACKET_GROUP0); 

	// Read all the sensor data
	for (i = 0; i < 26; i++) {
		// read each sensor byte
		buf[i] = oi_byte_rx();
	}

	i=0;

	byte = buf[i++];
	self->bumper_right = (byte >> 0) & 0x01;
	self->bumper_left = (byte >> 1) & 0x01;
	self->wheeldrop_right = (byte >> 2) & 0x01;
	self->wheeldrop_left = (byte >> 3) & 0x01;
	self->wheeldrop_caster = (byte >> 4) & 0x01;

	self->wall = buf[i++] & 0x01;
	self->cliff_left = buf[i++] & 0x01;
	self->cliff_frontleft = buf[i++] & 0x01;
	self->cliff_frontright = buf[i++] & 0x01;
	self->cliff_right = buf[i++] & 0x01; 
	self->virtual_wall = buf[i++] & 0x01;

	byte = buf[i++]; 
	self->overcurrent_ld1 = (byte >> 0) & 0x01;
	self->overcurrent_ld0 = (byte >> 1) & 0x01;
	self->overcurrent_ld2 = (byte >> 2) & 0x01;
	self->overcurrent_driveright = (byte >> 3) & 0x01;
	self->overcurrent_driveleft = (byte >> 4) & 0x01;

	// There are two unused bytes here
	i=i+2;

	self->remote_opcode = buf[i++];

	byte = buf[i++];
	self->button_play = (byte >> 0) & 0x01;
	self->button_advance = (byte >> 2) & 0x01;

	// Read the distance in millimeters since last update
	dist = buf[i] << 8 | buf[i+1];
	i = i + 2;

	self->dist_mm += dist;
	if (self->dist_mm >= 1000) {
		self->dist_m = self->dist_m + 1;
		self->dist_mm -= 1000;
	} else if (self->dist_mm <= -1000) {
		self->dist_m--;
		self->dist_mm += 1000;
	}

	// Angle in degrees since last update
	angle = buf[i] << 8 | buf[i+1];
	i += 2;

	self->angle += angle;
	if (self->angle >= 360) {
		self->angle -= 360;
	} else if (self->angle < 0) {
		self->angle += 360;
	}

	/// Charge state: 0=not charging, 1=reconditioning charging, 2=full charging,
	/// 3=trickle charging, 4=waiting, 5=charging fault condition
	self->charging_state = buf[i++];

	// Voltage in millivolts
	self->voltage = buf[i] << 8 | buf[i+1];
	i += 2;

	// Current in milliamps
	self->current = buf[i] << 8 | buf[i+1];
	i += 2;

	// Battery temperature
	self->temperature = buf[i++];

	// Battery charge in milliamp-hours
	self->charge = buf[i] << 8 | buf[i+1];
	i += 2;

	// Battery capacity in milliamp-hours
	self->capacity = buf[i] << 8 | buf[i+1];
	i += 2;
}



// Checks if the Create is On or Off.
uint8_t oi_is_on() {
	// Save current registers
	uint8_t ddrb_save = DDRB;
	uint8_t portb_save = PORTB;
	uint8_t result = 0;

	// make an input, skip if done earlier
	DDRB &= ~0x20;

	// disable pull-up, skip if done earlier
	PORTB &= ~0x20;

	if(PINB & 0x20) {
		// Power is on
		result = 1;
	} else {
		// Power is off
		result = 0;
	}

	// Restore registers
	DDRB = ddrb_save;
	PORTB = portb_save;

	return result;
}



// Power on the Create.
void oi_power_on() {
	if (!oi_is_on()) {
		// set D7 low
		PORTD &= ~PIN_7;

		// Delay so new state is seen by the iRobot oi
		wait_ms(100);

		// set D7 high to turn power on
		PORTD |= PIN_7;

		// Delay so new state is seen by the iRobot oi
		wait_ms(100);
	}
}



// Power off the Create
void oi_power_off() {
	if (oi_is_on()) {  
		/* set D7 low */
		PORTD &= ~PIN_3;

		/* Delay so new state is seen by the iRobot oi */
		wait_ms(100);

		/* set D7 high to turn power off */
		PORTD |= PIN_3;

		/* Delay so new state is seen by the iRobot oi */
		wait_ms(100);
	}
}



// Set the LEDS on the Create
void oi_set_leds(uint8_t play_led, uint8_t advance_led, uint8_t power_color, uint8_t power_intensity) {
	// LED Opcode
	oi_byte_tx(OI_OPCODE_LEDS);

	// Set the Play and Advance LEDs
	oi_byte_tx(advance_led << 3 && play_led << 2);

	// Set the power led color
	oi_byte_tx(power_color);

	// Set the power led intensity
	oi_byte_tx(power_intensity);
}



// Set the speed of the robot
void oi_set_speed(int16_t linear_speed, int16_t angular_speed) {
	int16_t rightVel = linear_speed + (angular_speed/1e3) * OI_HALF_AXLE_LENGTH;
	int16_t leftVel = linear_speed - (angular_speed/1e3) * OI_HALF_AXLE_LENGTH;

	oi_byte_tx(OI_OPCODE_DRIVE_WHEELS);
	oi_byte_tx(rightVel>>8);
	oi_byte_tx(rightVel & 0xff);
	oi_byte_tx(leftVel>>8);
	oi_byte_tx(leftVel & 0xff);
}



// MFT: Drive wheels directly
void oi_set_wheels(int16_t right_wheel, int16_t left_wheel) {
	oi_byte_tx(OI_OPCODE_DRIVE_WHEELS);
	oi_byte_tx(right_wheel>>8);
	oi_byte_tx(right_wheel & 0xff);
	oi_byte_tx(left_wheel>>8);
	oi_byte_tx(left_wheel& 0xff);
}



//MFT: Load a song
void oi_load_song(unsigned char song_index, unsigned char num_notes, unsigned char *notes, unsigned char *duration) {
	unsigned char i;
	oi_byte_tx(OI_OPCODE_SONG);
	oi_byte_tx(song_index);
	oi_byte_tx(num_notes);
	for (i=0;i<num_notes;i++) {
		oi_byte_tx(notes[i]);
		oi_byte_tx(duration[i]);
	}
}



//MFT: Play a song
void oi_play_song(unsigned char index){
	oi_byte_tx(OI_OPCODE_PLAY);
	oi_byte_tx(index);


}



void go_charge(void) {
	char charging_state=0;
	//Calling demo that will cause Create to seek out home base
	oi_byte_tx(OI_OPCODE_MAX);
	oi_byte_tx(0x01);
	//Control is returned immediately, so need to check for docking status
	DDRB &= ~0x80; //Setting pin7 to input
	PORTB |= 0x80; //Setting pullup on pin7
	do {
		charging_state = PINB >> 7;
	} while(charging_state == 0);
	lcd_clear();
	lcd_home_line1();
	lcd_puts("On Charger");

	//oi_byte_tx(0x07); //Undocumented soft reset opcode necessary to ensure Create is charging
}



// Transmit a byte of data over the serial connection to the Create
void oi_byte_tx(unsigned char value) {
	// Wait until the transmit buffer is empty
	while (!(UCSR1A & 0x20))
  		;

	UDR1 = value;
}



// Receive a byte of data from the Create serial connection. Blocks until
// a byte is received.
unsigned char oi_byte_rx(void) {
	// wait until a byte is received
	while (!(UCSR1A & 0x80));

	return UDR1;
}



// Resets distance values stored on Create and the structure maintained
// by this API
void oi_clear_distance(oi_t *self) {
	oi_update(self);
	self->dist_m = 0;
	self->dist_mm = 0;
}



//Resets angle values stored on Create and the structure maintained
// by this API
void oi_clear_angle(oi_t *self) {
	oi_update(self);
	self->angle = 0;
}



//Requests sensor update and returns distance as mm
int oi_current_distance(oi_t *self) {
	oi_update(self);
  	return (self->dist_m * 1000) + self->dist_mm;
}



//Requests sensor update and returns angle as degrees
int oi_current_angle(oi_t *self) {
	oi_update(self);
	return(self->angle);
}



//Returns bump sensor status
// 0 = no sensors pressed
// 1 = right sensor
// 2 = left sensor
// 3 = both sensors
char oi_bump_status(oi_t *self) {
	char status=0;
	oi_update(self);
	status = self->bumper_right;
	status |= (self->bumper_left << 1);
	return(status);
	
}
