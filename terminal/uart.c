/**
  ******************************************************************************
  * @file           : uart.c
  * @brief           Example for low level UART driver
  ******************************************************************************


  ******************************************************************************
  */

//#include "xparameters.h"
//#include "xuartlite_l.h"
//#include "sys.h"
#include "uart.h"
#include "buffers.h"
#include "terminal.h"
#include "CFG.h"



//uartregs_t * uart1 = (uartregs_t *)XPAR_RPI_UART_BASEADDR;
/** @brief list of UART base addresses */
const uint32_t uart_list[]={XPAR_AXI_UARTLITE_0_BASEADDR,
				XPAR_AXI_UARTLITE_1_BASEADDR,
				XPAR_AXI_UARTLITE_2_BASEADDR,
				XPAR_AXI_UARTLITE_3_BASEADDR,
				XPAR_AXI_UARTLITE_4_BASEADDR,
				XPAR_AXI_UARTLITE_5_BASEADDR,
				XPAR_AXI_UARTLITE_6_BASEADDR,
				XPAR_AXI_UARTLITE_7_BASEADDR,
				XPAR_AXI_UARTLITE_8_BASEADDR

};

/** @brief Array of UART structures*/
UART_HandleTypeDef huart[M_NUM];

//uartregs_t * huart[M_NUM];

/**
  * @brief  INIT UART
  * @retval None.
  */
void uart_init(){
	for(uint8_t i=0;i<M_NUM;i++){
		//huart[i]= (uartregs_t *)uart_list[i];
		//uart_rst(huart[i]);
		huart[i].inst = (uartregs_t *)uart_list[i];
		huart[i].id = i;
		uart_rst(huart[i].inst);
		//term_init(huart[i].txbuf,huart[i].rxbuf,i);
	}

}


/**
  * @brief UART interrupt enable
  * @param  *uart: Pointer to UART registers structure.
  * @retval None.
  */
void uart_intr_en(uartregs_t * uart){
	uart->ctrl |= UART_ENABLE_INTR;
}
/*
void uart_intr_dis(uartregs_t * uart){
	uart->ctrl &= ~UART_ENABLE_INTR;
}
*/

/**
  * @brief  Reset of UART FIFO
  * @param  *uart: Pointer to UART registers structure.
  * @retval None.
  */
void uart_rst(uartregs_t * uart){

	uart->ctrl = UART_FIFO_RX_RESET | UART_FIFO_TX_RESET;
}

/**
  * @brief  Reset of receive UART FIFO
  * @param  *uart: Pointer to UART registers structure.
  * @retval None.
  */
void uart_rst_rx(uartregs_t * uart){
	uart->ctrl = UART_FIFO_RX_RESET;
}

/**
  * @brief  Reset of transmit UART FIFO
  * @param  *uart: Pointer to UART registers structure.
  * @retval None.
  */
void uart_rst_tx(uartregs_t * uart)
{
	uart->ctrl = UART_FIFO_TX_RESET;
}

void uart_rx_rst_by_id(uint8_t id)
{

	uart_rst_rx(huart[id].inst);
}

void uart_tx_rst_by_id(uint8_t id){

	uart_rst_tx(huart[id].inst);
}

void uart_rst_by_id(uint8_t id){

	uart_rst(huart[id].inst);
}

/*
inline _Bool uart_istxempty(uartregs_t * uart){
	if((uart->state & UART_TX_FIFO_EMPTY) == UART_TX_FIFO_EMPTY){
		return TRUE;
	}
	return FALSE;
}
*/

/**
  * @brief  Check status of UART transit FIFO.
  * @param  *uart: Pointer to UART registers structure.
  * @retval True - FIFO full. False - FIFO not full.
  */
inline _Bool uart_istxfull(uartregs_t * uart){
	if((uart->state & UART_TX_FIFO_FULL) == UART_TX_FIFO_FULL){
		return 1;
	}
	return 0;
}

/**
  * @brief  Check status of UART receive FIFO.
  * @param  *uart:  Pointer to UART registers structure.
  * @retval True - FIFO is empty. False - FIFO not empty.
  */
inline _Bool uart_isrxempty(uartregs_t * uart){
	if((uart->state & UART_RX_FIFO_NOT_EMPTY) == UART_RX_FIFO_NOT_EMPTY){
		return 1;
	}
	return 0;
}


/**
  * @brief receive byte of data from UART.
  * @param * uart: - Pointer to UART registers structure.
  * @retval uint8_t - receive byte of data.
  */
inline uint8_t uart_recv(uartregs_t * uart){
	return uart->rx;
}

/**
  * @brief send byte of data to UART.
  * @param * uart: - Pointer to UART registers structure.
  * @retval None.
  */
void uart_send(uartregs_t * uart, uint8_t data){
	uart->tx = data;
}

/**
  * @brief Return UART status register
  * @param * uart: -  Pointer to UART registers structure.
  * @retval uint8_t - status.
  */
inline uint8_t uart_status(uartregs_t * uart){
	return uart->state;
}

