#ifndef __ARCH_H__
#define __ARCH_H__

#include <am.h>

#define PMEM_SIZE (128 * 1024 * 1024)
#define PGSIZE    4096    // Bytes mapped by a page

struct _RegSet {
  uintptr_t eax, ecx, edx, ebx, ebp, esp, esi, edi;
  int irq;
  uint32_t error_code, eip, cs, eflags;
};

#define SYSCALL_ARG1(r) r->eax
#define SYSCALL_ARG2(r) r->ecx
#define SYSCALL_ARG3(r) r->edx
#define SYSCALL_ARG4(r) r->ebx

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif
#endif
