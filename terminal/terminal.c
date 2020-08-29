/**
  ******************************************************************************
  * @file           : terminal.c
  * @brief           The core of terminal service
  ******************************************************************************
  * This file include main functions to realize terminal service

  ******************************************************************************
  */

#include <stdarg.h>
#include <string.h>

#include "terminal.h"
#include "uart.h"

#include "terminal_commands.h"
#include "buffers.h"


//#include "board_config.h"

/** @brief transceivers buffers size*/
#define TERM_BUFLEN			2048
#define RX_TERM_BUFLEN		1024
#define Srx_TERM_BUFLEN		1024
#define Stx_TERM_BUFLEN		1024

/** @brief Maximum number of arguments in command line*/
#define MAX_ARGS			20
/** @brief Sign position in signed data*/
#define SIGNPOS				0x80000000
/** @brief ACSII */
#define BACKSPACE_UNIX		0x08
#define BACKSPACE_WIN		0x7F
#ifdef TERM_INVITE_STR
	#define TERM_INVITE_STR		"sp3-kit-> "
#endif

//#define DEBUG

typedef struct uart_mem_rx { uint8_t mem[Srx_TERM_BUFLEN]; }uart_mem_rx;
typedef struct uart_mem_tx { uint8_t mem[Stx_TERM_BUFLEN]; }uart_mem_tx;
typedef struct str_mem {char mem[TERM_CMDLEN]; }str_mem;


uint8_t term_txmem[TERM_BUFLEN];
uint8_t term_rxmem[RX_TERM_BUFLEN];

char eth_mem[TERM_BUFLEN];

buf_t eth_stk;

uart_mem_tx slvs_txmem[M_NUM-1];
uart_mem_rx slvs_rxmem[M_NUM-1];

/** @brief FIFO structures for transmitting data to UART */
buf_t txbuf[M_NUM];
/** @brief FIFO structures for receiving data from UART */
buf_t rxbuf[M_NUM];

uint8_t echo = 0;

extern const t_cmd_t t_commands[];
extern UART_HandleTypeDef huart[M_NUM];
//extern rc_srec_pkt_t srec_pkt;
//extern uartregs_t * huart[M_NUM];

void tx_buf_clr_by_id(uint8_t id)
{
	buf_clear(&txbuf[id]);
}

void rx_buf_clr_by_id(uint8_t id)
{
	buf_clear(&rxbuf[id]);
}

/**
  * @brief INIT FIFO structures.
  * @retval None.
  */
void term_init(){
	buf_init(&eth_stk, (uint8_t*)eth_mem, TERM_BUFLEN);
	for(uint8_t i=0;i<M_NUM;i++){
		if(i==0){
			buf_init(&txbuf[0], term_txmem, TERM_BUFLEN);
			buf_init(&rxbuf[0], term_rxmem, RX_TERM_BUFLEN);
		}else{
			buf_init(&txbuf[i], slvs_txmem[i-1].mem, Stx_TERM_BUFLEN);
			buf_init(&rxbuf[i], slvs_rxmem[i-1].mem, Srx_TERM_BUFLEN);
		}
	}
}


/**
  * @brief convert numbers to print.
  * @retval None.
  */
