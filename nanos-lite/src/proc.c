#include "proc.h"

#define MAX_NR_PROC 4

static PCB pcb[MAX_NR_PROC];
static int nr_proc = 0;
PCB *current = NULL;

uintptr_t loader(_Protect *as, const char *filename);

void load_prog(const char *filename) {
  int i = nr_proc ++;
  _protect(&pcb[i].as);
//  printf("reach");
  uintptr_t entry = loader(&pcb[i].as, filename);

  // TODO: remove the following three lines after you have implemented _umake()
/*  _switch(&pcb[i].as);
  current = &pcb[i];
  ((void (*)(void))entry)();
*/
  _Area stack;
  stack.start = pcb[i].stack;
  stack.end = stack.start + sizeof(pcb[i].stack);

  pcb[i].tf = _umake(&pcb[i].as, stack, stack, (void *)entry, NULL, NULL);
}


_RegSet* schedule(_RegSet *prev) {
  current->tf = prev;
  static int process_cnt = 0;
  static bool first_process = true;
  if (first_process) {
	current = &pcb[0];
	first_process = false;
  }
  else if (process_cnt >= 2) {
	current = (current == &pcb[0] ? &pcb[1] : &pcb[0]);
	process_cnt = 0;
  }
  process_cnt++;
  _switch(&current->as);
  return current->tf;
}
