/*
 * terminal.h
 *
 *  Created on: 11.11.2016
 *      Author: kvb
 *      mail: mat3x@mail.ru
 */

#ifndef TERMINAL_H_
#define TERMINAL_H_

//#include <xil_types.h>
#include "uart.h"
#include "CFG.h"

//#include "buffers.h"

#define TERM_NUMLEN     	10

void term_init();
//void term_init(buf_t *txbuf,buf_t *rxbuf,u8 id);
void term_intrpt_hander();
void term_txbuf_flush(UART_HandleTypeDef *uart);
void term_rcv_uart(UART_HandleTypeDef *uart);
void uart_resend(UART_HandleTypeDef uart_from, UART_HandleTypeDef uart_to);
void print_dot_id0(uint8_t id);

void terminal_service(UART_HandleTypeDef *uart);
void terminal_input(char c, uint8_t id);
_Bool filter_char(uint8_t s);
//void terminal_input(char newchar);
//void parse_cmdline(char *str, u16 strlen);
//void tprint_int(u32 n, u8 chksign, u8 numwidth, u8 base);
#define tprint tprintf

void tprintf(uint8_t id,const char * format, ...);

uint16_t tstrcmp(const char *A, const char *B, uint16_t len);
uint16_t get_strlen(char *buf);
uint32_t str2unsigned(char * src);
char * strdiv(char * A, char divchar);
void parse_cmdline(uint8_t id, char *str, uint16_t strlen);

void eth_txbuf_flush();

void tx_buf_clr_by_id(uint8_t id);
void rx_buf_clr_by_id(uint8_t id);

#endif /* TERMINAL_H_ */
