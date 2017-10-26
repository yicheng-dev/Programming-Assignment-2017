#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>

enum {
  TK_NOTYPE = 256, TK_EQ, TK_NUM_D,TK_NUM_H,TK_IEQ, TK_LE, TK_GE, TK_DERF, TK_NEG,TK_AND,TK_OR, TK_EAX, TK_ECX, TK_EDX, TK_EBX, TK_ESP, TK_EBP, TK_ESI, TK_EDI,TK_EIP
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
  {"\\+", '+'},         // plus
  {"&&",TK_AND},
  {"\\|\\|",TK_OR},
  {"==", TK_EQ},  // equal
  {"!=",TK_IEQ},
  {"<=",TK_LE},
  {">=",TK_GE},
  {"<",'<'},
  {">",'>'},
  {"!",'!'},
  {"-", '-'},		//minus
  {"\\*",'*'},     //multyply
  {"/",'/'},     //division
  {"0[xX][0-9a-fA-F]+",TK_NUM_H},
  {"[0-9]+",TK_NUM_D},   //number(dec)
  {"\\(",'('},
  {"\\)",')'},
  {"\\$[eE][aA][xX]",TK_EAX},
  {"\\$[eE][cC][xX]",TK_ECX},
  {"\\$[eE][dD][xX]",TK_EDX},
  {"\\$[eE][bB][xX]",TK_EBX},
  {"\\$[eE][sS][pP]",TK_ESP},
  {"\\$[eE][bB][pP]",TK_EBP},
  {"\\$[eE][sS][iI]",TK_ESI},
  {"\\$[eE][dD][iI]",TK_EDI},
  {"\\$[eE][iI][pP]",TK_EIP} 




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
  char str[32];
} Token;

