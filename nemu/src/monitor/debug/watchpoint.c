#include "monitor/watchpoint.h"
#include "monitor/expr.h"
#define NR_WP 32

static WP wp_pool[NR_WP];
static WP *head, *free_;

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
WP* new_wp(){
	
	assert(free_!= NULL);
	WP *tem = free_;
	free_ = free_ -> next;
	tem -> next = head;
	head = tem;
	return head;
}
void free_wp(WP *wp) {
	WP *tem = head;
	if (tem == wp) {
		head = tem->next;
		wp->next = free_;
		free_ = wp;
		return;

	
	}
	while (tem != NULL) {
		if (tem -> next == wp)
			break;
		tem = tem -> next;
	}	
	assert(tem != NULL);
	tem -> next = wp->next;
	wp->next = free_;
	free_ = wp;
}
int check_wp () {
	int w = -1;
	WP* tem = head;
	while (tem != NULL) {
		bool success = 1;
		unsigned int t = expr(tem->expr,&success);
		assert(success);
		if (t != tem->last_val){
			printf("Triggered watchpoint NO:%d    expr:%s     old value:%x  new value:%x\n",tem->NO,
					tem->expr,tem->last_val,t);
			tem->last_val = t;
			w = tem->NO;
		}
		tem = tem->next;
	
	}
	return w;


}
void delete_wp(int NO) {
	free_wp(&wp_pool[NO]);
}
/* TODO: Implement the functionality of watchpoint */
void print_wp(){
	WP* tem = head;
	while (tem!=NULL) {
		printf ("NO:%d       expr:%s\n     current value:%x\n",tem->NO,tem->expr,tem->last_val);
		tem = tem->next;
	}
	



}
void print_wp_no(int NO) {
	printf("NO:%d      expr:%s\n",NO,wp_pool[NO].expr);
}

