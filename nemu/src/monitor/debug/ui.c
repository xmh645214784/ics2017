#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

void cpu_exec(uint64_t);

/* We use the `readline' library to provide more flexibility to read from stdin. */
char* rl_gets() {
    static char *line_read = NULL;

    if (line_read) {
        free(line_read);
        line_read = NULL;
    }

    line_read = readline("(nemu) ");

    if (line_read && *line_read) {
        add_history(line_read);
    }

    return line_read;
}

static int cmd_c(char *args) {
    cpu_exec(-1);
    return 0;
}

static int cmd_q(char *args) {
    return -1;
}
static int cmd_help(char *args);

static int cmd_si(char *args);

static int cmd_info(char *args);

static int cmd_x(char *args);

static int cmd_w(char *args);

static int cmd_d(char *args);

static int cmd_p(char *args);

static int cmd_b(char *args);

static struct {
    char *name;
    char *description;
    int (*handler) (char *);
} cmd_table [] = {
        { "help", "Display informations about all supported commands", cmd_help },
        { "c", "Continue the execution of the program", cmd_c },
        { "q", "Exit NEMU", cmd_q },
        { "si", "Step one instruction exactly.", cmd_si },
        { "info", "Generic command for showing things about the program being debugged.", cmd_info },
        { "x", "Examine memory: x FMT ADDRESS.", cmd_x },
        { "w", "Set a watchpoint for an expression.", cmd_w },
        { "d", "Delete some breakpoints or auto-display expressions", cmd_d},
        { "p", "Print value of expression EXP.", cmd_p},
        { "b", "Set breakpoint at specified line", cmd_b},

        /* TODO: Add more commands */

};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int my_help(const char *args) {
    int i;
    for(i = 0; i < NR_CMD; i ++) {
        if(strcmp(args, cmd_table[i].name) == 0) {
            printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
            return 0;
        }
    }
    printf("Unknown command '%s'\n", args);
    return 0;
}

static int cmd_help(char *args) {
    /* extract the first argument */
    char *arg = strtok(NULL, " ");
    int i;

    if (arg == NULL) {
        /* no argument given */
        for (i = 0; i < NR_CMD; i ++) {
            printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        }
    }
    else {
        for (i = 0; i < NR_CMD; i ++) {
            if (strcmp(arg, cmd_table[i].name) == 0) {
                printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
                return 0;
            }
        }
        printf("Unknown command '%s'\n", arg);
    }
    return 0;
}

static int cmd_si(char *args) {
    uint32_t n = 1;
    char *arg = strtok(NULL, " ");
    if(arg != NULL) {
        uint32_t getNum = (uint32_t) strtol(arg, NULL, 0);
        if ((int)getNum <= 0) {
            fputs("expected a number greater than 0.\n", stderr);
            return 1;
        }
        n = getNum;
    }
    cpu_exec(n);
    return 0;
}

static char *cpu_name[]={"eax", "ecx", "edx", "ebx", "esp", "ebp", "esi", "edi", "eip"};

static int print_cpu(int reg) {
    if (reg < 0 || reg > 8) {
        return 1;
    }
    else if (reg == 8) {
        printf("%-12s0x%x\t0x%x\n", cpu_name[reg], cpu.eip, cpu.eip);
    }
    else if (reg == R_ESP || reg == R_EBP) {
        printf("%-12s0x%x\t0x%x\n", cpu_name[reg], cpu.gpr[reg]._32, cpu.gpr[reg]._32);
    }
    else {
        printf("%-12s0x%x\t%d\n", cpu_name[reg], cpu.gpr[reg]._32, cpu.gpr[reg]._32);
    }
    return 0;
}

static int cmd_info(char *args) {
    char *arg = strtok(args, " ");
    if (arg != NULL) {
        if (strcmp("r", arg) == 0 || strcmp("registers", arg) == 0) {
            int i;
            for (i = 0; i < 9; ++i) {
                print_cpu(i);
            }
        }
        else if (strcmp("w", arg) == 0 || strcmp("watchpoints", arg) == 0) {
            WP *p=head;
            printf("Num     Value   What\n");
            for (;p!=NULL;p=p->next) {
                printf("%-8d%-8d%s\n",p->NO, p->key, p->str);
            }
        }
        else {
            my_help("info");
            return 1;
        }
    }
    else {
        my_help("info");
        return 1;
    }
    return 0;
}

