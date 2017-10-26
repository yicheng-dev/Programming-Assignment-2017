#include "cpu/exec.h"

make_EHelper(mov) {
  operand_write(id_dest, &id_src->val);
  print_asm_template2(mov);
}

make_EHelper(push_I8){
  rtl_sext(&t1, &id_dest->val, id_dest->width);
  rtl_push(&t1);
  print_asm_template1(push);
}

make_EHelper(push) {
  rtl_push(&id_dest->val);
  print_asm_template1(push);
}

make_EHelper(pop) {
  rtl_pop(&id_dest->val);
  operand_write(id_dest, &id_dest->val);
  print_asm_template1(pop);
}

make_EHelper(pusha) {
/*  if (decoding.is_operand_size_16) {
	t0=cpu.sp;
	t1=cpu.ax;
	t2=cpu.cx;
	t3=cpu.dx;
	rtl_push(&t1);
	rtl_push(&t2);
	rtl_push(&t3);
	t1=cpu.bx;
	t2=cpu.bp;
	t3=cpu.si;
	rtl_push(&t1);
	rtl_push(&t0);
	rtl_push(&t2);
	rtl_push(&t3);
	t1=cpu.di;
	rtl_push(&t1);
  }
  else {*/
//	printf("eax:%u;ecx:%u;edx:%u;ebx:%u;esi:%u;edi:%u;esp:%u;ebp:%u\n",cpu.eax,cpu.ecx,cpu.edx,cpu.ebx,cpu.esi,cpu.edi,cpu.esp,cpu.ebp);
    t0=cpu.esp;
	t1=cpu.eax;
	t2=cpu.ecx;
	t3=cpu.edx;
	rtl_push(&t1);
	rtl_push(&t2);
	rtl_push(&t3);
	t1=cpu.ebx;
	t2=cpu.ebp;
	t3=cpu.esi;
	rtl_push(&t1);
	rtl_push(&t0);
	rtl_push(&t2);
	rtl_push(&t3);
	t1=cpu.edi;
	rtl_push(&t1);
  
  print_asm("pusha");
}

make_EHelper(popa) {
  if (decoding.is_operand_size_16) {
	rtl_pop(&t0);
	rtl_pop(&t1);
	rtl_pop(&t2);
	rtl_sr_w(R_DI,&t0);
	rtl_sr_w(R_SI,&t1);
	rtl_sr_w(R_BP,&t2);
	rtl_pop(&t0);
	rtl_pop(&t0);
	rtl_pop(&t1);
	rtl_pop(&t2);
	rtl_pop(&t3);
	rtl_sr_w(R_BX,&t0);
	rtl_sr_w(R_DX,&t1);
	rtl_sr_w(R_CX,&t2);
	rtl_sr_w(R_AX,&t3);
  }
  else {
	rtl_pop(&t0);
	rtl_pop(&t1);
	rtl_pop(&t2);
	rtl_sr_l(R_EDI,&t0);
	rtl_sr_l(R_ESI,&t1);
	rtl_sr_l(R_EBP,&t2);
	rtl_pop(&t0);
	rtl_pop(&t0);
	rtl_pop(&t1);
	rtl_pop(&t2);
	rtl_pop(&t3);
	rtl_sr_l(R_EBX,&t0);
	rtl_sr_l(R_EDX,&t1);
	rtl_sr_l(R_ECX,&t2);
	rtl_sr_l(R_EAX,&t3);

  }
  print_asm("popa");
}

make_EHelper(leave) {
  if (decoding.is_operand_size_16) {
	rtl_lr_w(&t0,R_SP);
	rtl_lr_w(&t1,R_BP);
	rtl_mv(&t0,&t1);
	rtl_sr_w(R_SP,&t0);
	rtl_pop(&t3);
	rtl_sr_w(R_BP,&t3);
  }
  else {	
    rtl_lr_l(&t0,R_ESP);
    rtl_lr_l(&t1,R_EBP);
    rtl_mv(&t0,&t1);
    rtl_sr_l(R_ESP,&t0);
    rtl_pop(&reg_l(R_EBP));
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

make_EHelper(xchg) {
  rtl_mv(&t0, &id_dest->val);
  rtl_mv(&t1, &id_src->val);
  operand_write(id_dest, &t1);
  operand_write(id_src, &t0);

  print_asm("xchg *%s *%s", id_src->str, id_dest->str);
}

make_EHelper(rol) {
  rtl_mv(&t0,&id_src->val);
  while (t0 != 0){
	rtl_msb(&t1,&id_dest->val,id_dest->width);
	rtl_shli(&id_dest->val,&id_dest->val,1);
	rtl_add(&id_dest->val,&id_dest->val,&t1);
	t0--;
  }
  operand_write(id_dest,&id_dest->val);
}
