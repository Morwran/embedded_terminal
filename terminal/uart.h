/*
 * uart.h
 *
 *  Created on: 11.11.2016
 *      Author: ordweist
 */

#ifndef UART_H_
#define UART_H_
#include "stdint.h"
//#include <xil_types.h>
//#include "buffers.h"
/** @brief Base addresses of UART*/
#define XPAR_AXI_UARTLITE_0_BASEADDR 0x40000000
#define XPAR_AXI_UARTLITE_1_BASEADDR 0x40100000
#define XPAR_AXI_UARTLITE_2_BASEADDR 0x40200000
#define XPAR_AXI_UARTLITE_3_BASEADDR 0x40300000
#define XPAR_AXI_UARTLITE_4_BASEADDR 0x40400000
#define XPAR_AXI_UARTLITE_5_BASEADDR 0x40500000
#define XPAR_AXI_UARTLITE_6_BASEADDR 0x40600000
#define XPAR_AXI_UARTLITE_7_BASEADDR 0x40700000
#define XPAR_AXI_UARTLITE_8_BASEADDR 0x40800000

#define UART_ENABLE_INTR			0x10	/* enable interrupt */
#define UART_FIFO_RX_RESET			0x02	/* reset receive FIFO */
#define UART_FIFO_TX_RESET			0x01	/* reset transmit FIFO */

#define UART_OrE					0x20	//overrun error
#define UART_FE						0x40	//frame error
#define UART_PE						0x80	//parity error
#define UART_HARD_ERROR            (UART_PE | UART_FE | UART_OrE)

#define UART_INTR_ENABLED			0x10	/* interrupt enabled */
#define UART_TX_FIFO_FULL			0x08	/* transmit FIFO full */
#define UART_TX_FIFO_EMPTY			0x04	/* transmit FIFO empty */
#define UART_RX_FIFO_FULL			0x02	/* receive FIFO full */
#define UART_RX_FIFO_NOT_EMPTY		0x01	/* data in receive FIFO */

#define UART_FIFO_SIZE			16

/** @brief UART registers structure*/
typedef struct uartregs {
	volatile uint32_t rx;
	volatile uint32_t tx;
	volatile uint32_t state;
	volatile uint32_t ctrl;

} uartregs_t;

/** @brief UART counters of error*/
typedef struct{
	uint32_t cnt_OrE;
	uint32_t cnt_FE;
	uint32_t cnt_PE;
}uart_cnt_err_t;

#define U_ERRpp(__st__,__err__)((__st__&__err__)?1:0)

/** @brief UART structure*/
typedef struct{
	//buf_t *txbuf;
	//buf_t *rxbuf;
	uint8_t id;
	uart_cnt_err_t cnt_err;
	uartregs_t *inst;

}UART_HandleTypeDef;


//#define uart_status(_uart_)	(_uart_->state)

void uart_init();

void uart_intr_en(uartregs_t * uart);
void uart_intr_dis(uartregs_t * uart);
void uart_rst(uartregs_t * uart);
void uart_rst_rx(uartregs_t * uart);
void uart_rst_tx(uartregs_t * uart);

void uart_rx_rst_by_id(uint8_t id);
void uart_tx_rst_by_id(uint8_t id);
void uart_rst_by_id(uint8_t id);

_Bool uart_istxempty(uartregs_t * uart);
_Bool uart_istxfull(uartregs_t * uart);
_Bool uart_isrxempty(uartregs_t * uart);


uint8_t uart_recv(uartregs_t * uart);
void uart_send(uartregs_t * uart,uint8_t data);
uint8_t uart_status(uartregs_t * uart);

#endif /* UART_H_ */
