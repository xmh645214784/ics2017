#include "nemu.h"
#include "common.h"
#include "monitor/expr.h"
#include <stdlib.h>

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>

enum {
  
  RULE_ASSIGN=0,
  RULE_OR,
  RULE_AND,
  RULE_BIT_OR,
  RULE_BIT_XOR,
  RULE_BIT_AND,
  RULE_EQ, RULE_NE, 
  RULE_GT, RULE_LT, RULE_GE, RULE_LE,
  RULE_SHIFT_L, RULE_SHIFT_R,
  RULE_ADD, RULE_SUB,
  RULE_MUL, RULE_DIV, RULE_REM,
  RULE_NOT, RULE_NEG, RULE_DER, RULE_BIT_NOT,
  RULE_BRA_L, RULE_BRA_R,
  
  RULE_NOTYPE=256, RULE_DIGIT, RULE_HEX, RULE_ALPHA, RULE_REG
  
  /* done */
  /* TODO: Add more token types */
  
};

static struct precedence {
  int rule;
  int pre;
} rule_pre[] = {
  {RULE_ASSIGN,10},
  {RULE_OR,20},
  {RULE_AND,30},
  {RULE_BIT_OR, 40},
  {RULE_BIT_XOR, 50},
  {RULE_BIT_AND, 60},
  {RULE_EQ,70}, {RULE_NE,70},
  {RULE_GT,80}, {RULE_LT, 80}, {RULE_GE,80}, {RULE_LE, 80},
  {RULE_SHIFT_L, 90}, {RULE_SHIFT_R,90},
  {RULE_ADD,100}, {RULE_SUB,100},
  {RULE_MUL,110}, {RULE_DIV,110}, {RULE_REM, 110},
  {RULE_NOT,120}, {RULE_NEG, 120}, {RULE_DER, 120}, {RULE_BIT_NOT, 120},
  {RULE_BRA_L,130},{RULE_BRA_R,130}
};