static int print_addr(int n, swaddr_t addr) {
    int i;
    for (i = 0; i < n; ++i) {
        if (i % 4 == 0) {
            printf("0x%x:\t", (uint32_t)addr);
        }
        printf("0x%08x\t", (uint32_t)swaddr_read(addr, 4));
        if (i % 4 == 3) {
            fputc('\n', stdout);
        }
        addr += 4;
    }
    if (i % 4 != 0) {
        fputc('\n', stdout);
    }
    return 0;
}

static int cmd_x(char *args) {
    char *arg1 = strtok(NULL, " ");
    char *arg2 = NULL;
    bool s=true;
    if (arg1 != NULL) {
        //arg2 = strtok(args + strlen(arg1) + 1, " ");
        arg2 = args + strlen(arg1) + 1;
        if (!make_token(arg2, &s, 0))
        {
            printf("Bad expression!\n");
            return 0;
        }
    }
    int n = 1;
    uint32_t getAddr;
    if (arg1 == NULL) {
        my_help("x");
        return 1;
    }
    else if (arg2 != NULL)
    {
        int getNum = (int) strtol(arg1, NULL, 0);
        getAddr = eval_start(&s);
        if (getNum <= 0 || (int)getAddr < 0) {
            fputs("expected numbers greater than 0.\n", stderr);
            return 1;
        }
        n = getNum;
    }
    else
    {
        getAddr = eval_start(&s);
        if ((int)getAddr < 0) {
            fputs("expected a valid address.\n", stderr);
            return 1;
        }
    }
    if (s==false)
    {
        printf("Bad expression!\n");
        return 0;
    }
    print_addr(n, getAddr);
    return 0;
}

static int cmd_w(char *args) {
    //char *arg = strtok(NULL, " ");
    bool is_success=true;
    if (args==NULL) {
        char w[]="w";
        cmd_info(w);
        return 0;
    }
    if(!make_token(args, &is_success,1)) {
        if (is_success==false) {
            fputs("Invalid expression!\n" ,stderr);
        }
        return 1;
    }
    int result=eval_start(&is_success);
    if (is_success==false) {
        return 1;
    }
    WP *p=new_wp();
    if (p==NULL) {
        return 2;
    }
    p->key=result;
    strcpy(p->str, args);
    printf("watchpoint %d: %s\n", p->NO, p->str);
    return 0;
}

static int cmd_d(char *args) {
    char *arg = strtok(NULL, " ");
    if (arg==NULL)
    {
        my_help("d");
        return 0;
    }
    int getNum = (int) strtol(arg, NULL, 0);
    if (getNum <= 0) {
        fputs("expected numbers greater than 0.\n", stderr);
        return 1;
    }
    return free_wp(getNum);
}

static int cmd_p(char *args) {
    if (args==NULL)
    {
        my_help("p");
        return 0;
    }
    bool is_success=true;
    expr(args, &is_success, 1);
    if (is_success==true) {
        return 0;
    }
    else {
        return 1;
    }
}

static int cmd_b(char *args) {
    //char *arg = strtok(NULL, " ");
    if (args==NULL)
    {
        my_help("b");
        return 0;
    }
    char str[32];
    strcpy(str, "$eip==");
    //printf("%s\n", str);
    strcat(str, args);
    //printf("%s\n",str);
    return cmd_w(str);
}

void ui_mainloop(int is_batch_mode) {
    if (is_batch_mode) {
        cmd_c(NULL);
        return;
    }

    while (1) {
        char *str = rl_gets();
        if (str==NULL)
        {
            printf("exit\n");
            return;
        }
        char *str_end = str + strlen(str);

        /* extract the first token as the command */
        char *cmd = strtok(str, " ");
        if (cmd == NULL) { continue; }

        /* treat the remaining string as the arguments,
         * which may need further parsing
         */
        char *args = cmd + strlen(cmd) + 1;
        if (args >= str_end) {
            args = NULL;
        }

#ifdef HAS_IOE
        extern void sdl_clear_event_queue(void);
    sdl_clear_event_queue();
#endif

        int i;
        for(i = 0; i < NR_CMD; i ++) {
            if(strcmp(cmd, cmd_table[i].name) == 0) {
                if(cmd_table[i].handler(args) < 0) { return; }
                break;
            }
        }
        if(i == NR_CMD) {
            bool success=true;
            int len=0;
            if (args!=NULL) {
                len=strlen(cmd);
                //strcat(str, args);
                cmd[len]=' ';
            }
            expr(str,&success,0);
            if (success==false) {
                if (len!=0) {
                    cmd[len]='\0';
                }
                printf("Unknown command '%s'\n", cmd);
            }
        }
    }
}
