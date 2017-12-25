#include <x86.h>

#define PG_ALIGN __attribute((aligned(PGSIZE)))

static PDE kpdirs[NR_PDE] PG_ALIGN;
static PTE kptabs[PMEM_SIZE / PGSIZE] PG_ALIGN;
static void* (*palloc_f)();
static void (*pfree_f)(void*);

_Area segments[] = {      // Kernel memory mappings
  {.start = (void*)0, .end = (void*)PMEM_SIZE}
};

#define NR_KSEG_MAP (sizeof(segments) / sizeof(segments[0]))

void _pte_init(void* (*palloc)(), void (*pfree)(void*)) {
  palloc_f = palloc;
  pfree_f = pfree;

  int i;

  // make all PDEs invalid
  for (i = 0; i < NR_PDE; i ++) {
    kpdirs[i] = 0;
  }

  PTE *ptab = kptabs;
  for (i = 0; i < NR_KSEG_MAP; i ++) {
    uint32_t pdir_idx = (uintptr_t)segments[i].start / (PGSIZE * NR_PTE);
    uint32_t pdir_idx_end = (uintptr_t)segments[i].end / (PGSIZE * NR_PTE);
    for (; pdir_idx < pdir_idx_end; pdir_idx ++) {
      // fill PDE
      kpdirs[pdir_idx] = (uintptr_t)ptab | PTE_P;

      // fill PTE
      PTE pte = PGADDR(pdir_idx, 0, 0) | PTE_P;
      PTE pte_end = PGADDR(pdir_idx + 1, 0, 0) | PTE_P;
      for (; pte < pte_end; pte += PGSIZE) {
        *ptab = pte;
        ptab ++;
      }
    }
  }

  set_cr3(kpdirs);
  set_cr0(get_cr0() | CR0_PG);
}

void _protect(_Protect *p) {
  PDE *updir = (PDE*)(palloc_f());
  p->ptr = updir;
  // map kernel space
  for (int i = 0; i < NR_PDE; i ++) {
    updir[i] = kpdirs[i];
  }

  p->area.start = (void*)0x8000000;
  p->area.end = (void*)0xc0000000;
}

void _release(_Protect *p) {
}

void _switch(_Protect *p) {
  set_cr3(p->ptr);
}

void _map(_Protect *p, void *va, void *pa) {
  PDE *pdir = (PDE*)((uint32_t*)(p->ptr))[PDX(va)];
  if (!((uint32_t)pdir & 0x1)) {
    pdir = (PDE*)(palloc_f());
    ((uint32_t*)(p->ptr))[PDX(va)] = (uintptr_t)pdir | PTE_P;
    
    for (int i = 0; i < NR_PTE; i ++)
      pdir[i] = 0;
  }
  PTE *ptab = (PTE*)PTE_ADDR(pdir);
  ptab[PTX(va)] = ((uintptr_t)pa & ~0xfff) | PTE_P;
}

void _unmap(_Protect *p, void *va) {
}

_RegSet *_umake(_Protect *p, _Area ustack, _Area kstack, void *entry, char *const argv[], char *const envp[]) {
  uint32_t *stack = (uint32_t*)ustack.end;
  *(--stack) = 0x0;   //argc
  *(--stack) = 0x0;   //argv
  *(--stack) = 0x0;   //envp
  *(--stack) = 0x0;   //_start ret_address
  *(--stack) = 0x202; //eflags
  *(--stack) = 0x8;   //cs
  *(--stack) = (uint32_t)entry; //eip
  *(--stack) = 0x0;   //error_code
  *(--stack) = 0x81;  //irq
  *(--stack) = 0x0;   //eax
  *(--stack) = 0x0;   //ecx
  *(--stack) = 0x0;   //edx
  *(--stack) = 0x0;   //ebx
  *(--stack) = 0x0;   //esp
  *(--stack) = (uint32_t)ustack.end; //ebp
  *(--stack) = 0x0;   //esi
  *(--stack) = 0x0;   //edi

  return (_RegSet*)stack;
}

