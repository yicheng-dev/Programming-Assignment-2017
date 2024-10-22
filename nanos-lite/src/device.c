#include "common.h"

#define NAME(key) \
  [_KEY_##key] = #key,

static const char *keyname[256] __attribute__((used)) = {
  [_KEY_NONE] = "NONE",
  _KEYS(NAME)
};

extern off_t fs_fileof(int);
//static char event_temp[10000] __attribute__((used));
size_t events_read(void *buf, size_t len) {
  char events[100];
  int key = _read_key();
  bool down =false;
  if (key & 0x8000) {
	key = key ^ 0x8000;
    down =true;
  }
  
  if (key != _KEY_NONE){
//	printf("key:%d\n",key);
	if (down)
	  sprintf(events, "kd %s\n", keyname[key]);
	else
	  sprintf(events, "ku %s\n", keyname[key]);
  }
  else {
    sprintf(events, "t %d\n", _uptime());
  }
  if (len > strlen(events)) len = strlen(events);
  memcpy(buf, (void*)events, len);
//  printf("buf:%s\n",buf);
//  printf("strlen(buf):%d\n",strlen(buf));
  return len;
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
  memcpy((void*)fb+offset, buf, len);
}

extern int fs_open(const char *, int, int);
extern ssize_t fs_write(int, const void*, size_t);
extern int fs_close(int);
extern size_t fs_filesz(int);
extern void ramdisk_read(void *,off_t, size_t);
extern ssize_t fs_read(int, void*, size_t);
extern unsigned long _uptime();
extern int _read_key();
void init_device() {
  _ioe_init(); 
  sprintf(dispinfo, "WIDTH:%d\nHEIGHT:%d", _screen.width, _screen.height);
  // TODO: print the string to array `dispinfo` with the format
  // described in the Navy-apps convention
}
