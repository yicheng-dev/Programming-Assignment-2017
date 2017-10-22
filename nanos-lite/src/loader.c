#include "common.h"

#define DEFAULT_ENTRY ((void *)0x4000000)

extern void ramdisk_read(void *buf, off_t offset, size_t len);
extern size_t get_ramdisk_size();
extern int fs_open(const char*, int,int);
extern size_t fs_filesz(int fd);
extern off_t fs_fileof(int fd);

uintptr_t loader(_Protect *as, const char *filename) {
//  size_t ramdisk_size = get_ramdisk_size();
  void *buf = DEFAULT_ENTRY;
  int fd = fs_open(filename, 0, 0);
  ramdisk_read(buf, fs_fileof(fd), fs_filesz(fd));
//  printf("%s\n",filename);
  return (uintptr_t)DEFAULT_ENTRY;
}
