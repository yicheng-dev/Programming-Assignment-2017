#include "cpu/exec.h"
#include "memory/mmu.h"

void raise_intr(uint8_t NO, vaddr_t ret_addr) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * That is, use ``NO'' to index the IDT.
   */
  rtl_push((rtlreg_t*)&cpu.eflags);
  t0 = cpu.CS;
  rtl_push(&t0);
  rtl_push(&ret_addr);
  t0 = cpu.idtr.base+NO*sizeof(GateDesc);
  GateDesc t;
  rtl_lm ((rtlreg_t*)&t,&t0,4);
  t0+=4;
  rtl_lm(((rtlreg_t*)&t)+1,&t0,4);
  t0 = t.offset_15_0 + (t.offset_31_16<<16);
  decoding.is_jmp = 1;
  decoding.jmp_eip = t0;
}

void dev_raise_intr() {
}
