/*
 * music.c
 *
 * Created: 3/31/2012 9:46:39 AM
 *  Author: jmajzner
 */ 

#include <avr/io.h>
#include "open_interface.h"
#include "lcd.h"
#include "util.h"
#include <stdio.h>

void playmario();
void playfinal();
void win();

void playfinal(){
	unsigned char num_notes = 22;
	///each note beat
	unsigned char notes[22] = {48, 50, 52, 54, 56, 58, 60, 62, 64, 66, 68, 70, 72, 72, 72, 72, 68, 70, 72, 0, 70, 72};
	///length of each note
	unsigned char duration[22] = {6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 10, 10, 10, 30, 30, 30, 10, 10, 10, 45};
	unsigned char final_id = 1;
	oi_load_song(final_id, num_notes, notes, duration);
	oi_play_song(final_id);
}

void playmario() {
	
	unsigned char mario1NumNotes = 49;
	unsigned char mario1Notes[49]    = {48, 60, 45, 57, 46, 58,  0, 48, 60, 45, 57, 46, 58,  0, 41, 53, 38, 50, 39, 51,  0, 41, 53, 38, 50, 39, 51,  0, 51, 50, 49, 48, 51, 50, 44, 43, 49, 48, 54, 53, 52, 58, 57, 56, 51, 47, 46, 45, 44 };
	unsigned char mario1Duration[49] = {12, 12, 12, 12, 12, 12, 62, 12, 12, 12, 12, 12, 12, 62, 12, 12, 12, 12, 12, 12, 62, 12, 12, 12, 12, 12, 12, 48,  8,  8,  8, 24, 24, 24, 24, 24, 24,  8,  8,  8,  8,  8,  8, 16, 16, 16, 16, 16, 16 };
	oi_load_song(2, mario1NumNotes, mario1Notes, mario1Duration);
	
	oi_play_song(2);

}

win(){
	playfinal();
	for(int i=0; i<=20; i++)
	{
		oi_set_leds(1,1, 0,255);
		wait_ms(100);
		oi_set_wheels(-250,250);
		oi_set_leds(1,1, 255,255);
		wait_ms(100);
		oi_set_wheels(250,-250);
	}
	oi_set_wheels(0,0);
}



