#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "monitor/monitor.h"

/* The assembly code of instructions executed is only output to the screen
 * when the number of instructions executed is less than this value.
 * This is useful when you use the `si' command.
 * You can modify this value as you want.
 */
#define MAX_INSTR_TO_PRINT 10

int nemu_state = NEMU_STOP;

void exec_wrapper(bool);

/* Simulate how the CPU works. */
void cpu_exec(uint64_t n) {
    if (nemu_state == NEMU_END) {
        printf("Program execution has ended. To restart the program, exit NEMU and run again.\n");
        return;
    }
    nemu_state = NEMU_RUNNING;

    bool print_flag = n < MAX_INSTR_TO_PRINT;

    for (; n > 0; n --) {
        /* Execute one instruction, including instruction fetch,
         * instruction decode, and the actual execution. */
        exec_wrapper(print_flag);

#ifdef DEBUG
        /* TODO: check watchpoints here. */
    WP *p=head;
    bool is_changed=false;
    for (;p!=NULL;p=p->next) {
      if (p->str==NULL) {
          fprintf(stderr, "Bad watchpoint %d: %s\n", p->NO, p->str);
          continue;
      }
      char str_tmp[32];
      strcpy(str_tmp, p->str);
      bool is_success=true;
      if (!make_token(str_tmp,&is_success,1 )) {
          if (is_success==false) {
            fprintf(stderr, "Bad watchpoint %d: %s\n", p->NO, p->str);
            continue;
          }
      }
      int result=eval_start(&is_success);
      if (is_success==false) {
        fprintf(stderr, "Bad watchpoint %d: %s\n", p->NO, p->str);
      }
      else if (result!=p->key) {
        printf("watchpoint %d: %s\n", p->NO, p->str);
        printf("Old value: %d\nNew value: %d\n", p->key, result);
        is_changed=true;
        p->key=result;
      }
    }
    if (is_changed==true) {
      nemu_state=NEMU_STOP;
    }
#endif

#ifdef HAS_IOE
        extern void device_update();
    device_update();
#endif

        if (nemu_state != NEMU_RUNNING) { return; }
    }

    if (nemu_state == NEMU_RUNNING) { nemu_state = NEMU_STOP; }
}
