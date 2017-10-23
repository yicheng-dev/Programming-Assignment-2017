#include "common.h"

#define DEFAULT_ENTRY ((void *)0x4000000)

extern void ramdisk_read(void *buf, off_t offset, size_t len);
extern size_t get_ramdisk_size();
extern int fs_open(const char*, int,int);
extern int fs_close(int);
extern ssize_t fs_read(int, void*, size_t);
extern ssize_t fs_write(int ,const void*, size_t);

extern size_t fs_filesz(int fd);

uintptr_t loader(_Protect *as, const char *filename) {
//  size_t ramdisk_size = get_ramdisk_size();
  void *buf = DEFAULT_ENTRY;
  int fd = fs_open(filename, 0, 0);
  fs_read(fd, buf, fs_filesz(fd));
  fs_close(fd);

  printf("filename:%s\nfd:%d\n",filename,fd);
  return (uintptr_t)DEFAULT_ENTRY;
}
