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
  id_dest->width = decoding.is_operand_size_16 ? 2 : 4;
  rtl_lr(&t1, id_dest->width, R_ESP);
  t1 -= id_dest->width;
  rtl_sr(R_ESP, id_dest->width, &t1);

  id_dest->type = OP_TYPE_MEM;
  id_dest->addr = t1;
  t2 = cpu.eip + id_dest->width + 1;
  operand_write(id_dest, &t2);

  cpu.eip = (cpu.eip + id_dest->val) & ((id_dest->width == 2) ? 0x0000FFFF : 0xFFFFFFFF);

  print_asm("call %x", decoding.jmp_eip);
}

make_EHelper(ret) {
  TODO();

  print_asm("ret");
}

make_EHelper(call_rm) {
  TODO();

  print_asm("call *%s", id_dest->str);
}
