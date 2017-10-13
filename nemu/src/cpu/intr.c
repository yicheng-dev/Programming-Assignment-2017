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

  uint32_t t0 = cpu.idtr.val + NO*8;
  uint32_t t1 = cpu.idtr.val + NO*8 + 4;
  t0 = vaddr_read(t0,4);
  t1 = vaddr_read(t1,4);
  t0 &= 0x0000ffff;
  t1 &= 0xffff0000;
  uint32_t ret = t0|t1;
  decoding.jmp_eip = ret;
  decoding.is_jmp = 1;
  printf("dest: %u\n",gatedesc.val);
  
}

void dev_raise_intr() {
}
