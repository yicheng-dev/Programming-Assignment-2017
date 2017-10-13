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
  if (decoding.is_operand_size_16) {
	rtl_lr_w(&t0,R_SP);
	rtl_lr_w(&t1,R_AX);
	rtl_lr_w(&t2,R_CX);
	rtl_lr_w(&t3,R_DX);
	rtl_push(&t1);
	rtl_push(&t2);
	rtl_push(&t3);
	rtl_lr_w(&t1,R_BX);
	rtl_push(&t1);
	rtl_push(&t0);
	rtl_lr_w(&t0,R_BP);
	rtl_lr_w(&t1,R_SI);
	rtl_lr_w(&t2,R_DI);
	rtl_push(&t0);
	rtl_push(&t1);
	rtl_push(&t2);
  }
  else {
	rtl_lr_w(&t0,R_ESP);
	rtl_lr_l(&t1,R_EAX);
	rtl_lr_l(&t2,R_ECX);
	rtl_lr_l(&t3,R_EDX);
	rtl_push(&t1);
	rtl_push(&t2);
	rtl_push(&t3);
	rtl_lr_l(&t1,R_EBX);
	rtl_push(&t1);
	rtl_push(&t0);
	rtl_lr_l(&t0,R_EBP);
	rtl_lr_l(&t1,R_ESI);
	rtl_lr_l(&t2,R_EDI);
	rtl_push(&t0);
	rtl_push(&t1);
	rtl_push(&t2);
  }
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
    rtl_lr_w(&t0,R_AX);
	rtl_li(&t1,0);
	rtl_slt(&t2,&t0,&t1);
	if (t2 == 1) {
		rtl_li(&t0,0xffff);
		rtl_sr_w(R_DX,&t0);
	}
	else{
		rtl_sr_w(R_DX,&tzero);
	}
  }
  else {
    rtl_lr_l(&t0,R_EAX);
	rtl_li(&t1,0);
	rtl_slt(&t2,&t0,&t1);
	if (t2 == 1){
		rtl_li(&t0,0xffffffff);
		rtl_sr_l(R_EDX,&t0);
	}
	else{
		rtl_sr_l(R_EDX,&tzero);
	}
  }

  print_asm(decoding.is_operand_size_16 ? "cwtl" : "cltd");
}

make_EHelper(cwtl) {
  if (decoding.is_operand_size_16) {
	rtl_lr_b(&t0, R_AL);
	rtl_sext(&t0,&t0,1);
	rtl_sr_w(R_AX,&t0);
  }
  else {
	rtl_lr_w(&t0, R_AX);
	rtl_sext(&t0,&t0,2);
	rtl_sr_l(R_EAX,&t0);
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
