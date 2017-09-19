#include "cpu/exec.h"

make_EHelper(mov) {
  operand_write(id_dest, &id_src->val);
  print_asm_template2(mov);
}

make_EHelper(push) {
  //TODO();
  rtl_pushv(&id_dest->val, id_dest->width);
  
  print_asm_template1(push);
}

make_EHelper(pop) {
  //TODO();
  rtl_popv(&reg_l(id_dest->reg), id_dest->width);

  print_asm_template1(pop);
}

make_EHelper(pusha) {
  TODO();

  print_asm("pusha");
}

make_EHelper(popa) {
  TODO();

  print_asm("popa");
}

make_EHelper(leave) {
  //TODO();
  reg_l(R_ESP) = reg_l(R_EBP);
  rtl_lm(&t0, &reg_l(R_ESP), id_src->width);
  id_dest->type = OP_TYPE_REG;
  id_dest->reg = R_EBP;
  operand_write(id_dest, &t0);
  if (id_dest->width == 2) {
    reg_w(R_ESP) += 2;
  }
  else {
    reg_l(R_ESP) += 4;
  }

  print_asm("leave");
}

make_EHelper(cltd) {
  if (decoding.is_operand_size_16) {
    //TODO();
    if (reg_w(R_EAX) < 0) {
      reg_l(R_EDX) = 0x0000ffff;
    }
    else {
      reg_l(R_EDX) = 0;
    }
  }
  else {
    //TODO();
    if (reg_l(R_EAX) < 0) {
      reg_l(R_EDX) = 0xffffffff;
    }
    else {
      reg_l(R_EDX) = 0;
    }
  }

  print_asm(decoding.is_operand_size_16 ? "cwtl" : "cltd");
}

make_EHelper(cwtl) {
  if (decoding.is_operand_size_16) {
    TODO();
  }
  else {
    TODO();
  }

  print_asm(decoding.is_operand_size_16 ? "cbtw" : "cwtl");
}

make_EHelper(movsx) {
  id_dest->width = decoding.is_operand_size_16 ? 2 : 4;
  printf("bef: %x\n", id_src->val);
  rtl_sext(&t2, &id_src->val, id_src->width);
  printf("aft: %x\n", t2);
  operand_write(id_dest, &t2);
  sprintf(id_dest->str, "%%%s", reg_name(id_dest->reg, id_dest->width));
  print_asm_template2(movsx);
}

make_EHelper(movzx) {
  id_dest->width = decoding.is_operand_size_16 ? 2 : 4;
  operand_write(id_dest, &id_src->val);
  sprintf(id_dest->str, "%%%s", reg_name(id_dest->reg, id_dest->width));
  print_asm_template2(movzx);
}

make_EHelper(lea) {
  rtl_li(&t2, id_src->addr);
  operand_write(id_dest, &t2);
  print_asm_template2(lea);
}
