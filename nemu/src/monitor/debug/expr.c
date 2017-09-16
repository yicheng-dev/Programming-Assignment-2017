#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>
#include <string.h>
#include <stdlib.h>


enum {
  TK_NOTYPE = 256,TK_LBRAC,TK_RBRAC,TK_NUM,TK_HEXNUM,TK_REG_1,TK_REG_2,TK_REG_3,TK_REGSIG,TK_DEREF,TK_NEGSIG,TK_NOT,TK_MULTI,TK_DIVIDE,TK_MOD,
  TK_PLUS,TK_SUB,TK_EQ,TK_NEQ,TK_AND,TK_OR,TK_GE,TK_LE,TK_GREATER,
  TK_LESS

  /* TODO: Add more token types */

};

static struct rule {
  char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */
  {" +", TK_NOTYPE},    // spaces
  {"\\(",TK_LBRAC},			//left_bracket
  {"\\)",TK_RBRAC},			//right_bracket
  {"[1-9][0-9]*",TK_NUM},		// num
  {"0[Xx][0-9a-fA-F]+",TK_HEXNUM},	//hex_num
  {"\\$e[a-d]+x",TK_REG_1},  //start of register
  {"\\$e[sd]+i",TK_REG_2},
  {"\\$e[sb]+p",TK_REG_3},
  {"![^.]+",TK_NOT},		//logically not
  {"\\*", TK_MULTI},         // multiply
  {"/", TK_DIVIDE},          // divide
  {"%",TK_MOD},				//mod
  {"\\+", TK_PLUS},         // plus
  {"\\-", TK_SUB},			// subtract
  {"==", TK_EQ},         // equal
  {"!=",TK_NEQ},		// not equal
  {"&&",TK_AND},		//and
  {"\\|\\|",TK_OR},			//or
  {">=",TK_GE},			//greater or equal
  {"<=",TK_LE},			//less or equal
  {">",TK_GREATER},		//greater
  {"<",TK_LESS}			//less

};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX];

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char str[128];
  int str_len;
} Token;

Token tokens[128];
int nr_token;

static bool make_token(char *e) {
  int position = 0;
  int i,index;
  regmatch_t pmatch;
  nr_token = 0;

  
  while (e[position] != '\0') {
    /* Try all rules one by one. */
	  bool discard_space=false;
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);
        position += substr_len;
		
        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */
		Token new_token;
        switch (rules[i].token_type) {
			case TK_NOTYPE: discard_space=true;
							break;
			case TK_PLUS:	new_token.type=rules[i].token_type;
							break;
			case TK_NUM:	new_token.str_len=substr_len;
							new_token.type=rules[i].token_type;
							for (index=0;index<substr_len;index++)
								new_token.str[index]=substr_start[index];
							break;
			case TK_HEXNUM: new_token.str_len=substr_len;
							new_token.type=rules[i].token_type;
							for (index=0;index<substr_len;index++)
								new_token.str[index]=substr_start[index];
							break;
			case TK_SUB:	new_token.type=rules[i].token_type;
							break;
			case TK_MOD:	new_token.type=rules[i].token_type;
							break;
			case TK_DIVIDE:	new_token.type=rules[i].token_type;
							break;
			case TK_MULTI:	new_token.type=rules[i].token_type;
							break;
			case TK_NOT:	new_token.type=rules[i].token_type;
							break;
			case TK_LBRAC:	new_token.type=rules[i].token_type;
							break;
			case TK_RBRAC:	new_token.type=rules[i].token_type;
							break;
			case TK_EQ:		new_token.type=rules[i].token_type;
							break;
			case TK_NEQ:	new_token.type=rules[i].token_type;
							break;
			case TK_AND:	new_token.type=rules[i].token_type;
							break;
			case TK_OR:		new_token.type=rules[i].token_type;
							break;
			case TK_GE:		new_token.type=rules[i].token_type;
							break;
			case TK_LE:		new_token.type=rules[i].token_type;
							break;
			case TK_GREATER:new_token.type=rules[i].token_type;
							break;
			case TK_LESS:	new_token.type=rules[i].token_type;
							break;
			case TK_REGSIG:	new_token.type=rules[i].token_type;
							break;
			case TK_REG_1:	new_token.type=rules[i].token_type;
							new_token.str_len=substr_len;
							for (int index=0;index<substr_len;index++)
								new_token.str[index]=substr_start[index];
							break;
			case TK_REG_2:  new_token.type=rules[i].token_type;
							new_token.str_len=substr_len;
							for (int index=0;index<substr_len;index++)
								new_token.str[index]=substr_start[index];
							break;
			case TK_REG_3:  new_token.type=rules[i].token_type;
							new_token.str_len=substr_len;
							for (int index=0;index<substr_len;index++)
								new_token.str[index]=substr_start[index];
							break;

			default: TODO();
        }
		if (discard_space)
			break;
		tokens[nr_token++]=new_token;
		break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  return true;
}

