#include "monitor/watchpoint.h"
#include "monitor/expr.h"

#define NR_WP 32

static WP wp_pool[NR_WP];
WP *head, *free_;

void init_wp_pool() {
    int i;
    for(i = 0; i < NR_WP; i ++) {
        wp_pool[i].NO = i + 1;
        wp_pool[i].next = &wp_pool[i + 1];
    }
    wp_pool[NR_WP - 1].next = NULL;

    head = NULL;
    free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */

WP* new_wp() {
    if (free_==NULL) {
        fputs("No more free watchpoint pool!\n", stderr);
        return NULL;
    }
    else {
        WP* p=free_;
        free_=free_->next;
        p->next=NULL;
        if (head!=NULL) {
            p->next=head;
        }
        head=p;
        return p;
    }
}

int free_wp(int NO) {
    if (head==NULL) {
        fputs("Empty watchpoint pool\n", stderr);
        return 1;
    }
    WP* p=head, *q=NULL;
    if (NO!=head->NO) {
        for (;p->next!=NULL;p=p->next) {
            if (p->next->NO==NO) {
                break;
            }
        }
        if (p->next==NULL) {
            fputs("Cannot find the watchpoint!\n",stderr);
            return 1;
        }
        else {
            q=p->next;
            p->next=p->next->next;
        }
    }
    else {
        q=head;
        head=head->next;
    }
    p=free_;
    if (p!=NULL) {
        for (p=free_;p->next!=NULL;p=p->next) {}
        p->next=q;
        q->next=NULL;
        q->NO=p->NO+1;
    }
    else {
        free_=q;
        q->next=NULL;
    }
    return 0;
}
