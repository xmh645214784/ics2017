#include "cpu/exec.h"

make_EHelper(jmp) {
  // the target address is calculated at the decode stage
  decoding.is_jmp = 1;

  print_asm("jmp %x", decoding.jmp_eip);
}

make_EHelper(jcc) {
  // the target address is calculated at the decode stage
  uint8_t subcode = decoding.opcode & 0xf;
  rtl_setcc(&t2, subcode);
  decoding.is_jmp = t2;

  print_asm("j%s %x", get_cc_name(subcode), decoding.jmp_eip);
}

make_EHelper(jmp_rm) {
  decoding.jmp_eip = id_dest->val;
  decoding.is_jmp = 1;

  print_asm("jmp *%s", id_dest->str);
}

make_EHelper(call) {
  // the target address is calculated at the decode stage
  //TODO();
  t2 = *eip;
  rtl_pushv(&t2, id_dest->width);

  decoding.is_jmp = 1;
  decoding.jmp_eip = (t2 + id_dest->val);

  print_asm("call %x", decoding.jmp_eip);
}

make_EHelper(ret) {
  //TODO();

  rtl_pop(&t0);
  decoding.is_jmp = 1;
  decoding.jmp_eip = t0;
  print_asm("ret");
}

make_EHelper(call_rm) {
  //TODO();
  t2 = *eip;
  rtl_pushv(&t2, id_dest->width);

  decoding.is_jmp = 1;
  decoding.jmp_eip = (cpu.eip + id_dest->val);
  printf("jmp: %x\neip: %x\n", decoding.jmp_eip, cpu.eip);

  print_asm("call *%s", id_dest->str);
}
