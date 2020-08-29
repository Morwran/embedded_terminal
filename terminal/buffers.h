/*
 * buffers.h
 *
 *  Created on: 11.11.2016
 *      Author: ordweist
 */

#include "stdint.h"

/** @brief FIFO structure*/
typedef struct{
	uint8_t * head;
	uint16_t wr;
	uint16_t rd;
	uint16_t lenmax;
}buf_t;

void buf_init(buf_t * buf, uint8_t * memaddr, uint16_t bytelen);
void buf_push_len(buf_t * buf, uint8_t * src_addr, uint16_t bytelen);
void buf_push_once(buf_t * buf, uint8_t data);
uint8_t buf_pop_once(buf_t * buf);

_Bool buf_isempty(buf_t * buf);
_Bool buf_isFull(buf_t * buf);
void buf_clear(buf_t * buf);
