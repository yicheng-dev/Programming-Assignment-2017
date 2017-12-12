#include <x86.h>

#define PG_ALIGN __attribute((aligned(PGSIZE)))

static PDE kpdirs[NR_PDE] PG_ALIGN;
static PTE kptabs[PMEM_SIZE / PGSIZE] PG_ALIGN;
static void* (*palloc_f)();
static void (*pfree_f)(void*);

_Area segments[] = {      // Kernel memory mappings
  {.start = (void*)0,          .end = (void*)PMEM_SIZE}
};

#define NR_KSEG_MAP (sizeof(segments) / sizeof(segments[0]))

void _pte_init(void* (*palloc)(), void (*pfree)(void*)) {
  palloc_f = palloc;
  pfree_f = pfree;

  int i;

  // make all PDEs invalid
  for (i = 0; i < NR_PDE; i ++) {
    kpdirs[i] = 0;
  }

  PTE *ptab = kptabs;
  for (i = 0; i < NR_KSEG_MAP; i ++) {
    uint32_t pdir_idx = (uintptr_t)segments[i].start / (PGSIZE * NR_PTE);
    uint32_t pdir_idx_end = (uintptr_t)segments[i].end / (PGSIZE * NR_PTE);
    for (; pdir_idx < pdir_idx_end; pdir_idx ++) {
      // fill PDE
      kpdirs[pdir_idx] = (uintptr_t)ptab | PTE_P;

      // fill PTE
      PTE pte = PGADDR(pdir_idx, 0, 0) | PTE_P;
      PTE pte_end = PGADDR(pdir_idx + 1, 0, 0) | PTE_P;
      for (; pte < pte_end; pte += PGSIZE) {
        *ptab = pte;
        ptab ++;
      }
    }
  }

  set_cr3(kpdirs);
  set_cr0(get_cr0() | CR0_PG);
}

void _protect(_Protect *p) {
  PDE *updir = (PDE*)(palloc_f());
  p->ptr = updir;
  // map kernel space
  for (int i = 0; i < NR_PDE; i ++) {
    updir[i] = kpdirs[i];
  }

  p->area.start = (void*)0x8000000;
  p->area.end = (void*)0xc0000000;
}

void _release(_Protect *p) {
}

void _switch(_Protect *p) {
  set_cr3(p->ptr);
}

void _map(_Protect *p, void *va, void *pa) {
  PDE * pde = (PDE*)p->ptr;
  uint32_t dir = PDX(va); //find the directory`
  uint32_t page = PTX(va);
  pde += dir;
  PTE * pte;
  uint32_t present = *pde & PTE_P; // judge whether it's necessary to allocate a new page_table
  if (present == 0){ //present=0
    pte = (PTE*)(palloc_f());
	*pde = ((uint32_t)(pte) & ~0xfff) | PTE_P;
  }
  else {
	pte = (PTE* )((uint32_t)(*pde) & ~0xfff);
  }
  pte[page] = ((uint32_t)(pa) & ~0xfff) | PTE_P;
}

void _unmap(_Protect *p, void *va) {
}

_RegSet *_umake(_Protect *p, _Area ustack, _Area kstack, void *entry, char *const argv[], char *const envp[]) {
  int *stack = (int*)ustack.end; stack--;
  // stack frame of _start()
  *stack = 0x0; stack--;  //argc
  *stack = 0x0; stack--;  //argv
  *stack = 0x0; stack--;  //envp
  stack = NULL; stack--;  //start_ret_address
  // trap frame
  *stack = 0x202; stack--;  //eflags
  *stack = 0x8; stack--;  //cs
  *stack = (int)entry; stack--;  //eip
  *stack = 0x0; stack--;  //error_code
  *stack = 0x81; stack--;  //irq
  *stack = 0x0; stack--;  //eax
  *stack = 0x0; stack--;  //ecx
  *stack = 0x0; stack--;  //edx
  *stack = 0x0; stack--;  //ebx
  *stack = 0x0; stack--;  //esp
  *stack = (int)ustack.end; stack--;  //ebp
  *stack = 0x0; stack--;  //esi
  *stack = 0x0;   //edi

  return (_RegSet*)stack;
}
