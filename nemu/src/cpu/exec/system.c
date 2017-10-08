#include "cpu/exec.h"

void diff_test_skip_qemu();
void diff_test_skip_nemu();

make_EHelper(lidt) {
  //TODO();
  cpu.IDTR.limit = vaddr_read(id_dest->addr, 2);
  cpu.IDTR.base = vaddr_read(id_dest->addr + 2, 4);

  print_asm_template1(lidt);
}

make_EHelper(mov_r2cr) {
  TODO();

  print_asm("movl %%%s,%%cr%d", reg_name(id_src->reg, 4), id_dest->reg);
}

make_EHelper(mov_cr2r) {
  TODO();

  print_asm("movl %%cr%d,%%%s", id_src->reg, reg_name(id_dest->reg, 4));

#ifdef DIFF_TEST
  diff_test_skip_qemu();
#endif
}

void raise_intr(uint8_t NO, vaddr_t ret_addr);
make_EHelper(int) {
  //TODO();
  raise_intr(id_dest->val, *eip);

  print_asm("int %s", id_dest->str);

#ifdef DIFF_TEST
  diff_test_skip_nemu();
#endif
}

make_EHelper(iret) {
  //TODO();
  rtl_pop(&t0);
  rtl_pop(&t1);
  rtl_pop(&t2);
  decoding.is_jmp = 1;
  decoding.jmp_eip = t0;
  cpu.cs_init = t1;
  cpu.eflags_init = t2;

  print_asm("iret");
}

uint32_t pio_read(ioaddr_t, int);
void pio_write(ioaddr_t, int, uint32_t);

make_EHelper(in) {
  //TODO();
  reg_l(R_EAX) = pio_read(reg_w(R_EDX), id_dest->width);

  print_asm_template2(in);

#ifdef DIFF_TEST
  diff_test_skip_qemu();
#endif
}

make_EHelper(out) {
  //TODO();
  pio_write(reg_w(R_EDX), id_dest->width, reg_l(R_EAX));

  print_asm_template2(out);

#ifdef DIFF_TEST
  diff_test_skip_qemu();
#endif
}
