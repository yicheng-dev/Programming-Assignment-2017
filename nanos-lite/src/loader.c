#include "common.h"

#define DEFAULT_ENTRY ((void *)0x4000000)
int fs_open(const char* path);
ssize_t fs_read(int,void*,size_t);
int fs_close(int);
size_t fs_filesz(int); 
void ramdisk_read(void *buf,off_t offset, size_t len);
size_t get_ramdisk_size();
uintptr_t loader(_Protect *as, const char *filename) {
  int fd = fs_open(filename);
  size_t size = fs_filesz(fd);
  fs_read(fd,DEFAULT_ENTRY,size);
  
  
  return (uintptr_t)DEFAULT_ENTRY;
}
