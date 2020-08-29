/**
  ******************************************************************************
  * @file           : buffers.c
  * @brief           The Functions that realized FIFO structure for UART
  ******************************************************************************

  */

#include "buffers.h"


/**
   * @brief FIFO initialization function.
   * @param * buf: - Pointer to the FIFO structure;
   * @param * memaddr: - Pointer to the allocated memory for this FIFO;
   * @param bytelen: - memory size.
   * @retval None.
*/
void buf_init(buf_t * buf, uint8_t * memaddr, uint16_t bytelen){
	buf->head = memaddr;
	buf->wr = 0;
	buf->rd = 0;
	buf->lenmax = bytelen - 1;
}

/**
   * @brief Function for writing data to the FIFO.
   * @param * buf: - Pointer to the FIFO structure;
   * @param data: - Data to write to the FIFO.
   * @retval None.
   */
void buf_push_once(buf_t * buf, uint8_t data){
	buf->head[buf->wr++] = data;
	buf->wr &= buf->lenmax;
}

/**
   * @brief FIFO read function.
   * @param * buf: - Pointer to the FIFO structure;
   * @retval d: - data from the FIFO.
   */
uint8_t buf_pop_once(buf_t * buf){
	uint8_t d = buf->head[buf->rd++];
	buf->rd &= buf->lenmax;
	return d;
}

/**
   * @brief Function for writing a data array to a FIFO.
   * @param * buf: - Pointer to the FIFO structure;
   * @param * src_addr: - pointer to data array;
   * @param bytelen: - the size of the data array;
   * @retval None.
   */
void buf_push_len(buf_t * buf, uint8_t * src_addr, uint16_t bytelen){
	uint16_t i;
	for(i = 0; i < bytelen; i++){
		buf_push_once(buf, *(src_addr + i));
	}
}

/**
   * @brief FIFO check function.
   * Checks if the FIFO is empty.
   * @param * buf: Pointer to the FIFO.
   * @retval True - the FIFO is empty. False - the FIFO is not empty.
   */
_Bool buf_isempty(buf_t * buf){
	if(buf->wr == buf->rd){
		return 1;
	}
	return 0;
}

/**
   * @brief FIFO check function.
   * Checks if the FIFO is full.
   * @param * buf: Pointer to the FIFO.
   * @retval True - the FIFO is full. False - the FIFO is empty.
   */
_Bool buf_isFull(buf_t * buf){
	if((buf->wr-buf->rd) == buf->lenmax){
		return 1;
	}
	return 0;
}

/**
   * @brief FIFO cleaning function.
   * @param * buf: Pointer to the FIFO.
   * @retval None
   */
void buf_clear(buf_t * buf){
	buf->wr = 0;
	buf->rd = 0;
}



