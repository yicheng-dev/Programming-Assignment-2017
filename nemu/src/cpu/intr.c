#include "cpu/exec.h"
#include "memory/mmu.h"

void raise_intr(uint8_t NO, vaddr_t ret_addr) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * That is, use ``NO'' to index the IDT.
   */

  rtl_push(&cpu.eflags);
  rtl_push(&cpu.CS);
  rtl_push(&cpu.eip);

  uint32_t data[2], dest;
  data[0] = vaddr_read(cpu.idtr.val + NO*4, 4);
  data[1] = vaddr_read(cpu.idtr.val + NO*4 + 4, 4);
  memcpy(&dest, data, 4);
  cpu.eip = dest;
  
}

void dev_raise_intr() {
}
