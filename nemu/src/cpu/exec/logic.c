#include "cpu/exec.h"

make_EHelper(test) {
  //TODO();
  rtlreg_t result = id_dest->val & id_src->val;
  t0 = 0;
  rtl_set_CF(&t0);
  rtl_set_OF(&t0);
  rtl_update_ZFSF(&result, id_dest->width);
  print_asm_template2(test);
}

make_EHelper(and) {
  //TODO();
  rtlreg_t result = id_dest->val & id_src->val;
  operand_write(id_dest, &result);
  t0 = 0;
  rtl_set_CF(&t0);
  rtl_set_OF(&t0);
  rtl_update_ZFSF(&result, id_dest->width);

  print_asm_template2(and);
}

make_EHelper(xor) {
  //TODO();

  rtlreg_t result = id_dest->val ^ id_src->val;
  operand_write(id_dest, &result);
  t0 = 0;
  rtl_set_CF(&t0);
  rtl_set_OF(&t0);
  rtl_update_ZFSF(&result, id_dest->width);

  print_asm_template2(xor);
}

make_EHelper(or) {
  //TODO();
  rtlreg_t result = id_dest->val | id_src->val;
  operand_write(id_dest, &result);
  t0 = 0;
  rtl_set_CF(&t0);
  rtl_set_OF(&t0);
  rtl_update_ZFSF(&result, id_dest->width);

  print_asm_template2(or);
}

make_EHelper(sar) {
  //TODO();
  int32_t dest;
  if (id_dest->width == 1) {
    dest = (int32_t)((int8_t)id_dest->val);
  }
  else if (id_dest->width == 2) {
    dest = (int32_t)((int16_t)id_dest->val);
  }
  else {
    dest = (int32_t)((int32_t)id_dest->val);
  }
  dest >>= id_src->val & 0x1f;
  t0 = dest;
  operand_write(id_dest, &t0);
  // unnecessary to update CF and OF in NEMU

  print_asm_template2(sar);
}

make_EHelper(shl) {
  //TODO();
  id_dest->val <<= id_src->val & 0x1f;
  operand_write(id_dest, &id_dest->val);
  // unnecessary to update CF and OF in NEMU

  print_asm_template2(shl);
}

make_EHelper(shr) {
  //TODO();
  id_dest->val >>= id_src->val & 0x1f;
  operand_write(id_dest, &id_dest->val);
  // unnecessary to update CF and OF in NEMU

  print_asm_template2(shr);
}

make_EHelper(setcc) {
  uint8_t subcode = decoding.opcode & 0xf;
  rtl_setcc(&t2, subcode);
  operand_write(id_dest, &t2);

  print_asm("set%s %s", get_cc_name(subcode), id_dest->str);
}

make_EHelper(not) {
  //TODO();
  id_dest->val = ~id_dest->val;
  operand_write(id_dest, &id_dest->val);

  print_asm_template1(not);
}
