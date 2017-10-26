#include "fs.h"

typedef struct {
  char *name;
  size_t size;
  off_t disk_offset;
  off_t open_offset;
} Finfo;
void ramdisk_write(const void* ,off_t,size_t );
void ramdisk_read(void*,off_t,size_t);
void fb_write(const void*,off_t,size_t);
void dispinfo_read(void*,off_t,size_t);
size_t events_read(void*,size_t);
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

void init_fs() {
  // TODO: initialize the size of /dev/fb
  file_table[FD_FB].size = _screen.width * _screen.height * 4;  
}
int fs_open (const char* path) {
	for (int i = 0 ;  i < NR_FILES;i++) {
		if (strcmp(path,file_table[i].name) == 0){
			file_table[i].open_offset = 0; 
			return i;
		}
	}
	panic("Should not reach here\n");
	return -1;

}
ssize_t fs_write(int fd,const void*buf, size_t count) {
	const char *p = buf;
	switch (fd) {
	case 1:
	case 2:
		for (uint32_t i = 0; i < count;i++)
			_putc(p[i]);
		return count;
	case FD_FB:
		if (file_table[FD_FB].open_offset + count > 
				file_table[FD_FB].size)
			count = file_table[FD_FB].size -
				file_table[FD_FB].open_offset;
		fb_write(buf,file_table[FD_FB].open_offset,count);

		file_table[FD_FB].open_offset+=count;
		return count;
	default:
		if (file_table[fd].open_offset + count > 
				file_table[fd].size)
			count = file_table[fd].size -
				file_table[fd].open_offset;
		
	ramdisk_write(buf,file_table[fd].open_offset+file_table[fd].disk_offset,count);
	file_table[fd].open_offset+=count;
	return count;
	
	}
}
ssize_t fs_read(int fd,void *buf, size_t count) {
	switch (fd){ 
	case FD_DISPINFO:
		if (file_table[FD_DISPINFO].open_offset + count > 
				file_table[FD_DISPINFO].size)
			count = file_table[FD_DISPINFO].size -
				file_table[FD_DISPINFO].open_offset;

		dispinfo_read(buf,file_table[FD_DISPINFO].open_offset,count);
		file_table[FD_DISPINFO].open_offset+=count;
		return count;
	case FD_FB:
		panic("Unimplemented");
		return -1;
	case FD_EVENTS:
		return events_read(buf,count);
	default:
		if (file_table[fd].open_offset + count > 
				file_table[fd].size)
			count = file_table[fd].size -
				file_table[fd].open_offset;
	ramdisk_read(buf,file_table[fd].open_offset+ file_table[fd].disk_offset,count);
	file_table[fd].open_offset +=count;
	return count;
	}
}
off_t fs_lseek(int fd ,off_t offset,int whence){
	switch(whence)  {
		case 0:
			file_table[fd].open_offset = offset;
			break;
		case 1:
			file_table[fd].open_offset = file_table[fd].open_offset+offset;
			break;
		case 2:
			file_table[fd].open_offset = file_table[fd].size + offset;
			break;
		default:
			panic("Invalid parameter!\n");
			return -1;
	
	}
	file_table[fd].open_offset = file_table[fd].open_offset < file_table[fd].size ? file_table[fd].open_offset : file_table[fd].size;
   return file_table[fd].open_offset;


}
size_t fs_filesz(int fd) {
	return file_table[fd].size;

}
int fs_close(int fd) {
	return 0;
}
