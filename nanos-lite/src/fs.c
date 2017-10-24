#include "fs.h"

typedef struct {
  char *name;
  size_t size;
  off_t disk_offset;
  off_t open_offset;
} Finfo;

enum {FD_STDIN, FD_STDOUT, FD_STDERR, FD_FB, FD_EVENTS, FD_DISPINFO, FD_NORMAL};

/* This is the information about all files in disk. */
static Finfo file_table[] __attribute__((used)) = {
  {"stdin (note that this is not the actual stdin)", 0, 0},
  {"stdout (note that this is not the actual stdout)", 0, 0},
  {"stderr (note that this is not the actual stderr)", 0, 0},
  [FD_FB] = {"/dev/fb", 0, 0},
  [FD_EVENTS] = {"/dev/events", 0, 0},
  [FD_DISPINFO] = {"/proc/dispinfo", 128, 0},
#include "files.h"
};

#define NR_FILES (sizeof(file_table) / sizeof(file_table[0]))

int fs_open(const char*, int, int);
void init_fs() {
  // TODO: initialize the size of /dev/fb
  int fd = fs_open("/dev/fb",0,0);
  file_table[fd].size = _screen.width * _screen.height;
}

int fs_open(const char *pathname, int flags, int mode)
{
 // printf("fs_open: pathname:%s\n",pathname);
  int i;
  for (i=0; i<NR_FILES; i++){
	if (strcmp(pathname,file_table[i].name) == 0){
	  file_table[i].open_offset = 0;
	  return i;
	}
  }
  assert(0);
  return -1;
}

extern void ramdisk_read(void *, off_t, size_t);
extern void dispinfo_read(void *, off_t, size_t);
ssize_t fs_read(int fd, void *buf, size_t len)
{
	switch (fd) {
		case (FD_FB):  dispinfo_read(buf, file_table[FD_FB].disk_offset, len);
							 return len;
		default:
		if (len + file_table[fd].open_offset > file_table[fd].size)
			len = file_table[fd].size - file_table[fd].open_offset;
		if (len < 0 ) return -1;
		ramdisk_read(buf, file_table[fd].disk_offset+file_table[fd].open_offset, len);
		file_table[fd].open_offset += len;
		return len;
	}
}

static int cnt;
extern void ramdisk_write(const void *,off_t, size_t); 
extern void fb_write(const void *, off_t, size_t);

ssize_t fs_write(int fd, const void *buf, size_t len)
{
   switch (fd) {
	   case FD_STDOUT:
	   case FD_STDERR:cnt = 0;
					  for (; cnt < len; cnt++){
						  char *tmp = (char*)buf;
						  _putc(tmp[cnt]);
					  }
					  return cnt;
	   case FD_FB: fb_write(buf, file_table[FD_FB].disk_offset, len); return len;    
	   default: if (len + file_table[fd].open_offset > file_table[fd].size)
					len = file_table[fd].size - file_table[fd].open_offset;
				if (len < 0 ) return -1;
				ramdisk_write(buf, file_table[fd].disk_offset+file_table[fd].open_offset, len);
				file_table[fd].open_offset += len; 
				return len;
   }

}

off_t fs_lseek(int fd, off_t offset, int whence)
{
//  printf("fs_lseek begin, whence:%d ;fd:%d\n ;offset:%d\n",whence,fd,offset);
  switch (whence){
    case SEEK_SET: file_table[fd].open_offset = offset; break;
	case SEEK_CUR: file_table[fd].open_offset += offset; break;
	case SEEK_END: file_table[fd].open_offset = file_table[fd].size + offset;break;
	default: return -1;
  }

//  printf("fs_lseek end, open_offset:%d\n",file_table[fd].open_offset);
  return file_table[fd].open_offset;
}

int fs_close(int fd)
{
//  printf("fs_close end, fd:%d\n",fd);
  return 0;
}

size_t fs_filesz(int fd)
{
  return file_table[fd].size;
}

off_t fs_fileof(int fd)
{
  return file_table[fd].disk_offset;
}
