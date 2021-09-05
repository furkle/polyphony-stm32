/*
 * midi.c
 *
 *  Created on: Aug 12, 2021
 *      Author: furkle
 */

#include <stdio.h>

#include "buffer.h"
#include "midi.h"

//// private variables ////
midi_analysis_status_t analyzed_status;
midi_event_t midi_event;

//// public variables ////
midi_status_t root;
ring_buffer_u8_t rxbuf;
uint8_t midi_buf;

//// private func ////
void midi_set_note_on();
void midi_set_note_off();

int midi_event_is_generated();
void midi_analyze_event();

void midi_main()
{
	while (midi_buffer_dequeue(&midi_buf) == BUFFER_SUCCESS) {
		if (midi_event_is_generated()) {
			// Generate MIDI event from UART buffer.
			midi_analyze_event();
		}
	}
}

func_status_t midi_init()
{
	if (ring_buffer_u8_init(&rxbuf, MIDI_BUFFER_LENGTH) == BUFFER_FAILURE) {
		return FUNC_ERROR;
	}

	uint8_t i;

	for (i = 0; i < MAX_VOICES; i++) {
		midi_voice_init(&voices[i]);
	}

	return FUNC_SUCCESS;
}

void midi_voice_init(voice_t *voice) {
	voice->note_number = 0;
	voice->active = NOTE_OFF;
	voice->pitchbend = 0;
	voice->velocity = 127;
}

buffer_status_t midi_buffer_enqueue(uint8_t *inputc)
{
	return ring_buffer_u8_enqueue(&rxbuf, inputc);
}

buffer_status_t midi_buffer_dequeue(uint8_t *buf)
{
	return ring_buffer_u8_dequeue(&rxbuf, buf);
}

int midi_buffer_is_empty()
{
	if (0 == _ring_buffer_u8_get_used_size(&rxbuf)) {
		return 1;
	}

	return 0;
}

int midi_event_is_generated()
{
	uint8_t upper_half_byte = midi_buf & 0xF0;
	uint8_t lower_half_byte = midi_buf & 0x0F;

	if (upper_half_byte & 0x80) {
		// status byte.
		if (0xF0 != upper_half_byte) {
			// MIDI Channel Message.
			switch (upper_half_byte) {
				case 0x90:
					// Note On Message.
					midi_event.type = MSG_NOTE_ON;
					analyzed_status.stat = WAIT_DATA1;
					midi_event.channel = lower_half_byte;
					break;

				case 0x80:
					// Note Off Message.
					midi_event.type = MSG_NOTE_OFF;
					analyzed_status.stat = WAIT_DATA1;
					midi_event.channel = lower_half_byte;
					break;

				case 0xE0:
					// Pitch Bend.
					midi_event.type = MSG_PITCH;
					analyzed_status.stat = WAIT_DATA1;
					midi_event.channel = lower_half_byte;
					break;

				case 0xB0:
					// Control Change
					midi_event.type = MSG_CC;
					analyzed_status.stat = WAIT_DATA1;
					midi_event.channel = lower_half_byte;
					break;

				case 0xC0:
					// Program Change
					midi_event.type = MSG_PROG;
					analyzed_status.stat = WAIT_DATA1;
					midi_event.channel = lower_half_byte;
					break;

				default:
					midi_event.type = MSG_NOTHING;
					analyzed_status.stat = START_ANALYSIS;
					break;
			}
		}
	} else {
		// data byte
		switch (analyzed_status.stat) {
			case WAIT_DATA1:
				midi_event.data_byte[0] = midi_buf;
				if (midi_event.type == MSG_NOTE_ON ||
					midi_event.type == MSG_NOTE_OFF ||
					midi_event.type == MSG_PITCH ||
					midi_event.type == MSG_CC)
				{
					analyzed_status.stat = WAIT_DATA2;
				} else if (midi_event.type == MSG_PROG) {
					analyzed_status.stat = END_ANALYSIS;
				} else {
					analyzed_status.stat = START_ANALYSIS;
				}

				break;

			case WAIT_DATA2:
				midi_event.data_byte[1] = midi_buf;
				analyzed_status.stat = END_ANALYSIS;

				break;

			case END_ANALYSIS:
				// running status
				midi_event.data_byte[0] = midi_buf;
				analyzed_status.stat = WAIT_DATA2;

				break;

			case START_ANALYSIS:
				break;

			default:
				break;
		}
	}

	return analyzed_status.stat == END_ANALYSIS;
}

void midi_set_note_on()
{
	uint8_t i;
	for (i = 0; i < MAX_VOICES; i++) {
		if (voices[i].active == NOTE_OFF) {
			voices[i].active = NOTE_ON;
			voices[i].note_number = midi_event.data_byte[0];
			voices[i].velocity = midi_event.data_byte[1];

			break;
		}
	}
}

void midi_set_note_off()
{
	uint8_t i;
	for (i = 0; i < MAX_VOICES; i++) {
		if (voices[i].active == NOTE_ON && voices[i].note_number == midi_event.data_byte[0]) {
			voices[i].active = NOTE_OFF;
			// voices[i].note_number = 0;
			voices[i].velocity = midi_event.data_byte[1];

			break;
		}
	}
}

void midi_set_pitch_bend()
{
	uint16_t lsb = midi_event.data_byte[0];
	uint16_t msb = midi_event.data_byte[1];
	uint16_t pitchbend = (((msb) & 0x7F) << 7) + (lsb);
	uint8_t i;

	for (i = 0; i < MAX_VOICES; i++) {
		voices[i].pitchbend = pitchbend;
	}
}

void midi_set_control_change()
{
	return;
}

void midi_set_program_change()
{
	return;
}

void midi_analyze_event()
{
	if (midi_event.channel != root.channel_number) {
		return;
	}

	switch (midi_event.type) {
		case MSG_NOTE_ON:
			midi_set_note_on();
			break;

		case MSG_NOTE_OFF:
			midi_set_note_off();
			break;

		case MSG_PITCH:
			midi_set_pitch_bend();
			break;

		case MSG_CC:
			midi_set_control_change();
			break;

		case MSG_PROG:
			midi_set_program_change();
			break;

		default:
			break;
	}
}
