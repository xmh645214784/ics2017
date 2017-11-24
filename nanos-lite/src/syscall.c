#include "common.h"
#include "syscall.h"
#include "fs.h"
#include "proc.h"

int mm_brk(uint32_t new_brk);

uint32_t sys_write(int fd, const void *buf, uint32_t count) {
  const char *p = buf;
  int i = 0;
  if (fd == 2) printk("\33[1;34m");
  if (fd == 1 || fd == 2) { 
    for (; i < count; ++i) _putc(p[i]);
  }
  else {
    return fs_write(fd, buf, count);
  }
  if (fd == 2) printk("\33[0m");
  return count;
}

_RegSet* do_syscall(_RegSet *r) {
  uintptr_t a[4];
  a[0] = SYSCALL_ARG1(r);
  // if (a[0] != SYS_write) printk("Syscall: %d\n", a[0]);
  switch (a[0]) {
    case SYS_none: SYSCALL_ARG1(r) = 1; break;
    case SYS_exit: _halt(SYSCALL_ARG2(r)); break;
    case SYS_write: 
      SYSCALL_ARG1(r) = sys_write(SYSCALL_ARG2(r), (const void *)SYSCALL_ARG3(r), SYSCALL_ARG4(r));
      break;
    case SYS_open:
      SYSCALL_ARG1(r) = fs_open((const char *)SYSCALL_ARG2(r), SYSCALL_ARG3(r), SYSCALL_ARG4(r));
      break;
    case SYS_read:
      SYSCALL_ARG1(r) = fs_read(SYSCALL_ARG2(r), (void *)SYSCALL_ARG3(r), SYSCALL_ARG4(r));
      break;
    case SYS_lseek:
      SYSCALL_ARG1(r) = fs_lseek(SYSCALL_ARG2(r), SYSCALL_ARG3(r), SYSCALL_ARG4(r));
      break;
    case SYS_close:
      SYSCALL_ARG1(r) = fs_close(SYSCALL_ARG2(r));
      break;
    case SYS_brk:
      SYSCALL_ARG1(r) = mm_brk(SYSCALL_ARG2(r));
      break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }

  return NULL;
}
