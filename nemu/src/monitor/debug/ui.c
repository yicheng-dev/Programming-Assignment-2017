#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"
#include "math.h"
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <readline/readline.h>
#include <readline/history.h>


void cpu_exec(uint64_t);

/* We use the `readline' library to provide more flexibility to read from stdin. */
char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_c(char *args) {
  cpu_exec(-1);
  return 0;
}

static int cmd_q(char *args) {
  return -1;
}

static int cmd_help(char *args);

static int cmd_info(char *args)
{	
	int i;
	if (args==NULL){
		printf("You may want to input commands like \"info r\" or \"info w\".Try again!\n");
		return 0;
	}
	if (strcmp(args,"r")==0)
	{
		for (i=0;i<8;i++)
		{
			printf("%s:0x%x\n",reg_name(i,4),reg_l(i));
		}

	}
	else if (strcmp(args,"w")==0)
		print_watchpoint();

	else
		printf("You may want to input commands like \"info r\" or \"info w\".Try again!\n");
	return 0;
}

static int cmd_si(char *args)
{
	args= strtok(NULL," ");
	uint64_t exec_num;
	if (args==NULL){
		cpu_exec(1);
	}
	else if (strlen(args)==1&&args[0]>='1'&&args[0]<='9'){
		exec_num = atoi(args);
		cpu_exec(exec_num);
	}
	else if (strlen(args)==2&&args[0]=='1'&&args[1]=='0'){
		cpu_exec(10);
	}
	else
		printf("Please enter a number between 1 and 10!\n");
	return 0;
}

static int cmd_x(char *args)
{
	int N;
	int address;
	const int step=4;
	int j,k;
	args=strtok(NULL," ");
	if (args==NULL){
		printf("Lack arguments!You may want to input command like \"x [N] [address]\"\n");
		return 0;
	}
	N=atoi(args);
	args=strtok(NULL," ");
	if (args==NULL){
		printf("Lack arguments!You may want to input command like \"x [N] [address]\"\n");
		return 0;
	}
	sscanf(args, "%x", &address);
	
	
	for (j=0;j<N;j++)
	{
		printf("0x%08x: ",address);
		for (k=0;k<step;k++)
		{
			printf("%02x ",pmem[address+k]);
		}
		address+=step;
		printf("\n");
	}
	
	return 0;
	
}

static int cmd_p(char *args)
{
	bool success = true;
	bool *success_ptr= & success;
	uint32_t ans;
	ans = expr(args,success_ptr);
	if (*success_ptr)
		printf("%u\n",ans);
	else
		printf("Something was wrong with your command!\n");
	return 0;
}

static int cmd_w(char *args)
{
	args=strtok(NULL," ");
	new_wp(args);
	return 0;
}

static int cmd_d(char *args)
{
	args=strtok(NULL," ");
	free_wp(args);
	return 0;
}

static struct {
  char *name;
  char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display informations about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },
  {"info","Print the information of registers or watchers.",cmd_info},
  {"si","Execute one command every step",cmd_si},
  {"x","Scan the status of momery around the pointed address.",cmd_x},
  {"p","Compute the value of the formula after 'p'",cmd_p},
  {"w","Set a watchpoint",cmd_w},
  {"d","delete a watchpoint",cmd_d}
  /* TODO: Add more commands */

};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

void ui_mainloop(int is_batch_mode) {
  if (is_batch_mode) {
    cmd_c(NULL);
    return;
  }

  while (1) {
    char *str = rl_gets();
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */


    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }

#ifdef HAS_IOE
    extern void sdl_clear_event_queue(void);
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }
    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
	}
  
}