bool bad_expression=false;

bool check_parentheses(int p,int q)
{
	int t;
	if (q-p==1 && tokens[p].type==TK_LBRAC && tokens[q].type==TK_RBRAC)
	{
		printf("bad:1\n");
		bad_expression=true;
		return false;
	}
	int left=0;
	int right=0;

	bool another_pair=false;

	for (t=p;t<=q;t++){

		if (left>0 && left==right) another_pair=true;
		if (left<right){
			printf("bad:2\n");
			bad_expression=true;
			return false;
		}
		if (tokens[t].type==TK_LBRAC)
			left++;
		if (tokens[t].type==TK_RBRAC)
			right++;
	}
/*
	printf("p:%d\n",p);
	printf("q:%d\n",q);
	printf("left:%d\n",left);
	printf("right:%d\n",right);
*/	
	if (left!=right){
		printf("bad:3\n");
		bad_expression=true;
		return false;
	}

	if (tokens[p].type!=TK_LBRAC || tokens[q].type!=TK_RBRAC)
		return false;
	if (another_pair) return false;
	return true;
}

int dominant(int p,int q)
{
	int t;
	int bra_num=0;

	for (t=q;t>=p;t--){
		if (tokens[t].type==TK_RBRAC) bra_num++;
		if (tokens[t].type==TK_LBRAC) bra_num--;
		if (bra_num==0 && (tokens[t].type==TK_AND || tokens[t].type==TK_OR))
			return t;
	}


	if (t==p-1){
		for (t=q;t>=p;t--){
			if (tokens[t].type==TK_RBRAC) bra_num++;
			if (tokens[t].type==TK_LBRAC) bra_num--;
			if (bra_num==0 && (tokens[t].type==TK_EQ || tokens[t].type==TK_NEQ))
				return t;
		}
	}
	if (t==p-1){
		for (t=q;t>=p;t--){
			if (tokens[t].type==TK_RBRAC) bra_num++;
			if (tokens[t].type==TK_LBRAC) bra_num--;
			if (bra_num==0 && (tokens[t].type==TK_GE || tokens[t].type==TK_LE ||
						tokens[t].type==TK_GREATER || tokens[t].type==TK_LESS))
				return t;
		}
	}

	if (t==p-1){
		for (t=q;t>=p;t--){
			if (tokens[t].type==TK_RBRAC) bra_num++;
			if (tokens[t].type==TK_LBRAC) bra_num--;
			if (bra_num==0 && (tokens[t].type==TK_PLUS || tokens[t].type==TK_SUB))
				return t;
		}
	}
	if (t==p-1)
	{
		for (t=q;t>=p;t--){
			if (tokens[t].type==TK_RBRAC) bra_num++;
			if (tokens[t].type==TK_LBRAC) bra_num--;
			if (bra_num==0 && (tokens[t].type==TK_MULTI || tokens[t].type==TK_DIVIDE || tokens[t].type==TK_MOD))
				return t;
		}
	}
	if (t==p-1 && (tokens[t+1].type == TK_DEREF || tokens[t+1].type == TK_NEGSIG || tokens[t+1].type == TK_NOT))
		return p;
	printf("bad:4\n");
	bad_expression=true;
	return 0;
}