static struct rule {
  char *regex;
  int token_type;
} rules[] = {
  
  /* done */
  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */
  
  {"^((0x)|(0X))[0-9a-fA-F]+", RULE_HEX},
  {"[0-9]+", RULE_DIGIT},
  {"^[a-zA-Z_][a-zA-Z0-9_]*", RULE_ALPHA},
  {"\\$(eax|ecx|edx|ebx|esp|ebp|esi|edi|eip)", RULE_REG},
  {" +", RULE_NOTYPE},
  {"\\+", RULE_ADD},
  {"-", RULE_SUB},
  {"\\*", RULE_MUL},
  {"/", RULE_DIV},
  {"%",RULE_REM},
  {"\\(", RULE_BRA_L},
  {"\\)", RULE_BRA_R},
  {"!=", RULE_NE},
  {"==", RULE_EQ},
  {">=",RULE_GE},
  {"<=",RULE_LE},
  {"<<",RULE_SHIFT_L},
  {">>",RULE_SHIFT_R},
  {">", RULE_GT},
  {"<",RULE_LT},
  {"=", RULE_ASSIGN},
  {"\\|\\|", RULE_OR},
  {"&&", RULE_AND},
  {"\\|", RULE_BIT_OR},
  {"&",RULE_BIT_AND},
  {"\\^",RULE_BIT_XOR},
  {"~",RULE_BIT_NOT},
  {"\\!", RULE_NOT},
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
  
  for(i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if(ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      Assert(ret == 0, "regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

Token tokens[32];
int nr_token;

typedef struct variables {
  char str[32];
  int key;
} variables;

#define VAR_MAX 32
static variables var[VAR_MAX];
static int var_cnt=0;

static int find_var(char *str) {
  int i=0;
  for (;i < var_cnt; ++i) {
    if (strcmp(var[i].str, str)==0) {
      return i;
    }
  }
  return -1;
}

static int set_var(char *str, int value) {
  int i=0;
  if (var_cnt==VAR_MAX) {
    printf("Variables reached limits (%d), please clear!\n", VAR_MAX);
    return -1;
  }
  for (;i < var_cnt; ++i) {
    if (strcmp(var[i].str, str)==0) {
      var[i].key=value;
      return i;
    }
  }
  strcpy(var[var_cnt].str,str);
  var[var_cnt].key=value;
  ++var_cnt;
  return var_cnt-1;
}

static int clear_var() {
  var_cnt=0;
  printf("Ok!\n");
  return 0;
}

static char *cpu_name_rule[]={"$eax", "$ecx", "$edx", "$ebx", "$esp", "$ebp", "$esi", "$edi", "$eip"};

static bool print_err(char *str, int n) {
  fprintf(stderr, "%s\n\033[32m%*c\033[0m\n", str, n+1, '^');
  return false;
}


bool make_token(char *e, bool *is_match, int prompt) {
  *is_match=true;
  int position = 0;
  int i;
  regmatch_t pmatch;
  int left=0,right=0;
  nr_token = 0;
  int start_alpha=0;
  while(e[position] != '\0') {
    /* Try all rules one by one. */
    for(i = 0; i < NR_REGEX; i ++) {
      if(regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
  char *substr_start = e + position;
  int substr_len = pmatch.rm_eo;
  
//  Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s", i, rules[i].regex, position, substr_len, substr_len, substr_start);
  
  
  /* done */
  /* TODO: Now a new token is recognized with rules[i]. Add codes
   * to record the token in the array `tokens'. For certain types
   * of tokens, some extra actions should be performed.
   */
  int is_neg_or_der=0;
  int t;
  if (start_alpha&&nr_token==1&&rules[i].token_type!=RULE_ASSIGN&&rules[i].token_type!=RULE_NOTYPE) {
    return print_err(e, position);
  }
  if (substr_len>31) {
    fprintf(stderr, "The string is too long!\n");
    return print_err(e, position);
  }
  if (nr_token>31) {
    fprintf(stderr, "Too many terms!\n");
    return false;
  }
  switch(rules[i].token_type) {
    case RULE_DIGIT:
      if (nr_token!=0) {
        t=tokens[nr_token-1].type;
        if (t==RULE_NEG) {
    --nr_token;
    is_neg_or_der=1;
        }
        else if (t==RULE_DER){
    --nr_token;
    is_neg_or_der=2;
        }
        else if (t==RULE_DIGIT || t==RULE_HEX || t==RULE_ALPHA || t==RULE_BRA_R || t==RULE_REG) {
    return print_err(e, position);
        }
      }
      tokens[nr_token].type=RULE_DIGIT;
      strncpy(tokens[nr_token].str,substr_start,substr_len);
      tokens[nr_token].str[substr_len]='\0';
      tokens[nr_token].value=strtol(tokens[nr_token].str,NULL,0);
      if (is_neg_or_der==1) {
        tokens[nr_token].value=-tokens[nr_token].value;
      }
      else if(is_neg_or_der==2){
        tokens[nr_token].value=(int)swaddr_read((swaddr_t)tokens[nr_token].value,4);
      }
      break;
    case RULE_HEX:
      if (nr_token!=0) {
        t=tokens[nr_token-1].type;
        if (t==RULE_NEG) {
    --nr_token;
    is_neg_or_der=1;
        }
        else if (t==RULE_DER){
    --nr_token;
    is_neg_or_der=2;
        }
        else if (t==RULE_DIGIT || t==RULE_HEX || t==RULE_ALPHA || t==RULE_BRA_R || t==RULE_REG) {
    return print_err(e, position);
        }
      }
      tokens[nr_token].type=RULE_HEX;
      strncpy(tokens[nr_token].str,substr_start,substr_len);
      tokens[nr_token].str[substr_len]='\0';
      tokens[nr_token].value=strtol(tokens[nr_token].str,NULL,0);
      if (is_neg_or_der==1) {
        tokens[nr_token].value=-tokens[nr_token].value;
      }
      else if(is_neg_or_der==2){
        tokens[nr_token].value=(int)swaddr_read((swaddr_t)tokens[nr_token].value,4);
      }
      break;
    case RULE_ALPHA: 
      if (nr_token!=0) {
        t=tokens[nr_token-1].type;
        if (t==RULE_NEG) {
    --nr_token;
    is_neg_or_der=1;
        }
        else if (t==RULE_DER) {
    --nr_token;
    is_neg_or_der=2;
        }
        else if (t==RULE_DIGIT || t==RULE_HEX || t==RULE_ALPHA || t==RULE_BRA_R || t==RULE_REG) {
    return print_err(e, position);
        }
      }
      tokens[nr_token].type=RULE_ALPHA;
      strncpy(tokens[nr_token].str,substr_start,substr_len);
      tokens[nr_token].str[substr_len]='\0';
      //printf("%s\n", tokens[nr_token].str);
      int tmp_var=find_var(tokens[nr_token].str);
      //printf("var: %d\n",tmp_var);
      //int is_find=find_var(tokens[nr_token].str);
      if (nr_token==0) {
        //tmp_var=set_var(tokens[nr_token].str,0);
        if (tmp_var==-1) {
    start_alpha=1;
        }
        else {
    tokens[nr_token].value=var[tmp_var].key;
        }
        /*if (tmp_var==-1) {
         *    return false;
      }*/
      }
      else if (tmp_var!=-1) {
        if (is_neg_or_der==0) {
    tokens[nr_token].value=var[tmp_var].key;
        }
        if (is_neg_or_der==1) {
    tokens[nr_token].value=-var[tmp_var].key;
        }
        else if(is_neg_or_der==2) {
    tokens[nr_token].value=(int)swaddr_read((swaddr_t)var[tmp_var].key,4);
        }
      }
      else {
        fprintf(stderr, "Cannot find the variable '%s'\n", tokens[nr_token].str);
        return false;
      }
      break;
    case RULE_REG:
      
      if (nr_token!=0) {
        t=tokens[nr_token-1].type;
        if (t==RULE_NEG) {
    --nr_token;
    is_neg_or_der=1;
        }
        else if (t==RULE_DER) {
    --nr_token;
    is_neg_or_der=2;
        }
        else if (t==RULE_DIGIT || t==RULE_HEX || t==RULE_ALPHA || t==RULE_BRA_R || t==RULE_REG) {
    return print_err(e, position);
        }
      }
      tokens[nr_token].type=RULE_REG;
      strncpy(tokens[nr_token].str,substr_start,substr_len);
      tokens[nr_token].str[substr_len]='\0';
      int gpr_cnt=0;
      for (;gpr_cnt<8;++gpr_cnt) {
        if (strcmp(tokens[nr_token].str,cpu_name_rule[gpr_cnt])==0) {
    tokens[nr_token].value=(int)cpu.gpr[gpr_cnt]._32;
    break;
        }
      }
      if (gpr_cnt==8) {
        tokens[nr_token].value=(int)cpu.eip;
      }
      if (is_neg_or_der==1) {
        tokens[nr_token].value=-tokens[nr_token].value;
      }
      else if(is_neg_or_der==2) {
        tokens[nr_token].value=(int)swaddr_read((swaddr_t)tokens[nr_token].value,4);
      }
      break;
    case RULE_NOTYPE:--nr_token;break;
    case RULE_ADD:
      tokens[nr_token].type=RULE_ADD;
      strcpy(tokens[nr_token].str, "+");
      if (nr_token==0) {
        return print_err(e, position);
      }
      else
      {
        t=tokens[nr_token-1].type;
        if (t!=RULE_DIGIT && t!=RULE_HEX && t!=RULE_ALPHA && t!=RULE_BRA_R && t!=RULE_REG) {
    return print_err(e, position);
        }
      }
      break;
    case RULE_SUB:
      tokens[nr_token].type=RULE_SUB;
      strcpy(tokens[nr_token].str, "-");
      if (nr_token!=0) {
        t=tokens[nr_token-1].type;
        if (t!=RULE_DIGIT && t!=RULE_HEX && t!=RULE_ALPHA && t!=RULE_REG && t!=RULE_BRA_R)
        {
    tokens[nr_token].type=RULE_NEG;
        }
      }
      else {
        tokens[nr_token].type=RULE_NEG;
      }
      if (tokens[nr_token].type!=RULE_NEG) {
        if (nr_token==0) {
    return print_err(e, position);
        }
        else
        {
    t=tokens[nr_token-1].type;
    if (t!=RULE_DIGIT && t!=RULE_HEX && t!=RULE_ALPHA && t!=RULE_BRA_R && t!=RULE_REG) {
      return print_err(e, position);
    }
        }
      }
      break;
    case RULE_MUL:
      tokens[nr_token].type=RULE_MUL;
      strcpy(tokens[nr_token].str, "*");
      if (nr_token!=0) {
        t=tokens[nr_token-1].type;
        if (t!=RULE_DIGIT && t!=RULE_HEX && t!=RULE_ALPHA && t!=RULE_REG && t!=RULE_BRA_R)
        {
    tokens[nr_token].type=RULE_DER;
        }
      }
      else {
        tokens[nr_token].type=RULE_DER;
      }
      if (tokens[nr_token].type!=RULE_DER) {
        if (nr_token==0) {
    return print_err(e, position);
        }
        else
        {
    t=tokens[nr_token-1].type;
    if (t!=RULE_DIGIT && t!=RULE_HEX && t!=RULE_ALPHA && t!=RULE_BRA_R && t!=RULE_REG) {
      return print_err(e, position);
    }
        }
      }
      break;
    case RULE_DIV:
      tokens[nr_token].type=RULE_DIV;
      strcpy(tokens[nr_token].str, "/");
      if (nr_token==0) {
        return print_err(e, position);
      }
      else
      {
        t=tokens[nr_token-1].type;
        if (t!=RULE_DIGIT && t!=RULE_HEX && t!=RULE_ALPHA && t!=RULE_BRA_R && t!=RULE_REG) {
    return print_err(e, position);
        }
      }
      break;
    case RULE_REM:
      tokens[nr_token].type=RULE_REM;
      strcpy(tokens[nr_token].str, "%");
      if (nr_token==0) {
        return print_err(e, position);
      }
      else
      {
        t=tokens[nr_token-1].type;
        if (t!=RULE_DIGIT && t!=RULE_HEX && t!=RULE_ALPHA && t!=RULE_BRA_R && t!=RULE_REG) {
    return print_err(e, position);
        }
      }
      break;
    case RULE_SHIFT_L:
      tokens[nr_token].type=RULE_SHIFT_L;
      strcpy(tokens[nr_token].str, "<<");
      if (nr_token==0) {
        return print_err(e, position);
      }
      else
      {
        t=tokens[nr_token-1].type;
        if (t!=RULE_DIGIT && t!=RULE_HEX && t!=RULE_ALPHA && t!=RULE_BRA_R && t!=RULE_REG) {
    return print_err(e, position);
        }
      }
      break;
    case RULE_SHIFT_R:
      tokens[nr_token].type=RULE_SHIFT_R;
      strcpy(tokens[nr_token].str, ">>");
      if (nr_token==0) {
        return print_err(e, position);
      }
      else
      {
        t=tokens[nr_token-1].type;
        if (t!=RULE_DIGIT && t!=RULE_HEX && t!=RULE_ALPHA && t!=RULE_BRA_R && t!=RULE_REG) {
    return print_err(e, position);
        }
      }
      break;
    case RULE_BRA_L:
      tokens[nr_token].type=RULE_BRA_L;
      strcpy(tokens[nr_token].str, "(");
      if (nr_token!=0)
      {
        t=tokens[nr_token-1].type;
        if (t==RULE_DIGIT || t==RULE_HEX || t==RULE_ALPHA || t==RULE_REG || t==RULE_BRA_R)
        {
    return print_err(e, position);
        }
      }
      ++left;
      break;
    case RULE_BRA_R:
      tokens[nr_token].type=RULE_BRA_R;
      strcpy(tokens[nr_token].str, ")");
      if (nr_token==0) {
        return print_err(e, position);
      }
      t=tokens[nr_token-1].type;
      if (t!=RULE_DIGIT && t!=RULE_HEX && t!=RULE_ALPHA && t!=RULE_REG && t!=RULE_BRA_R)
      {
        return print_err(e, position);
      }
      ++right;
      if (right>left) {
        return print_err(e, position);
      }
      break;
    case RULE_NE:
      tokens[nr_token].type=RULE_NE;
      strcpy(tokens[nr_token].str,"!=");
      if (nr_token==0) {
        return print_err(e, position);
      }
      else
      {
        t=tokens[nr_token-1].type;
        if (t!=RULE_DIGIT && t!=RULE_HEX && t!=RULE_ALPHA && t!=RULE_BRA_R && t!=RULE_REG) {
    return print_err(e, position);
        }
      }
      break;
    case RULE_EQ:
      tokens[nr_token].type=RULE_EQ;
      strcpy(tokens[nr_token].str,"==");
      if (nr_token==0) {
        return print_err(e, position);
      }
      else
      {
        t=tokens[nr_token-1].type;
        if (t!=RULE_DIGIT && t!=RULE_HEX && t!=RULE_ALPHA && t!=RULE_BRA_R && t!=RULE_REG) {
    return print_err(e, position);
        }
      }
      break;
    case RULE_GT:
      tokens[nr_token].type=RULE_GT;
      strcpy(tokens[nr_token].str,">");
      if (nr_token==0) {
        return print_err(e, position);
      }
      else
      {
        t=tokens[nr_token-1].type;
        if (t!=RULE_DIGIT && t!=RULE_HEX && t!=RULE_ALPHA && t!=RULE_BRA_R && t!=RULE_REG) {
    return print_err(e, position);
        }
      }
      break;
    case RULE_LT:
      tokens[nr_token].type=RULE_LT;
      strcpy(tokens[nr_token].str,"<");
      if (nr_token==0) {
        return print_err(e, position);
      }
      else
      {
        t=tokens[nr_token-1].type;
        if (t!=RULE_DIGIT && t!=RULE_HEX && t!=RULE_ALPHA && t!=RULE_BRA_R && t!=RULE_REG) {
    return print_err(e, position);
        }
      }
      break;
    case RULE_GE:
      tokens[nr_token].type=RULE_GE;
      strcpy(tokens[nr_token].str,">=");
      if (nr_token==0) {
        return print_err(e, position);
      }
      else
      {
        t=tokens[nr_token-1].type;
        if (t!=RULE_DIGIT && t!=RULE_HEX && t!=RULE_ALPHA && t!=RULE_BRA_R && t!=RULE_REG) {
    return print_err(e, position);
        }
      }
      break;
    case RULE_LE:
      tokens[nr_token].type=RULE_LE;
      strcpy(tokens[nr_token].str,"<=");
      if (nr_token==0) {
        return print_err(e, position);
      }
      else
      {
        t=tokens[nr_token-1].type;
        if (t!=RULE_DIGIT && t!=RULE_HEX && t!=RULE_ALPHA && t!=RULE_BRA_R && t!=RULE_REG) {
    return print_err(e, position);
        }
      }
      break;
    case RULE_ASSIGN:
      if (nr_token!=1 || tokens[0].type!=RULE_ALPHA) {
        return print_err(e, position);
      }
      tokens[nr_token].type=RULE_ASSIGN;
      strcpy(tokens[nr_token].str,"="); 
      if (nr_token!=1||tokens[0].type!=RULE_ALPHA) {
        return print_err(e, position);
      }
      break;
    case RULE_OR:
      tokens[nr_token].type=RULE_OR;
      strcpy(tokens[nr_token].str,"||");
      if (nr_token==0) {
        return print_err(e, position);
      }
      else
      {
        t=tokens[nr_token-1].type;
        if (t!=RULE_DIGIT && t!=RULE_HEX && t!=RULE_ALPHA && t!=RULE_BRA_R && t!=RULE_REG) {
    return print_err(e, position);
        }
      }
      break;
    case RULE_BIT_OR:
      tokens[nr_token].type=RULE_BIT_OR;
      strcpy(tokens[nr_token].str,"|");
      if (nr_token==0) {
        return print_err(e, position);
      }
      else
      {
        t=tokens[nr_token-1].type;
        if (t!=RULE_DIGIT && t!=RULE_HEX && t!=RULE_ALPHA && t!=RULE_BRA_R && t!=RULE_REG) {
    return print_err(e, position);
        }
      }
      break;
    case RULE_BIT_AND:
      tokens[nr_token].type=RULE_BIT_AND;
      strcpy(tokens[nr_token].str,"&");
      if (nr_token==0) {
        return print_err(e, position);
      }
      else
      {
        t=tokens[nr_token-1].type;
        if (t!=RULE_DIGIT && t!=RULE_HEX && t!=RULE_ALPHA && t!=RULE_BRA_R && t!=RULE_REG) {
    return print_err(e, position);
        }
      }
      break;
    case RULE_BIT_XOR:
      tokens[nr_token].type=RULE_BIT_XOR;
      strcpy(tokens[nr_token].str,"^");
      if (nr_token==0) {
        return print_err(e, position);
      }
      else
      {
        t=tokens[nr_token-1].type;
        if (t!=RULE_DIGIT && t!=RULE_HEX && t!=RULE_ALPHA && t!=RULE_BRA_R && t!=RULE_REG) {
    return print_err(e, position);
        }
      }
      break;
    case RULE_AND:
      tokens[nr_token].type=RULE_AND;
      strcpy(tokens[nr_token].str,"&&");
      if (nr_token==0) {
        return print_err(e, position);
      }
      else
      {
        t=tokens[nr_token-1].type;
        if (t!=RULE_DIGIT && t!=RULE_HEX && t!=RULE_ALPHA && t!=RULE_BRA_R && t!=RULE_REG) {
    return print_err(e, position);
        }
      }
      break;
    case RULE_NOT:
      tokens[nr_token].type=RULE_NOT;
      strcpy(tokens[nr_token].str, "!");
      if (nr_token!=0) {
        t=tokens[nr_token-1].type;
        if (t==RULE_DIGIT || t==RULE_HEX || t==RULE_ALPHA || t==RULE_REG || t==RULE_BRA_R)
        {
    return print_err(e, position);
        }
      }
      break;
    case RULE_BIT_NOT:
      tokens[nr_token].type=RULE_BIT_NOT;
      strcpy(tokens[nr_token].str, "~");
      if (nr_token!=0) {
        t=tokens[nr_token-1].type;
        if (t==RULE_DIGIT || t==RULE_HEX || t==RULE_ALPHA || t==RULE_REG || t==RULE_BRA_R)
        {
    return print_err(e, position);
        }
      }
      break;
  }
  position += substr_len;
  ++nr_token;
  break;
      }
    }
    if(i == NR_REGEX) {
        if (prompt)
        {
          printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
        }
      *is_match=false;
      return false;
    }
  }
  int t2;
  if (nr_token>0)
  {
    t2=tokens[nr_token-1].type;
    if (t2!=RULE_DIGIT && t2!=RULE_HEX && t2!=RULE_ALPHA && t2!=RULE_BRA_R && t2!=RULE_REG) {
      return print_err(e, position-1);
    }
  }
  if (left!=right) {
    fprintf(stderr, "Brackets do not match.\n");
    return false;
  }
  if (start_alpha==1 && nr_token==1) {
    *is_match=false;
    if (prompt) {
      fprintf(stderr, "Cannot find the variable '%s'\n", tokens[0].str);
      *is_match=true;
    }
    return false;
  }
  return true; 
}

static int find_digit(int p, int q, int op, int *pre, int *next) {
  int i;
  if (tokens[op].type<RULE_NOT) {
    for (i=op;i>=p;--i) {
      if (tokens[i].type>RULE_NOTYPE) {
  *pre=i;
  break;
      }
    }
    if (i<p) {
      return -1;
    }
  }
  for (i=op;i<=q;++i) {
    if (tokens[i].type>RULE_NOTYPE) {
      *next=i;
      break;
    }
  }
  if (i>q) {
    return -1;
  }
  return 0;
}

static int eval(int p, int q, bool *success) {
  if (p==q) {
    *success=true;
    return tokens[p].value;
  }
  int i=p;
  int op=0;
  for (i=p;i<=q;++i) {
    if (tokens[i].type<RULE_NOTYPE) {
      ++op;
    }
  }
  int j=0;
  for (;j<op;++j) {
    int max_pre=-1, max_pre_pos=-1;
    for (i=p;i<=q;++i) {
      if (tokens[i].type<RULE_NOTYPE&&rule_pre[tokens[i].type].pre>max_pre) {
  max_pre=rule_pre[tokens[i].type].pre;
  max_pre_pos=i;
      }
    }
    if (max_pre!=-1) {
      int pre=0,next=0;
      if (find_digit(p,q,max_pre_pos,&pre,&next)==0) {
  switch (tokens[max_pre_pos].type) {
    case RULE_AND:
      tokens[max_pre_pos].value=(tokens[pre].value&&tokens[next].value);
      break;
    case RULE_BIT_AND:
      tokens[max_pre_pos].value=(tokens[pre].value&tokens[next].value);
      break;
    case RULE_OR:
      tokens[max_pre_pos].value=(tokens[pre].value||tokens[next].value);
      break;
    case RULE_BIT_OR:
      tokens[max_pre_pos].value=(tokens[pre].value|tokens[next].value);
      break;
    case RULE_BIT_XOR:
      tokens[max_pre_pos].value=(tokens[pre].value^tokens[next].value);
      break;
    case RULE_EQ:
      tokens[max_pre_pos].value=(tokens[pre].value==tokens[next].value);
      break;
    case RULE_NE:
      tokens[max_pre_pos].value=(tokens[pre].value!=tokens[next].value);
      break;
    case RULE_GT:
      tokens[max_pre_pos].value=(tokens[pre].value>tokens[next].value);
      break;
    case RULE_LT:
      tokens[max_pre_pos].value=(tokens[pre].value<tokens[next].value);
      break;
    case RULE_GE:
      tokens[max_pre_pos].value=(tokens[pre].value>=tokens[next].value);
      break;
    case RULE_LE:
      tokens[max_pre_pos].value=(tokens[pre].value<=tokens[next].value);
      break;
    case RULE_ADD:
      tokens[max_pre_pos].value=(tokens[pre].value+tokens[next].value);
      break;
    case RULE_SUB:
      tokens[max_pre_pos].value=(tokens[pre].value-tokens[next].value);
      break;
    case RULE_MUL:
      tokens[max_pre_pos].value=(tokens[pre].value*tokens[next].value);
      break;
    case RULE_DIV:
      if (tokens[next].value) {
        tokens[max_pre_pos].value=(tokens[pre].value/tokens[next].value);
      }
      else {
        fputs("Divisor cannot be zero!\n", stderr);
        *success=false;
        return -1;
      }
      break;
    case RULE_REM:
      if (tokens[next].value) {
        tokens[max_pre_pos].value=(tokens[pre].value%tokens[next].value);
      }
      else {
        fputs("Divisor cannot be zero!\n", stderr);
        *success=false;
        return -1;
      }
      break;
    case RULE_SHIFT_L:
      tokens[max_pre_pos].value=(tokens[pre].value<<tokens[next].value);
      break;
    case RULE_SHIFT_R:
      tokens[max_pre_pos].value=(tokens[pre].value>>tokens[next].value);
      break;
    case RULE_NOT:
      tokens[max_pre_pos].value=(!tokens[next].value);
      break;
    case RULE_NEG:
      tokens[max_pre_pos].value=(-tokens[next].value);
      break;
    case RULE_DER:
      tokens[max_pre_pos].value=(int)swaddr_read((swaddr_t)tokens[next].value,4);
      break;
    case RULE_BIT_NOT:
      tokens[max_pre_pos].value=(~tokens[next].value);
      break;
  }
  tokens[next].type=RULE_NOTYPE;
  if (tokens[max_pre_pos].type<RULE_NOT) {
    tokens[pre].type=RULE_NOTYPE;
  }
  tokens[max_pre_pos].type=RULE_DIGIT;
      }
      else {
  printf("calculate error! please debug!\n");
  *success=false;
  return 0;
      }
    }
  }
  for (i=p;i<=q;++i) {
    if (tokens[i].type>RULE_NOTYPE) {
      *success=true;
      return tokens[i].value;
    }
  }
  *success=false;
  return -1;
}

static int check_brackets(int p, int q, int *r, int *s) {
  int i;
  for (i=p; i<=q; ++i) {
    if (tokens[i].type==RULE_BRA_L) {
      int j=i+1;
      int cnt=1, cnt2=1;
      for (;j<=q;++j ) {
  if (tokens[j].type==RULE_BRA_L) {
    ++cnt;
    ++cnt2;
  }
  else if (tokens[j].type==RULE_BRA_R) {
    --cnt;
    if (cnt==0) {
      *r=i;
      *s=j;
      return cnt2;
    }
  }
      }
      return -1;
    }
  }
  return 0;
}

static int eval_bra(int p, int q, bool *success) {
  int pp=p,qq=q;
  int check;
  while ((check=check_brackets(p,q,&pp,&qq))!=0) {
    if (check==-1) {
      *success=false;
      return -1;
    }
    tokens[pp].type=RULE_NOTYPE;
    tokens[qq].type=RULE_NOTYPE;
    eval_bra(pp+1,qq-1,success);
    if (*success!=true) {
      return -1;
    }
  }
  return eval(p,q,success);
}

int eval_start(bool *success) {
  if (nr_token>=2 && tokens[1].type==RULE_ASSIGN) {
    int key=eval_bra(2, nr_token-1, success);
    int i=set_var(tokens[0].str, key);
    if (success==false) {
      return -1;
    }
    if (i==-1) {
      printf("The result is ");
      return key;
    }
    printf("%s = ", var[i].str);
    return key;
  }
  else {
    return eval_bra(0, nr_token-1, success);
  }
}

uint32_t expr(char *e, bool *success, int prompt) {
  if (e==NULL) {
    return 1;
  }
  if (strcmp("clear",e)==0) {
    clear_var();
    *success=true;
    return 0;
  }
  bool is_match=true;
  if(!make_token(e, &is_match, prompt)) {
    //printf("is_match: %d\n",is_match);
    if (is_match==false) {
      *success = false;
    }
    else {
      *success=true;
    }
    return 0;
  }
  bool s=true;
  int result=eval_start(&s);
  if (s==true) {
    printf("%d\n", result);
  }
  
  /* done */
  /* TODO: Insert codes to evaluate the expression. */
  //panic("please implement me");
  return 0;
}
