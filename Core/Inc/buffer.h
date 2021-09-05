/*
 * buffer.h
 *
 *  Created on: Aug 12, 2021
 *      Author: furkle
 */

#ifndef INC_BUFFER_H_
#define INC_BUFFER_H_

#include <stdint.h>

typedef enum {
	BUFFER_FAILURE,
	BUFFER_SUCCESS
} buffer_status_t;

typedef struct {
	uint16_t idx_front;
	uint16_t idx_rear;
	uint16_t length;
	uint8_t  *buffer;
} ring_buffer_u8_t;

/////////////////////////////
//methods for uint8_t FIFO.
/////////////////////////////
extern buffer_status_t ring_buffer_u8_init(ring_buffer_u8_t *rbuf, uint16_t buflen);
extern buffer_status_t ring_buffer_u8_free(ring_buffer_u8_t *rbuf);
extern buffer_status_t ring_buffer_u8_enqueue(ring_buffer_u8_t *rbuf, uint8_t *inputc);
extern buffer_status_t ring_buffer_u8_dequeue(ring_buffer_u8_t *rbuf, uint8_t *ret);

// debug
extern uint16_t _ring_buffer_u8_get_used_size(ring_buffer_u8_t *rbuf);

#endif /* INC_BUFFER_H_ */
