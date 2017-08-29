#ifndef __EXPR_H__
#define __EXPR_H__

#include "common.h"

uint32_t expr(char *, bool *, int);

typedef struct token {
    int type;
    char str[32];
    int value;
} Token;

extern Token tokens[32];
extern int nr_token;
bool make_token(char *e, bool *is_match, int prompt);
int eval_start(bool *success);

#endif
