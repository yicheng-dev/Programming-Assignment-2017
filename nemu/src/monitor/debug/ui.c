#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
WP* new_wp();
void cpu_exec(uint64_t);
void delete_wp(int);
void print_wp();
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
static int cmd_d(char *args) {
	delete_wp(atoi(args));
	return 0;

}
static int cmd_help(char *args);
static int cmd_si (char *args) {
	if (args == NULL)
		cpu_exec(1);
	else cpu_exec(atoi(args));
	return 0;
}
static int cmd_info (char *args){
	if (*args == 'r') {
		printf ("EAX  %08x\nEDX  %08x\nECX  %08x\nEBX  %08x\nEBP  %08x\nESI  %08x\nEDI  %08x\nESP  %08x\nEIP  %08x\n",cpu.eax,cpu.edx,cpu.ecx,cpu.ebx,cpu.ebp,cpu.esi,cpu.edi,cpu.esp,cpu.eip);
		printf ("CF:%d OF:%d ZF:%d SF:%d\n",cpu.eflags.CF,cpu.eflags.OF,cpu.eflags.ZF,cpu.eflags.SF);
	}
	if (*args == 'w') {
		print_wp();
	}
	return 0;
}
static int cmd_x (char *args);
static int cmd_w (char *args);
static int cmd_p (char *args){
	bool success = true;
	uint32_t ans = expr(args,&success);
	if (success) {
		printf("%x\n",ans);
	}else {
		puts("Invalid expression");
	}
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
  { "si", "Continue the execution of the program by step",cmd_si},
  { "info", "Print the information of the registers or the watchpoints",cmd_info},
  { "x", "Exam the memory of a given address",cmd_x},
  { "w", "Set watchpoint",cmd_w},
  {"d","Delete a watchpoint",cmd_d},
  {"p","Print the expression",cmd_p}
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
static int cmd_x(char *args) {
	char* buf = strtok(args," ");
	bool success = true;
		args = args+strlen(buf)+1;
		u_int len, addr;
		sscanf(buf,"%d",&len);
		addr = expr(args,&success);
		if (success){	
		for (int i = 0 ; i < len; i++) {
			u_int mem = vaddr_read(addr+i*4,4);
			printf("%08x:    %02x %02x %02x %02x\n",addr+i*4,mem&0xFF,(mem>>8)&0xFF,(mem>>16)&0xFF,(mem>>24)&0xFF);
		}	  }
		else printf("Invalid expression\n");
	return 0;
}
static int cmd_w (char *args) {
	bool success = true;
	uint32_t temv = expr(args,&success);
	if (!success) {
		puts("Invalid expression");
		return -1;
	}
	WP* tem = new_wp();
	strcpy(tem->expr,args);
	tem->last_val = temv;
	printf("New watchpoint NO:%d   expr:%s\n",tem->NO,args);
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
