#include "nemu.h"
#include "device/mmio.h"
#define PMEM_SIZE (128 * 1024 * 1024)

#define pmem_rw(addr, type) *(type *)({\
    Assert(addr < PMEM_SIZE, "physical address(0x%08x) is out of bound", addr); \
    guest_to_host(addr); \
    })

#define PGSIZE 0x1000

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
//  printf("addr:0x%x\tlen:%d\n",addr,len);
//  if (len + (addr & 0xfff) > PGSIZE) {
  if ((addr & ~0xfff) != ((addr+len-1) & ~0xfff)) {
	int len1 = PGSIZE - (addr & 0xfff);
	int len2 = len - len1;
	int ret1 = paddr_read(page_translate(addr), len1);
	int ret2 = paddr_read(page_translate(addr+len1), len2);
	printf("ret1:%d\tret2:%d\n",ret1,ret2);
	return ret1+(ret2 >> (8*len1));
  }
  return paddr_read(page_translate(addr), len);

}

void vaddr_write(vaddr_t addr, int len, uint32_t data) {
//  if (len + (addr & 0xfff) > PGSIZE) {
  if ((addr & ~0xfff) != ((addr+len-1) & ~0xfff)) {
	int len1 = PGSIZE - (addr & 0xfff);
	paddr_write(page_translate(addr), len1, data);
	int len2 = len - len1;
	data = data >> (8*len1);
	printf("data:%d\n",data);	
	paddr_write(page_translate(addr+len1), len2, data);
  }
  else
    paddr_write(page_translate(addr), len, data);

}

//static int num = 0;
paddr_t page_translate(vaddr_t addr){
  printf("addr: 0x%x\n",addr);
  if (cpu.cr0.paging == 1){

//	printf("num: %d\n",num);
//	printf("page translate begin\n");
//	printf("cpu.cr0: 0x%x\ncpu.cr3: 0x%x\n",cpu.cr0.val,cpu.cr3.val);
//    printf("addr: 0x%x\n",addr);
    PDE pde;
	PTE pte;
	vaddr_t ret_addr;
	uint32_t offset, dir, page;
    dir = ((addr >> 22) & 0x3ff) << 2;
	page = ((addr >> 12) & 0x3ff) << 2;
	offset = addr & 0xfff;
	ret_addr = cpu.cr3.val + dir;
	pde.val = paddr_read(ret_addr, 4);
	Assert(pde.present == 1,"now, addr:0x%x\n",addr);
    
	ret_addr = ((uint32_t)(pde.val) & ~0xfff) + page;
	pte.val = paddr_read(ret_addr, 4);
	Assert(pte.present == 1,"now, addr:0x%x\n",addr);

	ret_addr = ((uint32_t)(pte.val) & ~0xfff) | offset;

//	printf("page_translate end!\n");
//	num++;
	return ret_addr;
  }
  else{
//	printf("page translate skip\n");
	return addr;
  }
}