Token tokens[32];
int nr_token;

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

      //  Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
        //    i, rules[i].regex, position, substr_len, substr_len, substr_start);
        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */
		if (rules[i].token_type == TK_NOTYPE) 
			break;
		tokens[nr_token++].type = rules[i].token_type;
        switch (rules[i].token_type) {	
			case TK_NUM_D:{
				int j = 0;
				for (; j < substr_len; j++) 
					tokens[nr_token-1].str[j] = substr_start[j];
				tokens[nr_token-1].str[j] = '\0';}				
				break;
			case TK_NUM_H:{
				int j = 0;
				for (; j < substr_len; j++) 
					tokens[nr_token-1].str[j] = substr_start[j];
				tokens[nr_token-1].str[j] = '\0';}				
			break;
						  // default: TODO();
        }

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
static bool is_reg (int i) {
	return tokens[i].type >= TK_EAX && tokens[i].type <= TK_EIP; 

}
bool check_parentheses(int p, int q){
	if (tokens[p].type != '('|| tokens[q].type != ')')
		return false;
	else {
		int stack = 0;
		for (int i = p + 1; i < q;i++) {
			if (tokens[i].type == ')'){
				if (stack <= 0) 
					return false;
				else stack--;
			}
			if (tokens[i].type == '('){
				stack++;
			}	
		}
		return stack == 0;
	}
}
uint32_t eval (int p,int q, bool *success) {
	if (q < p) {
	//	printf("Invalid expression!\n");
		*success = false;
	   return -1;	
	} else if (p == q) {
		uint32_t val = 0;
		if (tokens[p].type == TK_NUM_D) {
			sscanf(tokens[p].str,"%u",&val);
			return val;
	 	}
	 	if (tokens[p].type == TK_NUM_H) {
			sscanf(tokens[p].str,"%x",&val);
	//		puts("Hit");
			return val;
		}
		switch (tokens[p].type) {
			case TK_EAX:
				return reg_l(R_EAX);
			case TK_ECX:
				return reg_l(R_ECX);
			case TK_EDX:
				return reg_l(R_EDX);
			case TK_EBX:
				return reg_l(R_EBX);
			case TK_ESP:
				return reg_l(R_ESP);
			case TK_EBP:
				return reg_l(R_EBP);
			case TK_ESI:
				return reg_l(R_ESI);
			case TK_EDI:
				return reg_l(R_EDI);
			case TK_EIP:
				return cpu.eip;			
		
		
		
		}
		*success = false;
		return -1;
		
	}else if (check_parentheses(p,q)) {
		return eval(p+1,q-1,success); 
	}else {
		int OP;
		int stack = 0;
		bool flag = 0;
		for (int i = q;i >=p&&!flag;i-- ) {
			if (tokens[i].type == ')'){
				stack++;
				continue;
			}	
			if (tokens[i].type == '('){
				stack--;
				continue;
			}
			if (stack == 0 &&tokens[i].type == TK_OR) {
				OP = i;
				flag = 1;
				break;
			}
		
		}
		stack = 0;
		for (int i = q;i >=p&&!flag;i-- ) {
			if (tokens[i].type == ')'){
				stack++;
				continue;
			}	
			if (tokens[i].type == '('){
				stack--;
				continue;
			}
			if (stack == 0 &&tokens[i].type == TK_AND) {
				OP = i;
				flag = 1;
				break;
			}
		
		}
		stack = 0;
		for (int i = q;i >=p&&!flag;i-- ) {
			if (tokens[i].type == ')'){
				stack++;
				continue;
			}	
			if (tokens[i].type == '('){
				stack--;
				continue;
			}
			if (stack == 0 &&(tokens[i].type == TK_EQ || tokens[i].type == TK_GE || 
						tokens[i].type == TK_IEQ || tokens[i].type == TK_LE ||
						tokens[i].type == '<' || tokens[i].type == '>')) {
				OP = i;
				flag = 1;
				break;
			}
		
		}
		stack = 0;
		for (int i = q;i >=p&&!flag;i-- ) {
			if (tokens[i].type == ')'){
				stack++;
				continue;
			}	
			if (tokens[i].type == '('){
				stack--;
				continue;
			}
			if (stack == 0 &&(tokens[i].type == '+' || tokens[i].type == '-')) {
				OP = i;
				flag = 1;
				break;
			}
		
		}
		stack = 0;
		for (int i = q;i >=p&&!flag;i-- ) {
			if (tokens[i].type == ')'){
				stack++;
				continue;
			}	
			if (tokens[i].type == '('){
				stack--;
				continue;
			}
			if (stack == 0 &&(tokens[i].type == '*' || tokens[i].type == '/')) {
				OP = i;
				flag = 1;
				break;
			}
		
		}
		stack = 0;
		for (int i = p;i <=q&&!flag;i++ ) {
			if (tokens[i].type == ')'){
				stack--;
				continue;
			}	
			if (tokens[i].type == '('){
				stack++;
				continue;
			}
			if (stack == 0 &&(tokens[i].type == TK_NEG || tokens[i].type == TK_DERF|| tokens[i].type == '!')) {
				OP = i;
				flag = 1;
				break;
			}
		
		}
		if (flag) {
			switch (tokens[OP].type) {
				case '+':
					return eval(p,OP-1,success) + eval(OP+1,q,success);
				
				case '-':
					return eval(p,OP-1,success) -  eval(OP+1,q,success);	
			
				case '*':
					return eval(p,OP-1,success) * eval(OP+1,q,success);
				case '/':
					return eval(p,OP-1,success) / eval(OP+1,q,success);
				case TK_EQ:
					return eval(p,OP-1,success) == eval(OP+1,q,success);
				case TK_IEQ:
					return eval(p,OP-1,success) != eval(OP+1,q,success);
				case TK_LE:
					return eval(p,OP-1,success) <= eval(OP+1,q,success);
				case TK_GE:
					return eval(p,OP-1,success) >= eval(OP+1,q,success);
				case '>':
					return eval(p,OP-1,success) > eval(OP+1,q,success);
				case '<':
					return eval(p,OP-1,success) < eval(OP+1,q,success);
				case '!':
					return !eval(OP+1,q,success);
				case TK_NEG:
					return -eval(OP+1,q,success);
				case TK_DERF:
					
						
						return vaddr_read(eval(OP+1,q,success),4);
				case TK_AND :
							
					return eval(p,OP-1,success) && eval(OP+1,q,success);
				case TK_OR:

					return eval(p,OP-1,success) || eval(OP+1,q,success);
					





				default:
					return -1;
			}		
		}else {
			*success = false;
			return -1;
		
		
		}


	
	} 
	


}
uint32_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }
  for (int i = 0; i < nr_token;i++) {
	if ((tokens[i].type == '-' || tokens[i].type == '*') && (i == 0 || (tokens[i-1].type != ')' && tokens[i-1].type != TK_NUM_D && tokens[i-1].type != TK_NUM_H && !is_reg(i-1)))){
		tokens[i].type = tokens[i].type == '-' ? TK_NEG:TK_DERF;		
	}
  
  }
  
  /* TODO: Insert codes to evaluate the expression. */

  return eval(0,nr_token-1,success);
}
