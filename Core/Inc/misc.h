/*
 * misc.h
 *
 *  Created on: Aug 12, 2021
 *      Author: furkle
 */

#ifndef INC_MISC_H_
#define INC_MISC_H_

typedef enum {
	FUNC_ERROR,
	FUNC_SUCCESS
} func_status_t;

typedef enum {
	NOTE_OFF,
	NOTE_ON
} note_active_t;

typedef struct {
	uint8_t note_number;
	note_active_t active;
	uint16_t velocity;
	uint16_t pitchbend;
} voice_t;

#endif /* INC_MISC_H_ */
