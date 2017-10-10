#include "cpu/exec.h"
#include "memory/mmu.h"

void raise_intr(uint8_t NO, vaddr_t ret_addr) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * That is, use ``NO'' to index the IDT.
   */

  rtl_push(&cpu.eflags);
  rtl_push(&cpu.CS);
  rtl_push(&cpu.eip);
//  GateDesc gatedesc;
  uint32_t data[2];
  uint64_t dest;
  data[0] = vaddr_read(cpu.idtr.val + NO*sizeof(GateDesc), 4);
  data[1] = vaddr_read(cpu.idtr.val + NO*sizeof(GateDesc) + 4, 4);
//  printf("cpu.idtr.val: 0x%x\n",cpu.idtr.val);
  memcpy(&dest,data,8);
  decoding.jmp_eip = dest;
  decoding.is_jmp = 1;
//  decoding.jmp_eip = ;
//  printf("dest: %p\n",&dest);  
}

void dev_raise_intr() {
}
