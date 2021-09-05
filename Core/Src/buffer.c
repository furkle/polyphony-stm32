/*
 * buffer.c
 *
 *  Created on: Aug 12, 2021
 *      Author: furkle
 */

#include <stdint.h>
#include <stdlib.h>

#include "buffer.h"
#include "misc.h"


/////////////////////////////
//methods for uint8_t FIFO.
/////////////////////////////

buffer_status_t ring_buffer_u8_init(ring_buffer_u8_t *rbuf, uint16_t buflen)
{
	uint32_t i;

	ring_buffer_u8_free(rbuf);

	rbuf->buffer = (uint8_t *)malloc(buflen * sizeof(uint8_t));

	if (rbuf->buffer == NULL) {
		return BUFFER_FAILURE;
	}

	for (i=0; i < buflen; i++) {
		rbuf->buffer[i] = 0;
	}

	rbuf->length = buflen;

	return BUFFER_SUCCESS;
}

buffer_status_t ring_buffer_u8_free(ring_buffer_u8_t *rbuf)
{
	if (rbuf->buffer != NULL) {
		free(rbuf->buffer);
	}

	rbuf->idx_front = rbuf->idx_rear = 0;
	rbuf->length = 0;

	return BUFFER_SUCCESS;
}

buffer_status_t ring_buffer_u8_enqueue(ring_buffer_u8_t *rbuf, uint8_t *inputc)
{
	if (((rbuf->idx_front + 1) & (rbuf->length - 1)) == rbuf->idx_rear) {
		// buffer overrun error occurs.
		return BUFFER_FAILURE;
	}

	rbuf->buffer[rbuf->idx_front] = *inputc;
	rbuf->idx_front++;
	rbuf->idx_front &= (rbuf->length - 1);

	return BUFFER_SUCCESS;
}

buffer_status_t ring_buffer_u8_dequeue(ring_buffer_u8_t *rbuf, uint8_t *ret)
{
	if (rbuf->idx_front == rbuf->idx_rear) {
		// if buffer underrun error occurs.
		return BUFFER_FAILURE;
	}

	*ret = (rbuf->buffer[rbuf->idx_rear]);
	rbuf->idx_rear++;
	rbuf->idx_rear &= (rbuf->length -1);
	return BUFFER_SUCCESS;
}

uint16_t _ring_buffer_u8_get_used_size(ring_buffer_u8_t *rbuf)
{
	if (rbuf->idx_front >= rbuf->idx_rear) {
		return rbuf->idx_front - rbuf->idx_rear;
	}

	return rbuf->idx_front + rbuf->length - rbuf->idx_rear;
}
