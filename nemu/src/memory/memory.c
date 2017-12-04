#include "nemu.h"
#include "device/mmio.h"
#define PMEM_SIZE (128 * 1024 * 1024)

#define pmem_rw(addr, type) *(type *)({\
    Assert(addr < PMEM_SIZE, "physical address(0x%08x) is out of bound", addr); \
    guest_to_host(addr); \
    })

uint8_t pmem[PMEM_SIZE];
paddr_t page_translate(vaddr_t addr);

/* Memory accessing interfaces */

uint32_t paddr_read(paddr_t addr, int len) {
  if (is_mmio(addr)==-1)
	return pmem_rw(addr, uint32_t) & (~0u >> ((4 - len) << 3));
  else
	return mmio_read(addr, len, is_mmio(addr));
}

void paddr_write(paddr_t addr, int len, uint32_t data) {
  if (is_mmio(addr)==-1)
	memcpy(guest_to_host(addr), &data, len);
  else
	mmio_write(addr, len, data, is_mmio(addr));
}


uint32_t vaddr_read(vaddr_t addr, int len) {
//  if (特殊情况)
	
  paddr_t paddr = page_translate(addr);
  return paddr_read(paddr, len);

}

void vaddr_write(vaddr_t addr, int len, uint32_t data) {
  // if (特殊情况)
  paddr_t paddr = page_translate(addr);
  paddr_write(paddr, len, data);

}

static int translate_num = 0; //调试用

paddr_t page_translate(vaddr_t addr){
  if (translate_num < 10000){
    translate_num++;
	printf("cr0.val: 0x%x\n",cpu.cr0.val);
  }
  if (cpu.cr0.paging == 1){
	printf("page translate begin\n");

    PDE pde;
	PTE pte;
	vaddr_t ret_addr;
	uint32_t offset, dir, page;
    dir = ((addr >> 22) & 0x3ff) << 2;
	page = ((addr >> 12) & 0x3ff) << 2;
	offset = addr & 0xfff;
	ret_addr = (cpu.cr3.page_directory_base << 12) + dir;
	pde.val = paddr_read(ret_addr, 4);
	Assert(pde.present == 1,"now, present:0x%x\n",pde.present);
    
	ret_addr = (pde.page_frame << 12) + page;
	pte.val = paddr_read(ret_addr, 4);
	Assert(pte.present == 1,"now, present:0x%x\n",pte.present);

	ret_addr = (pte.page_frame << 12) | offset;

	printf("page_translate end!\n");
	return ret_addr;
  }
  else{
//	printf("page translate skip\n");
	return addr;
  }

}
