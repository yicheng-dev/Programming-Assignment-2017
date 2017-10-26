#include "cpu/exec.h"

make_EHelper(test) {
  t0 = id_dest->val & id_src->val;
  rtl_set_OF(&tzero);
  rtl_set_CF(&tzero);
  rtl_update_ZFSF(&t0,id_dest->width);

  print_asm_template2(test);
}

make_EHelper(and) {
  t0 = id_dest->val & id_src->val;
  rtl_set_OF(&tzero);
  rtl_set_CF(&tzero);
  rtl_update_ZFSF(&t0,id_dest->width);
  operand_write(id_dest,&t0);
  print_asm_template2(and);
}

make_EHelper(xor) {
  t1 = id_dest->val ^ id_src->val;
  operand_write(id_dest,&t1);
  
  rtl_set_OF(&tzero);
  rtl_set_CF(&tzero);
  rtl_update_ZFSF(&t1,id_dest->width); 
  print_asm_template2(xor);
}

make_EHelper(or) {
  t1 = id_dest->val | id_src->val;
  operand_write(id_dest,&t1);
  
  rtl_set_OF(&tzero);
  rtl_set_CF(&tzero);
  rtl_update_ZFSF(&t1,id_dest->width); 

  print_asm_template2(or);
}

make_EHelper(sar) {
  rtl_sext(&t0,&id_dest->val,id_dest->width);
  t0 = c_sar(t0,id_src->val);
  operand_write(id_dest,&t0);
  // unnecessary to update CF and OF in NEMU
  rtl_update_ZFSF(&t0,id_dest->width);
  print_asm_template2(sar);
}

make_EHelper(shl) {
  t0 = c_shl(id_dest->val,id_src->val);
  operand_write(id_dest,&t0);
  // unnecessary to update CF and OF in NEMU
  rtl_update_ZFSF(&t0,id_dest->width);
  print_asm_template2(shl);
}

make_EHelper(shr) {
  t0 = c_shr(id_dest->val, id_src->val);
  operand_write(id_dest,&t0);
  // unnecessary to update CF and OF in NEMU
  rtl_update_ZFSF(&t0,id_dest->width);
  print_asm_template2(shr);
}

make_EHelper(setcc) {
  uint8_t subcode = decoding.opcode & 0xf;
  rtl_setcc(&t2, subcode);
  operand_write(id_dest, &t2);

  print_asm("set%s %s", get_cc_name(subcode), id_dest->str);
}

make_EHelper(not) {
  t0 = id_dest->val;	
  rtl_not(&t0);
  operand_write(id_dest,&t0);

  print_asm_template1(not);
}
