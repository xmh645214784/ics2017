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
  //TODO();
  int i = 0;
  t0 = cpu.gpr[R_ESP]._32;
  for (; i < 8; ++i) {
    if (i != R_ESP)
      rtl_push(&cpu.gpr[i]._32);
    else
      rtl_push(&t0);
  }
  print_asm("pusha");
}

make_EHelper(popa) {
  //TODO();
  int i = 7;
  for (; i >= 0; --i) {
    if (R_ESP == i) {
      cpu.gpr[R_ESP]._32 += 4;
    }
    else {
      rtl_pop(&cpu.gpr[i]._32);
    }
  }
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
    rtl_sext(&t0, &reg_l(R_EAX), 2);
    rtl_shri(&reg_l(R_EDX), &t0, 16);
  }
  else {
    rtl_sari(&reg_l(R_EDX), &reg_l(R_EAX), 31);
  }
  print_asm(decoding.is_operand_size_16 ? "cwtl" : "cltd");
}

make_EHelper(cwtl) {
  if (decoding.is_operand_size_16) {
    rtl_shli(&reg_l(R_EAX), &reg_l(R_EAX), 24);
    rtl_sari(&reg_l(R_EAX), &reg_l(R_EAX), 8);
    rtl_shri(&reg_l(R_EAX), &reg_l(R_EAX), 16);
  }
  else {
    rtl_sext(&reg_l(R_EAX), &reg_l(R_EAX), 2);
  } 
  print_asm(decoding.is_operand_size_16 ? "cbtw" : "cwtl");
}

make_EHelper(movsx) {
  id_dest->width = decoding.is_operand_size_16 ? 2 : 4;
  rtl_sext(&t2, &id_src->val, id_src->width);
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

make_EHelper(movcr_s) {
  printf("get into movcr_s, cr%d: %x, reg: %x\n", id_dest->reg, cpu.CR[id_dest->reg], id_src->val);
  cpu.CR[id_dest->reg] = id_src->val;
  print_asm("movcr_s");
}

make_EHelper(movcr_l) {
  printf("get into movcr_l, %s: %x, cr%d: %x\n", reg_name(id_dest->reg, 4), reg_l(id_dest->reg), id_src->reg, cpu.CR[id_src->reg]);
  reg_l(id_dest->reg) = cpu.CR[id_src->reg];
  print_asm("movcr_l");
}