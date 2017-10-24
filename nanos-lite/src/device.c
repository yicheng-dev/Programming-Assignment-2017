#include "common.h"

#define NAME(key) \
  [_KEY_##key] = #key,

static const char *keyname[256] __attribute__((used)) = {
  [_KEY_NONE] = "NONE",
  _KEYS(NAME)
};

size_t events_read(void *buf, size_t len) {
      
  return 0;
}

static char dispinfo[128] __attribute__((used));

void dispinfo_read(void *buf, off_t offset, size_t len) {
//  printf("fb_read: offset:%d\nlen:%d\n",offset,len);
  memcpy(buf, (void*)dispinfo+offset, len);
//  printf("buf:%s\n",buf);
}

extern uint32_t* const fb;
void fb_write(const void *buf, off_t offset, size_t len) {
//  printf("fb_write: offset:%d\nlen:%d\n",offset,len);
  memcpy(((void*)fb)+offset, buf, len);
}

extern int fs_open(const char *, int, int);
extern ssize_t fs_write(int, const void*, size_t);
extern int fs_close(int);
extern size_t fs_filesz(int);
extern off_t fs_fileof(int);
extern void ramdisk_read(void *,off_t, size_t);
extern ssize_t fs_read(int, void*, size_t);
void init_device() {
  _ioe_init();
  static char * buf;
  sprintf(buf, "t 1234\nkd RETURN\nku A\n");
  int fd = fs_open("/dev/events",0 ,0);
  fs_write(fd, (void*)buf, sizeof((void*)buf));
  sprintf(dispinfo, "WIDTH:%d\nHEIGHT:%d", _screen.width, _screen.height);
  // TODO: print the string to array `dispinfo` with the format
  // described in the Navy-apps convention
}