void tprint_int(uint8_t id, uint32_t n, uint8_t chksign, uint8_t numwidth, uint8_t base){
	char buf[TERM_NUMLEN] = { 0 };
	//char *rez_p=buf;
	uint8_t i = 0;
	uint32_t rem = 0;
	uint8_t sign = chksign && ((n & SIGNPOS) == SIGNPOS);
	//xil_printf("n_in %d base %d\r\n",n,base);
	if(0 == n){
		if(numwidth){
			for(i = 0; i < numwidth; i++){
				if(id!=ETH_ID)
					buf_push_once(&txbuf[id], '0');
				else
					buf_push_once(&eth_stk, '0');

				//*rez_p++='0';

			}
		} else {
			if(id!=ETH_ID)
				buf_push_once(&txbuf[id], '0');
			else
				buf_push_once(&eth_stk, '0');

			//*rez_p++='0';

		}
		return;
	}
	if(sign){
		n = ~n;
		n++;
		if(id!=ETH_ID)
			buf_push_once(&txbuf[id], '-');
		else
			buf_push_once(&eth_stk, '-');

		//*rez_p++='-';
	}
	while((n) && (i < TERM_NUMLEN)){
		rem = n % base;
		//xil_printf("rem %d n %d base %d\r\n",rem,n,base);
		if(rem < 10){
			rem += '0';
			//xil_printf("rem <10 %d\r\n",rem);
		} else {
			rem += 'A' - 10;
			//xil_printf("rem >10 %d\r\n",rem);
		}
		buf[i++] = rem;
		n = n / base;
		//xil_printf("n/base %d\r\n",n);
	}
	if(numwidth){
		if(numwidth <= i){
			i = numwidth;
		} else {
			for(; i < numwidth; i++){
				buf[i] = '0';
			}
		}
	}
	for(; i != 0; i--){
		if(id!=ETH_ID)
			buf_push_once(&txbuf[id], buf[i-1]);
		else
			buf_push_once(&eth_stk, buf[i-1]);

		//*rez_p++=buf[i-1];
		//xil_printf("buf %c\r\n",buf[i-1]);
	}
	return;
}

/**
  * @brief print function .
  * @retval None.
  */
void tprintf(uint8_t id, const char * format, ...){
	uint16_t len = 0;
	char *buf;

	uint16_t buflen = 0;
	uint8_t numwidth = 0;

	va_list va;
	va_start(va, format);

	len = TERM_CMDLEN;


	while((*format != 0) && len--){
		// normal char
		if(*format != '%'){
			if(id==ETH_ID)
				//*eth_p++ = *(format++);
				buf_push_once(&eth_stk, *(format++));
			else
				buf_push_once(&txbuf[id], *(format++));
		// or not.
		} else {
			// pass '%' symbol
			format++;
			// check for settings for printsize
			if(('1' <= *format) && (*format <= '9' )){
				numwidth = (*format)-'0';
				format++;
			} else {
				numwidth = 0;
			}
			//xil_printf("numwidth %d\r\n",numwidth);
			// parse next
			switch(*format){
			case '%':
				if(id==ETH_ID)
					//*eth_p++ = *(format);
					buf_push_once(&eth_stk, *(format));
				else
					buf_push_once(&txbuf[id], *(format));
				break;
			case 'u':

				//tprint_int(id,va_arg(va, int), 0, numwidth, 10, eth_p);
				tprint_int(id,va_arg(va, int), 0, numwidth, 10);
				//eth_p+=strlen(eth_p);



				break;
			case 'd':
				//tprint_int(id,va_arg(va, int), 1, numwidth, 10, eth_p);
				tprint_int(id,va_arg(va, int), 1, numwidth, 10);

				//eth_p+=strlen(eth_p);
				break;
			case 'X':
				//tprint_int(id,va_arg(va, int), 0, numwidth, 16, eth_p);
				tprint_int(id,va_arg(va, int), 0, numwidth, 16);
				//eth_p+=strlen(eth_p);

				break;
			case 's':
				buf = va_arg(va, char *);
				buflen = get_strlen(buf);

				if(id==ETH_ID){
					//memcpy(eth_p,buf,buflen);
					//eth_p+=buflen;
					buf_push_len(&eth_stk, (uint8_t *)buf, buflen);
				}
				else
					buf_push_len(&txbuf[id], (uint8_t *)buf, buflen);
				break;
			case 'c':
				if(id==ETH_ID)
					//*eth_p++ = va_arg(va, int);
					buf_push_once(&eth_stk, va_arg(va, int));
				else
					buf_push_once(&txbuf[id], va_arg(va, int));
				break;
			default:
				// on default, print nothing
				break;
			}
			format++;
		}
	}
	va_end(va);

}


void clear_eth_stk(){
	memset(eth_mem,'\0',TERM_BUFLEN);
	buf_clear(&eth_stk);
}


void eth_txbuf_flush(){
	if(!buf_isempty(&eth_stk))
	{
		//sendler(eth_mem);
		clear_eth_stk();
	}
}

/**
  * @brief flush transmit buffer
  * @param None.
  * @retval None.
  */
