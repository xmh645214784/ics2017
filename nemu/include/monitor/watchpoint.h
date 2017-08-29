#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include "common.h"
#include "expr.h"

typedef struct watchpoint {
    int NO;
    int key;
    struct watchpoint *next;
    char str[32];
    /* TODO: Add more members if necessary */


} WP;

extern WP *head;
WP* new_wp();
int free_wp(int NO);

#endif
