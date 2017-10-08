#include "cpu/exec.h"
#include "memory/mmu.h"

void raise_intr(uint8_t NO, vaddr_t ret_addr) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * That is, use ``NO'' to index the IDT.
   */
  //TODO();
  t0 = cpu.eflags_init;
  t1 = cpu.cs_init;
  t2 = cpu.eip;
  rtl_push(&t0);
  rtl_push(&t1);
  rtl_push(&t2);
  uint32_t address = NO * 8 + idtr_base;
  uint32_t offset = vaddr_read(address + 4, 4);
  uint32_t selector = vaddr_read(address, 4);
  offset = (offset & 0xffff0000) + (selector & 0xffff);
  cpu.eip = offset;
  printf("offset : 0x%x\n", offset);
  decoding.jmp_eip = ret_addr;
  decoding.is_jmp = 1;
}

void dev_raise_intr() {
}