void term_txbuf_flush(UART_HandleTypeDef *uart){
//	while(uart_istxempty() && !buf_isempty(&txbuf)){
	while(!uart_istxfull(uart->inst) && !buf_isempty(&txbuf[uart->id]))
	{
		uart_send(uart->inst,buf_pop_once(&txbuf[uart->id]));
		//delay(100);
	}
}


/**
  * @brief filter ASCII symbols
  * @param s - ACII symbols
  * @retval bool. True - available symbol. False - unavailable symbol.
  */
_Bool filter_char(uint8_t s){
	if(s==10 || s==13 || s==32 || (s>=44 && s<=46) || (s>=48 && s<=58) || (s>=65 && s<=92) || (s>=97 && s<=122))
		return 1;
	else
		return 0;
}

void uart_resend(UART_HandleTypeDef uart_from, UART_HandleTypeDef uart_to){
	uint8_t t=0;
	while(!uart_isrxempty(uart_from.inst) && t<TERM_CMDLEN){
		uint8_t s = uart_recv(uart_from.inst);
		t++;
		while(!uart_istxfull(uart_to.inst))
			uart_send(uart_to.inst,s);
	}
}

/**
  * @brief printing a dot symbol
  * @param id: - номер порта на передачу.
  * @retval None.
  */
void print_dot_id0(uint8_t id){
	if(!uart_istxfull(huart[id].inst))
		uart_send(huart[id].inst,'.');
	return;
}

/**
  * @brief Receiving process from UART
  * @param * uart: - Pointer to UART structure
  * @retval None.
  */
void term_rcv_uart(UART_HandleTypeDef *uart){
	uint8_t t=0;
	uint8_t st = uart_status(uart->inst);
	if((st & UART_HARD_ERROR) !=0){
		uart_rst_rx(uart->inst);

		uart->cnt_err.cnt_OrE+=U_ERRpp(st,UART_OrE);
		uart->cnt_err.cnt_FE+=U_ERRpp(st,UART_FE);
		uart->cnt_err.cnt_PE+=U_ERRpp(st,UART_PE);

		return;
	}

	//if(!uart_isrxempty(uart.inst)){
	if(uart->id>0){
		while(!uart_isrxempty(uart->inst) && t<TERM_CMDLEN){

			uint8_t s = uart_recv(uart->inst);
			t++;
			//if(uart.id==1)
			//	uart_send(huart[0].inst,s);

			//tprintf(0,"%8X\r\n",s);

			//if(filter_char(s)){
			//if(uart.id==0)
			//	rcv_srec_pkt_from_uart(s);

			buf_push_once(&rxbuf[uart->id], s);

			//}
		}

	}
	else{
		//if(!uart_isrxempty(uart.inst)){
		while(!uart_isrxempty(uart->inst) && t<TERM_CMDLEN){


			uint8_t s = uart_recv(uart->inst);

			//tprintf(ETH_ID,"%c",s);

			//rcv_srec_pkt_from_uart(s);

			//if(!srec_pkt.rcv_proc)
			buf_push_once(&rxbuf[uart->id], s);
			t++;

			return;

		}
	}
	//return;
}


/**
  * @brief Parsing receive command line.
  * @param id: - number of receive port.
  * @param *str: - Pointer to receive line.
  * @param strlen: - Lenght of line.
  * @retval None.
  */
void parse_cmdline(uint8_t id, char *str, uint16_t strlen){
	uint8_t i;
	uint8_t argc = 0;
	char divchars[] = " ";
	char *argv[MAX_ARGS] = { 0 };
//	argv[argc] = strdiv(str, ' ');
	argv[argc] = strtok(str, divchars);
	if(argv[argc++] == 0){
		return;
	}
	for(;argc < MAX_ARGS; argc++){
		argv[argc] = strtok(0, divchars);
		if(!argv[argc]){
			break;
		}
	}

	for (i = 0; i < get_max_handlers(); i++){
		if(tstrcmp(argv[0], (char *)t_commands[i].name, strlen)){
			t_commands[i].handler(id, argc, argv);
			break;
		}
	}
/*	if(i == get_max_handlers()){
		tprintf("unknown command\r\n");
#ifdef DEBUG
		tprintf("%s and %d\r\n", str, strlen);
#endif
	}
*/
	return;
}

/**
  * @brief Pre-processing of receive command line.
  * @param id: - number of receive port.
  * @param с: - receive symbol.
  * @retval None.
  */
