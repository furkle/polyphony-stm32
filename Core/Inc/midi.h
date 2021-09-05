/*
 * midi.h
 *
 *  Created on: Aug 12, 2021
 *      Author: furkle
 */

#ifndef INC_MIDI_H_
#define INC_MIDI_H_

#define MIDI_BUFFER_LENGTH 512
// Not allowing any SysEx.
#define MIDI_DATABYTE_MAX 4
#define MIDI_NOTES_MAX 128

#define MAX_CHANNELS 16
#define MAX_VOICES 3

#include <stdlib.h>
#include <stdint.h>
#include "misc.h"

////public typedef////
typedef enum {
	START_ANALYSIS,    // Initial Status, including exception.
	WAIT_DATA1,        // Waiting data byte (1st byte)
	WAIT_DATA2,        // Waiting data byte (2nd byte)
	END_ANALYSIS       // Analysis is ended.
} analysis_status_t;

typedef enum {
	MSG_NOTHING,    // Exception (can't resolved, missing data, etc.)
	MSG_NOTE_ON,    // Note-on message
	MSG_NOTE_OFF,   // Note-off message
	MSG_PITCH,      // PitchBend message
	MSG_CC,         // Control Change message
	MSG_PROG        // Program Change message
} event_type_t;

typedef struct {
	event_type_t type;
	uint8_t channel;
	uint8_t data_byte[MIDI_DATABYTE_MAX]; //data_byte[0]=MSB, [1]=LSB, [2]=OTHER...(e.g. sysEx, Control Change...)
} midi_event_t;

typedef struct {
	analysis_status_t stat;
	uint8_t data_idx;
} midi_analysis_status_t;

typedef struct {
	// 0 to 15, corresponding to 16 MIDI channels
	uint8_t channel_number;
} midi_status_t;

//// public variables ////
extern midi_status_t midi_status;
extern ring_buffer_u8_t rxbuf;
extern uint8_t midi_buf;
voice_t voices[MAX_VOICES];

//// public func ////
extern void midi_main();
extern func_status_t midi_init();
extern buffer_status_t midi_buffer_enqueue(uint8_t *inputc);
extern buffer_status_t midi_buffer_dequeue();
extern int midi_buffer_is_empty();
extern void midi_voice_init(voice_t *voice);

#endif /* INC_MIDI_H_ */