int eval(int p,int q){
	if (bad_expression) return 0;
	if (p>q){
		printf("bad:5\n");
		bad_expression=true;
		return 0;
	}
	else if (p==q || q==p+1){
		if (tokens[q].type==TK_NUM){
			int ret=0,add=1,i;
			for (i=tokens[q].str_len-1;i>=0;i--){
				ret+=add*(tokens[q].str[i]-'0');
				add*=10;
			}
			if (q==p+1 && tokens[p].type==TK_NEGSIG)
				ret=-ret;
			else if (q==p+1 && tokens[p].type == TK_DEREF)
			{
				int address;
				sscanf(tokens[q].str, "%x", &address);
				ret = vaddr_read(address,8);
			}


	 	//	printf("sub_ans:%d\n",ret);
			return ret;
		}
		else if (tokens[q].type == TK_HEXNUM){
			int ret=0,add=1,i;
			for (i=tokens[q].str_len-1;i>=2;i--){
				if (tokens[q].str[i]>='0' && tokens[q].str[i]<='9')
					ret+=add*(tokens[q].str[i]-'0');
				else if (tokens[q].str[i]>='a' && tokens[q].str[i]<='f')
					ret+=add*(tokens[q].str[i]-'a'+10);
				else if (tokens[q].str[i]>='A' && tokens[q].str[i]<='F')
					ret+=add*(tokens[q].str[i]-'A'+10);
				add*=16;
			}
			if (q==p+1 && tokens[p].type==TK_NEGSIG)
				ret=-ret;
			else if (q==p+1 && tokens[p].type == TK_DEREF)
			{
				int address;
				sscanf(tokens[q].str, "%x", &address);
				ret = vaddr_read(address,8);
			}

			return ret;
		}
		else if (tokens[q].type == TK_REG_1 || tokens[q].type == TK_REG_2 || tokens[q].type == TK_REG_3)
		{
			int ret=0;
			if (tokens[q].type == TK_REG_1)
			{
				if (tokens[q].str[2]=='a')
					ret = cpu.eax;
				else if (tokens[q].str[2]=='b')
					ret = cpu.ebx;
				else if (tokens[q].str[2]=='c')
					ret = cpu.ecx;
				else if (tokens[q].str[2]=='d')
					ret = cpu.edx;
			}
			else if (tokens[q].type == TK_REG_2)
			{
				if (tokens[q].str[2]=='s')
					ret = cpu.esi;
				else if (tokens[q].str[2]=='d')
					ret = cpu.edi;
			}
			else 
			{
				if (tokens[q].str[2]=='s')
					ret = cpu.esp;
				else if (tokens[q].str[2]=='b')
					ret = cpu.ebp;
			}
			return ret;
		}
		else{
			printf("bad:6\n");
			bad_expression=true;
			return 0;
		}
	}
	else if (check_parentheses(p,q) == true){
		return eval(p+1,q-1);
	}
	else{
        if (bad_expression){
			return 0;
		}
		int op=dominant(p,q);
//		printf("op:%d\n",op);
		if (op-1>=p){
			int val1=eval(p,op-1);
			int val2=eval(op+1,q);
			switch (tokens[op].type){
				case TK_PLUS:return val1+val2;break;
				case TK_SUB:return val1-val2;break;
				case TK_MULTI:return val1*val2;break;
				case TK_DIVIDE:if (!(tokens[op+1].str_len==1 && tokens[op+1].str[0]=='0'))
									return val1/val2;
							   else {
						       printf("Error:Divide by 0!\n");
						       bad_expression=true;
						       return 0;
					           }
				 	           break;
				case TK_MOD:if (!(tokens[op+1].str_len==1 && tokens[op+1].str[0]=='0'))
							     return val1%val2;
					        else {
						         printf("Error:Divide by 0!\n");
						         bad_expression=true;
						         return 0;
				            }
					        break;
				case TK_EQ:return val1==val2;break;
				case TK_NEQ:return val1!=val2;break;
				case TK_AND:return val1&&val2;break;
				case TK_OR:return val1||val2;break;
				case TK_GE:return val1>=val2;break;
				case TK_LE:return val1<=val2;break;
				case TK_GREATER:return val1>val2;break;
				case TK_LESS:return val1<val2;break;
				default:assert(0);
			}
		}
		else {
			if (tokens[op].type == TK_DEREF){
				int address=eval(op+1,q);
				sscanf(tokens[q].str, "%x", &address);
				int ret = vaddr_read(address,8);
				return ret;
			}
			else if (tokens[op].type == TK_NEGSIG){
				int ret = 0 - eval(op+1,q);
				return ret;
			}
			else if (tokens[op].type == TK_NOT){
				int ret = !eval(op+1,q);
				return ret;
			}
		}
	}
	return 0;
}

uint32_t expr(char *e, bool *success) {

  if (!make_token(e)) {
    *success = false;
    return 0;
  }
  
  int i;
  for (i=0;i<nr_token;i++){
	  if (tokens[i].type==TK_MULTI && (i==0 || (tokens[i-1].type!=TK_HEXNUM && tokens[i-1].type != TK_NUM && tokens[i-1].type != TK_RBRAC)))
		  tokens[i].type = TK_DEREF;
  }

  for (i=0;i<nr_token;i++){
	  if (tokens[i].type==TK_SUB && (i==0 || (tokens[i-1].type!=TK_HEXNUM && tokens[i-1].type != TK_NUM && tokens[i-1].type != TK_RBRAC)))
		  tokens[i].type=TK_NEGSIG;
  }
/*
  int i,j;
  for (i=0;i<nr_token;i++)
  {
	  printf("type:%d\n",tokens[i].type);
	  if (tokens[i].type==TK_NUM)
	  {
		  printf("str: ");
		  for (j=0;j<tokens[i].str_len;j++)
			  printf("%c",tokens[i].str[j]);
		  printf("\n");
	  }
  }
  */
  uint32_t ans=eval(0,nr_token-1);
  if (bad_expression==false){
     printf("%u\n",ans);
  }
  else printf("Bad expression!\n");
  /* TODO: Insert codes to evaluate the expression. */
//  TODO();

  return 0;
}
