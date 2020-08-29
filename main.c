/*
 * main.c
 *
 *  Created on: 28.08.2020
 *      Author: kvb
 */

#include "terminal/terminal.h"

extern UART_HandleTypeDef huart[M_NUM];


/**
  * @brief  The application entry point to show how to use.
  *
  * @retval None
  */
int main(void)
{
	term_init();

	for(uint8_t i=0;i<M_NUM;i++)
		terminal_service(&huart[i]);



	return 0;
}
