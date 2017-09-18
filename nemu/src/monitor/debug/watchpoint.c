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
	printf("New watchpoint %d:%s\n",wp_new->NO,wp_new->expr);
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
		printf("Delete watchpoint %d:%s\n",head->NO,head->expr);
		WP * tmp=head;
		head=head->next;
		tmp->next=free_;
		free_=tmp;
		return;
	}
	while (p!=NULL){
		if (p->NO==num){
			WP * tmp=p;
			printf("Delete watchpoint %d:%s\n",p->NO,p->expr);
			p=p->next;
			tmp->next=free_;
			free_=tmp;
			return;
		}
		p=p->next;
	}
	printf("There is no watchpoint you want to delete!\n");
	return;
}

void init_expr_val()
{
	WP * p=head;
	while (p!=NULL){
		bool success=true;
		p->expr_val=expr(p->expr,&success);
		p=p->next;
	}
}

bool check_expr_val()
{
	WP *p=head;
	while (p!=NULL){
		bool success=true;
		uint32_t tmp=expr(p->expr,&success);
		if (tmp!=p->expr_val)
			return false;
		p=p->next;
	}
	return true;
}

void print_watchpoint()
{
	WP * p=head;
	if (p==NULL){
		printf("There is no active watchpoint!\n");
		return;
	}
	while (p!=NULL){
		printf("Hardware watchpoint %d:%s\n",p->NO,p->expr);
		p=p->next;
	}
}

