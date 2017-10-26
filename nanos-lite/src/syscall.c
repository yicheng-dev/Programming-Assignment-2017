#include "common.h"
#include "syscall.h"
uintptr_t sys_none();
int fs_open(const char*);
void sys_exit(uintptr_t);
ssize_t sys_write(int,const void*i,size_t);
uint32_t sys_brk(uint32_t);
int sys_open(const char*);
int fs_open(const char*);
ssize_t fs_write(int,const void*,size_t);
ssize_t fs_read(int,void*,size_t);
off_t fs_lseek(int,off_t,int);
int fs_close(int);
_RegSet* do_syscall(_RegSet *r) {
  uintptr_t a[4];
  a[0] = SYSCALL_ARG1(r);
  a[1] = SYSCALL_ARG2(r);
  a[2] = SYSCALL_ARG3(r);
  a[3] = SYSCALL_ARG4(r);
  switch (a[0]) { 
	case SYS_none:
        SYSCALL_ARG1(r) = sys_none();
		break;
	case SYS_exit:
		sys_exit(a[1]);
		break;
	case SYS_write:
		SYSCALL_ARG1(r) = fs_write(a[1],(void*)a[2],a[3]);
		break;
	case SYS_brk:
        SYSCALL_ARG1(r) = sys_brk(a[1]);
        break;
	case SYS_open:
		SYSCALL_ARG1(r) = fs_open((char*)a[1]);
		break;
	case SYS_read:
		SYSCALL_ARG1(r) = fs_read(a[1],(void*)a[2],a[3]);
		break;
	case SYS_lseek:
		SYSCALL_ARG1(r) = fs_lseek(a[1],a[2],a[3]);
		break;
	case SYS_close:
		SYSCALL_ARG1(r) = fs_close(a[1]);
		break;
    default: panic("Unhandled syscall ID = %d", a[0]);
   }
  return NULL;
}
uintptr_t sys_none(){
  return 1;
}
void sys_exit(uintptr_t code){
	_halt(code);
}

uint32_t sys_brk(uint32_t brk) {
	return 0;
}




