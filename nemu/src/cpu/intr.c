#include "cpu/exec.h"
#include "memory/mmu.h"

void raise_intr(uint8_t NO, vaddr_t ret_addr) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * That is, use ``NO'' to index the IDT.
   */

  rtl_push(&cpu.eflags);
  rtl_push(&cpu.CS);
  rtl_push(&cpu.eip);
  GateDesc gatedesc;
  rtl_lm(&gatedesc.val,&cpu.idtr.val + NO*8, 4);
  rtl_lm(&gatedesc.val+4,&cpu.idtr.val + NO*8 + 4, 4);
//  data[0]=0x1234;
//  data[1]=0x5678;
//  printf("gatedesc:%x\n",data[0]);
//  printf("data[1]:%x\n",data[1]);
//  printf("cpu.idtr.val: 0x%x\n",cpu.idtr.val);
 /* 
  memcpy(&gatedesc,&data[0],4);
  memcpy(&gatedesc+4,&data[1],4);
  decoding.jmp_eip = gatedesc.val;
  decoding.is_jmp = 1;
  printf("dest: %u\n",gatedesc.val);
  */
}

void dev_raise_intr() {
}
