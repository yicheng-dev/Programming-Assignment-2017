#include "cpu/exec.h"

make_EHelper(mov) {
  operand_write(id_dest, &id_src->val);
  print_asm_template2(mov);
}

make_EHelper(push) {
  rtl_push(&id_dest->val);  

  print_asm_template1(push);
}

make_EHelper(pop) {
  
  rtl_pop(&t2);
  operand_write(id_dest,&t2);
  print_asm_template1(pop);
}

make_EHelper(pusha) {
  t0 = reg_l(R_ESP);
  rtl_push(&reg_l(R_EAX));
  rtl_push(&reg_l(R_ECX));
  rtl_push(&reg_l(R_EDX));
  rtl_push(&reg_l(R_EBX));
  rtl_push(&t0);
  rtl_push(&reg_l(R_EBP));
  rtl_push(&reg_l(R_ESI));
  rtl_push(&reg_l(R_EDI));










  print_asm("pusha");
}

make_EHelper(popa) {
   rtl_pop(&reg_l(R_EDI));
   rtl_pop(&reg_l(R_ESI));
   rtl_pop(&reg_l(R_EBP));
   rtl_pop(&t0);
   rtl_pop(&reg_l(R_EBX));
   rtl_pop(&reg_l(R_EDX));
   rtl_pop(&reg_l(R_ECX));
   rtl_pop(&reg_l(R_EAX));
         

  print_asm("popa");
}

make_EHelper(leave) {
  reg_l(R_ESP) = reg_l(R_EBP);
  rtl_pop(&reg_l(R_EBP)); 

  print_asm("leave");
}

make_EHelper(cltd) {
  if (decoding.is_operand_size_16) {
     rtl_sext(&reg_l(R_EAX),&reg_l(R_EAX),2);
  }
  else {
    if ((int) reg_l(R_EAX) < 0) {
		reg_l(R_EDX) = -1;
	}
	else reg_l(R_EDX) = 0;
  }

  print_asm(decoding.is_operand_size_16 ? "cwtl" : "cltd");
}

make_EHelper(cwtl) {
  if (decoding.is_operand_size_16) {
    reg_b(R_AH) = (int) reg_b(R_AL) < 0? -1 : 0;
  }
  else {
    rtl_sext(&reg_l(R_EAX),&reg_l(R_EAX),2);
  }

  print_asm(decoding.is_operand_size_16 ? "cbtw" : "cwtl");
}

make_EHelper(movsx) {
  id_dest->width = decoding.is_operand_size_16 ? 2 : 4;
  rtl_sext(&t2, &id_src->val, id_src->width);
  operand_write(id_dest, &t2);
  print_asm_template2(movsx);
}

make_EHelper(movzx) {
  id_dest->width = decoding.is_operand_size_16 ? 2 : 4;
  operand_write(id_dest, &id_src->val);
  print_asm_template2(movzx);
}

make_EHelper(lea) {
  rtl_li(&t2, id_src->addr);
  operand_write(id_dest, &t2);
  print_asm_template2(lea);
}
