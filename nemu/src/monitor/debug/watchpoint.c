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
	
	bool success=true;
	bool *success_ptr=&success;
	uint32_t tmp=expr(args,success_ptr);
	if (*success_ptr==false){
		printf("Please enter a valid expression.\n");
		return;
	}
	WP* wp_new=get_wp(args);
	wp_new->expr_val=tmp;
	wp_new->next=head;
	head=wp_new;
	printf("New watchpoint %d:%s\n",wp_new->NO,wp_new->expr);
}


WP* get_wp(char *args)
{
	WP * p = free_;
	strcpy(p->expr,args);
	p->NO=free_->NO;
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
	while (p->next!=NULL){
		if (p->next->NO==num){
			WP * tmp=p->next;
			printf("Delete watchpoint %d:%s\n",p->next->NO,p->next->expr);
			p->next=p->next->next;
			tmp->next=free_;
			free_=tmp;
			return;
		}
		p=p->next;
	}
	printf("There is no watchpoint you want to delete!\n");
	return;
}


bool check_expr_val()
{
	bool flag=true;
	WP *p=head;
	while (p!=NULL){
		bool success=true;
		bool *success_ptr=&success;
		uint32_t tmp=expr(p->expr,success_ptr);
//		printf("%d\n",tmp);
		if (tmp!=p->expr_val){
			printf("Watchpoint %d is triggered!\n",p->NO);
			flag=false;
			p->expr_val=tmp;
		}
		p=p->next;
	}
	return flag;
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

