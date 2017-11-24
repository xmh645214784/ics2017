#ifndef __ARCH_H__
#define __ARCH_H__

#include <am.h>

#define PMEM_SIZE (128 * 1024 * 1024)
#define PGSIZE    4096    // Bytes mapped by a page

struct _RegSet {
  union {
  struct {
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
    int32_t irq;
    uint32_t error_code, eip, cs, eflags;
  };
  uint32_t buf[13];
  };
};

#define SYSCALL_ARG1(r) (r->eax)
#define SYSCALL_ARG2(r) (r->ebx)
#define SYSCALL_ARG3(r) (r->ecx)
#define SYSCALL_ARG4(r) (r->edx)

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif
#endif
