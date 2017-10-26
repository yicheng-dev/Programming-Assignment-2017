#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include "common.h"
#define MAX_EXPR_LEN 40
typedef struct watchpoint {
  int NO;
  struct watchpoint *next;
  char expr[MAX_EXPR_LEN];
  unsigned int last_val;
  /* TODO: Add more members if necessary */


} WP;
#endif
