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
  decoding.is_jmp = 1;
  rtl_push(eip);
  print_asm("call %x", decoding.jmp_eip);
}

make_EHelper(ret) {
  rtl_pop(eip);
  decoding.jmp_eip = *eip;
  print_asm("ret");
}

make_EHelper(ret_I) {
  rtl_pop(eip);
  decoding.jmp_eip = *eip;
  rtl_lr_l(&t0,R_ESP);
  rtl_addi(&t0,&t0,id_dest->val);
  rtl_sr_l(R_ESP,&t0);
  print_asm("ret %d", id_dest->val);
}

make_EHelper(call_rm) {

  decoding.jmp_eip = id_dest->val;
  decoding.is_jmp = 1;

  print_asm("call *%s", id_dest->str);
}
