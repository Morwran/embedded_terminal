/**
  ******************************************************************************
  * @file           : terminal_commands.c
  * @brief           terminal handler
  ******************************************************************************

  */

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "string.h"
#include "terminal_commands.h"

#define last_ver_is (branch_ver[branch_ver_size()-1].ver)
#define last_ver_is_about (branch_ver[branch_ver_size()-1].about)

extern const t_cmd_t t_commands[];

/** @brief version structure*/
typedef struct ver{
	char *ver;
	char *about;
}version_t;

/** @brief History table of version and version about*/
const version_t branch_ver[]={\
		{"1.09","fix leak"},\
		{"1.10","running new diagnostic by btn"},\
		{"1.11","switch off rl in when ch off"},\
		{"1.12","locking btn when diagnostic proc"},\
		{"1.13","adding version branch"},\
		{"1.14","change current control"}\
};

typedef char char_buff[20];
/** @brief List of related device addresses */
const char_buff ADDR_HW[]={
		{"m0"}, \
		{"m1"}, \
		{"m2"}, \
		{"m3"}, \
		{"m4"}, \
		{"m5"}, \
		{"m6"}, \
		{"m7"}, \
		{"m8"}, \
		{"m9"},\

};


static inline uint8_t branch_ver_size()
{
	return (sizeof(branch_ver)/sizeof(version_t));
}

/**
   * @brief choosing next version and version about from history table.
   * @param _Bool direction_up: - Pointer to the FIFO.
   * @retval version_t: - selected version and version about.
   */
static version_t choose_next_ver(_Bool direction_up)
{
	static uint8_t ver_id=(sizeof(branch_ver)/sizeof(version_t))-1;
	if(direction_up)
		ver_id++;
	else
		ver_id--;
	if(ver_id>=branch_ver_size()){
		if(direction_up)
			ver_id=0;
		else
			ver_id=branch_ver_size()-1;
	}

	return branch_ver[ver_id];

}

/**
  * @brief Example of handler function for available command from command table
  * Output of register values at a given address.
  * @param id: - number of request port.
  * @param argc: - number of arguments.
  * @param argv[]: - Pointer to argument array.
  * @retval None.
  */

static void mrd(uint8_t id, uint8_t argc, char * argv[]){
	uint8_t i, n = 1;
	if(!argc){
		return;
	} else if (argc > 2) {
		n = str2unsigned(argv[2]);
		tprintf(id,"mrd : %d\r\n", n);
	}
	uint32_t *addr = (uint32_t *)str2unsigned(argv[1]);
	tprintf(id,"mrd : %8X\r\n", addr);
	for(i = 0; i < n; i++)
		tprintf(id,"0x%8X\t: 0x%8X\r\n", addr + i, *(addr + i));
	return;
}

/**
  * @brief Example of handler function for available command from command table
  * Writing values to a register at a given address.
  * @param id: - number of request port.
  * @param argc: - number of arguments.
  * @param argv[]: - Pointer to argument array.
  * @retval None.
  */

static void mwr(uint8_t id, uint8_t argc, char * argv[]){
	if(argc < 2){
		return;
	}
	uint32_t *addr = (uint32_t *)str2unsigned(argv[1]);
	uint32_t data = str2unsigned(argv[2]);

	*addr = data;
	return;
}



/**
  * @brief Example of handler function for available command from command table
  * Return version and version about.
  * @param id: - number of request port.
  * @param argc: - number of arguments.
  * @param argv[]: - Pointer to argument array.
  * @retval None.
  */
static void version_handler(uint8_t id, uint8_t argc, char * argv[]){
	if(argc==2){
		if(strcmp(ADDR_HW[id],argv[1])==0){
			tprintf(id,"rply ver %s %s\r\n",last_ver_is,ADDR_HW[id]);

			return;
		}
		return;
	}
	if(argc==3){
		if(strcmp(ADDR_HW[id],argv[2])==0){

			if(strcmp("about",argv[1])==0){
				tprintf(id,"rply v%s about: %s %s\r\n",last_ver_is,last_ver_is_about,ADDR_HW[id]);

				return;
			}
			if(strcmp("branch",argv[1])==0){
				for(int i=0;i<branch_ver_size();i++)
					tprintf(id,"rply v%s about: %s %s\r\n",branch_ver[i].ver,branch_ver[i].about,ADDR_HW[id]);


				return;
			}
			if(strcmp("prev",argv[1])==0){

				version_t ver_sel=choose_next_ver(0);

				tprintf(id,"rply v%s about: %s %s\r\n",ver_sel.ver,\
						ver_sel.about,ADDR_HW[id]);

				return;

			}
			if(strcmp("next",argv[1])==0){
				version_t ver_sel=choose_next_ver(1);

				tprintf(id,"rply v%s about: %s %s\r\n",ver_sel.ver,\
						ver_sel.about,ADDR_HW[id]);

				return;
			}
\
		}
	}
	return;
}
/**
  * @brief Example of handler function for available command from command table
  * Return available terminal commands.
  * @param id: - number of request port.
  * @param argc: - number of arguments.
  * @param argv[]: - Pointer to argument array.
  * @retval None.
  */
static void help_handler(uint8_t id, uint8_t argc, char * argv[]){
	if(argc==1){
		tprintf(id,"Commands table:\r\n");
		for(uint8_t i=0;i<get_max_handlers();i++)
			tprintf(id,"%s\r\n",t_commands[i].name);
	}
}

/** @brief Command table*/
const t_cmd_t t_commands[] = \
		{	\
			{"help",  		(handler_ptr)help_handler				},\
			{"mrd",    		(handler_ptr)mrd						},\
			{"mwr",    		(handler_ptr)mwr						},\
			{"version",    	(handler_ptr)version_handler			}, \

		};

uint8_t get_max_handlers(){
	return (sizeof(t_commands)/sizeof(t_cmd_t));
}




