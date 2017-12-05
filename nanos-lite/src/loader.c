#include "common.h"

#define DEFAULT_ENTRY ((void *)0x8048000)

extern void ramdisk_read(void *buf, off_t offset, size_t len);
extern size_t get_ramdisk_size();
extern int fs_open(const char*, int,int);
extern int fs_close(int);
extern ssize_t fs_read(int, void*, size_t);
extern ssize_t fs_write(int ,const void*, size_t);

extern size_t fs_filesz(int fd);
extern void* new_page(void);

uintptr_t loader(_Protect *as, const char *filename) {
  void *va = DEFAULT_ENTRY;
  void *pa;
//  size_t ramdisk_size = get_ramdisk_size();
//  void *buf = DEFAULT_ENTRY;
  int fd = fs_open(filename, 0, 0);
  int page = 0;
  int page_num = fs_filesz(fd)/PGSIZE;
  while (page <= page_num){
    pa = new_page();
	if ((uint32_t)(va) + PGSIZE <= fs_filesz(fd))
		fs_read(fd, pa, PGSIZE);
	else
		fs_read(fd, pa, fs_filesz(fd)-(uint32_t)va);
	_map(as, va, pa);
    page++;
	va += PGSIZE;
  }
  fs_close(fd);
//  printf("filename:%s\nfd:%d\n",filename,fd);
  return (uintptr_t)DEFAULT_ENTRY;
}
