#include "cpu/exec.h"
#include "memory/mmu.h"

void raise_intr(uint8_t NO, vaddr_t ret_addr) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * That is, use ``NO'' to index the IDT.
   */

  rtl_push(&cpu.eflags);
  rtl_push(&cpu.CS);
  rtl_push(&cpu.eip);
  uint16_t data[2];
  GateDesc gatedesc;
  data[0] = vaddr_read(cpu.idtr.val + NO*8, 4);
  data[1] = vaddr_read(cpu.idtr.val + NO*8 + 4, 4);

  printf("data[0]:%x\n",data[0]);
  printf("data[1]:%x\n",data[1]);
  printf("cpu.idtr.val: 0x%x\n",cpu.idtr.val);
  
  memcpy(&gatedesc,data,8);
  decoding.jmp_eip = gatedesc.val;
  decoding.is_jmp = 1;
  printf("dest: %u\n",gatedesc.offset_15_0);  
}

void dev_raise_intr() {
}
