#include "monitor/watchpoint.h"
#include "monitor/expr.h"
#include <string.h>
#include <stdlib.h>
#define NR_WP 32

static WP wp_pool[NR_WP];
static WP *head, *free_;

WP* get_wp(char *args);


void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = &wp_pool[i + 1];
  }
  wp_pool[NR_WP - 1].next = NULL;

  head = NULL;
  free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */

void new_wp(char *args)
{
	if (free_==NULL)
		assert(0);
	WP* wp_new=get_wp(args); 
	wp_new->next=head;
	head=wp_new;
}

WP* get_wp(char *args)
{
	WP * p = free_;
	p->NO=free_->NO;
	strcpy(p->expr,args);
	free_=free_->next;
	return p;
		
}

bool isvalid(char *args)
{
	int size=strlen(args);
	int i;
	for (i=0;i<size;i++)
		if (args[i]<'0' || args[i]> '9')
			return false;
	return true;
}

void free_wp(char *args)
{
	if (!isvalid(args)){
		printf("Please enter command like d [N], and N indicates the No of watchpoint.\n");
		return;
	}
	int num=atoi(args);
	WP * p = head;
	if (head == NULL){
		printf("Now there's no active watchpoint!\n");
		return;
	}
	if (head->NO==num){
		head->next=free_;
		free_=head;
		head=NULL;
		return;
	}
	while (p->next!=NULL){
		if (p->next->NO==num){
			p->next->next=free_;
			free_ = p->next;
			p->next = p->next->next;
			return;
		}
		p=p->next;
	}
	printf("There is no watchpoint you want to delete!\n");
	return;
}

