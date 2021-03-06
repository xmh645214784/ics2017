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
#ifdef DEBUG
  sprintf(id_dest->str, "%%%s", reg_name(id_dest->reg, id_dest->width));
#endif
  print_asm_template2(movsx);
}

make_EHelper(movzx) {
  id_dest->width = decoding.is_operand_size_16 ? 2 : 4;
  operand_write(id_dest, &id_src->val);
#ifdef DEBUG
  sprintf(id_dest->str, "%%%s", reg_name(id_dest->reg, id_dest->width));
#endif
  print_asm_template2(movzx);
}

make_EHelper(lea) {
  rtl_li(&t2, id_src->addr);
  operand_write(id_dest, &t2);
  print_asm_template2(lea);
}

make_EHelper(movcr_s) {
  cpu.CR[id_src->reg] = id_dest->val;
  print_asm("movl %s,cr%d", id_dest->str, id_src->reg);
}

make_EHelper(movcr_l) {
  reg_l(id_src->reg) = cpu.CR[id_dest->reg];
  print_asm("movl cr%d,%s", id_dest->reg, id_src->str);
}

make_EHelper(movs) {
  id_dest->type = OP_TYPE_MEM;
  id_dest->addr = cpu.gpr[R_EDI]._32;
  id_dest->width = decoding.is_operand_size_16 ? 2 : 4;
  t0 = cpu.gpr[R_ESI]._32;
  rtl_lm(&t1, &t0, id_dest->width);
  operand_write(id_dest, &t1);
  cpu.gpr[R_ESI]._32 += (eflags(DF) ? -id_dest->width : id_dest->width);
  cpu.gpr[R_EDI]._32 += (eflags(DF) ? -id_dest->width : id_dest->width);

#ifdef DEBUG
  sprintf(id_src->str, "%s%#x", ((int)t0 < 0 ? "-" : ""), ((int)t0 < 0 ? (int)-t0 : (int)t0));
  sprintf(id_dest->str, "%%es:(%%edi)");
#endif

  print_asm_template2(movs);
}