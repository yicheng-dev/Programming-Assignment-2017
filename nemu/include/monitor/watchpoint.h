#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include "common.h"

typedef struct watchpoint {
  int NO;
  struct watchpoint *next;
  char expr[128];
  /* TODO: Add more members if necessary */


} WP;

void new_wp(char *args);
void free_wp(char *args);

#endif