void terminal_input(char c, uint8_t id){
	//static char str[TERM_CMDLEN] = { 0x00 };
	//static u16 strlen = 0;
	static str_mem str[M_NUM+1];
	static uint16_t strlen[M_NUM+1] = {0}; //+1 для ethe

	if((c == '\r') || (c == '\n')){
		c = '\0';
	}


	str[id].mem[strlen[id]++] = c;
	if((strlen[id] == TERM_CMDLEN) || (c == '\0')){
		if(strlen[id] > 1){
//			buf_push_once(&txbuf, '\r');
//			buf_push_once(&txbuf, '\n');
			parse_cmdline(id, str[id].mem, strlen[id]);
		}
		strlen[id] = 0;

	}
	return;
}

/**
  * @brief Main terminal service .
  * @param *uart - Pointer to UART structure.
  * @retval None.
  */
void terminal_service(UART_HandleTypeDef *uart){
	uint8_t c;
	uint8_t t=0;

	//if((u32)uart.inst!=(u32)huart[0].inst){
		term_rcv_uart(uart);
	//}
	//term_rcv_uart();
	//if(!buf_isempty(&rxbuf[uart.id])){
	while(!buf_isempty(&rxbuf[uart->id]) && t<TERM_CMDLEN){
		c = buf_pop_once(&rxbuf[uart->id]);
		// for echo proposes
		//echo_ops(c);
		// commands parsing
		terminal_input(c,uart->id);
		t++;
	}
	//intc_master_disable();
	term_txbuf_flush(uart);
	//intc_master_enable();
	return;
}

/**
  * @brief compare two lines.
  * @param *A: - Pointer to line 1.
  * @param *B: - Pointer to line 2.
  * @retval bool
  */
uint16_t tstrcmp(const char *A, const char *B, uint16_t len){
	uint16_t i = 0;
	for(i = 0; i < len; i++){
		if(*A != *B){
			return 0;
		}
		if(*A == '\0'){
			return i;
		}
		A++;
		B++;
	}
	return i;
}

/**
  * @brief Return length of line.
  * @param *buf: - Pointer to char buffer.
  * @retval len: - Length of line.
  */
uint16_t get_strlen(char *buf){
	uint16_t len = 0;
	if(!buf){
		return 0;
	}
	while((*buf++ != '\0') && (len < TERM_BUFLEN)){
		len++;
	}
	return len;
}


/**
  * @brief Converting line to unsigned int data
  * @param *src: - Pointer to line.
  * @retval val: - unsigned int data.
  */
uint32_t str2unsigned(char  * src) {
	if (NULL==src) {
		return 0;
	}

	uint32_t val = 0;
	uint8_t hexbegin[2] = "0x";
	if (tstrcmp(src, (char *)hexbegin, 2)) {
		//������������������������������������������������������������������������������������������������������������������������������������������������ ������������������ ������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������ ������������������������������������������������������������������������
		uint16_t len = get_strlen(src);
		uint16_t i = 0;
		for (i=2;i<len;i++) {
			uint16_t tmp = 0;
			if ('0'<=src[i] && src[i]<='9') tmp=src[i]-'0';
			if ('A'<=src[i] && src[i]<='F') tmp=src[i]-'A'+10;
			if ('a'<=src[i] && src[i]<='f') tmp=src[i]-'a'+10;
			val = (val<<4) | tmp;
		}
	} else {
		//������������������������������������������������������������������������������������������������������������������������������������������������ ������������������ ������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������ ������������������������������������������������������������������������
		uint16_t len = get_strlen(src);
		uint16_t i = 0;
		for (i=0;i<len;i++) {
			uint16_t tmp = src[i]-'0';
			val = (val*10) + tmp;
		}
	}

	return val;
}


char * strdiv(char * A, char divchar){
	static char * begin, * seek;
	static uint16_t i = 0;
	if(A){
		seek  = A;
		begin = A;
	} else {
		seek++;
		begin = seek;
	}
	if(seek == '\0')
		return 0;
	while(*seek != divchar){
		if(*seek == '\0'){
			if(seek == begin){
				return 0;
			} else {
				break;
			}
		}
		seek++;
		i++;
	}
	*seek = '\0';
	return begin;
}


