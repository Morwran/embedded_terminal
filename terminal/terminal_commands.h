/*
 * terminal_commands.h
 *
 *  Created on: 12.11.2016
 *      Author: ordweist
 */

#ifndef TERMINAL_COMMANDS_H_
#define TERMINAL_COMMANDS_H_

#include "terminal.h"

/** @brief terminal command length*/
#define TERM_CMDLEN		80

typedef void handler_func(uint8_t, uint8_t, char **);
typedef handler_func *handler_ptr;

/** @brief Command table structure*/
typedef struct t_cmd_struct {
	uint8_t name[TERM_CMDLEN];
	handler_ptr handler;

} t_cmd_t;

uint8_t get_max_handlers();

#endif /* TERMINAL_COMMANDS_H_ */
