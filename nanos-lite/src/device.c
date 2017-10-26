#include "common.h"

#define NAME(key) \
  [_KEY_##key] = #key,

static const char *keyname[256] __attribute__((used)) = {
  [_KEY_NONE] = "NONE",
  _KEYS(NAME)
};

size_t events_read(void *buf, size_t len) {
  char tem[128];
  int key = _read_key();
  int l;
  //Log("get in");
  if (key!=_KEY_NONE) {
	if (key >= 0x8000)
		sprintf(tem,"kd %s\n",keyname[key - 0x8000]);
    else sprintf(tem,"ku %s\n",keyname[key]);
	l = strlen(tem) < len ? strlen(tem) : len;
	memcpy(buf,tem,l);
	return l;
  }
  sprintf(tem,"t %d\n",_uptime());
  l = strlen(tem) < len ? strlen(tem) : len;
  memcpy(buf,tem,l);
  return l;
}

static char dispinfo[128] __attribute__((used));
void dispinfo_read(void *buf, off_t offset, size_t len) {
	memcpy(buf,dispinfo+offset,len);
}
extern uint32_t* const fb;
void fb_write(const void *buf, off_t offset, size_t len) {
	memcpy(fb+offset/4,buf,len);
}

void init_device() {
  _ioe_init();
  sprintf(dispinfo,"WIDTH:%d\nHEIGHT:%d",_screen.width,_screen.height);
  // TODO: print the string to array `dispinfo` with the format
  // described in the Navy-apps convention
}
