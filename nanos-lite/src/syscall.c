#include "common.h"
#include "syscall.h"

extern uint32_t _end;

extern int fs_open(const char *pathname, int flags, int mode);
extern ssize_t fs_read(int fd, void *buf, size_t len);
extern ssize_t fs_write(int fd, const void *buf, size_t len);
extern off_t fs_lseek(int fd, off_t offset, int whence);
extern int fs_close(int fd);

_RegSet* do_syscall(_RegSet *r) {
  uintptr_t a[4];
  a[0] = SYSCALL_ARG1(r);
  a[1] = SYSCALL_ARG2(r);
  a[2] = SYSCALL_ARG3(r);
  a[3] = SYSCALL_ARG4(r);
  	
//  printf("eax:0x%x;ecx:0x%x;edx:0x%x;ebx:0x%x;esi:0x%x;edi:0x%x;esp:0x%x;ebp:0x%x\n",r->eax,r->ecx,r->edx,r->ebx,r->esi,r->edi,r->esp,r->ebp);

  switch (a[0]) {
	case SYS_none: SYSCALL_ARG1(r)=1; break;
	case SYS_exit: //printf("eax:0x%x; ecx:0x%x; edx:0x%x; ebx:0x%x\n",r->eax,r->ecx,r->edx,r->ebx);
				   _halt(SYSCALL_ARG2(r));break;
	case SYS_write: //printf("eax:0x%x; ecx:0x%x; edx:0x%x; ebx:0x%x\n",r->eax,r->ecx,r->edx,r->ebx);break;
				   if (SYSCALL_ARG2(r)==1 || SYSCALL_ARG2(r)==2){
					   SYSCALL_ARG1(r)=0;
					   for (; SYSCALL_ARG1(r) < SYSCALL_ARG4(r); SYSCALL_ARG1(r)++){
						   char *tmp=(char*)(SYSCALL_ARG3(r));
						   _putc(tmp[SYSCALL_ARG1(r)]);
					   }
				   }
				   else
				       SYSCALL_ARG1(r) = fs_write((int)SYSCALL_ARG2(r), (const void*)SYSCALL_ARG3(r), (size_t)SYSCALL_ARG4(r));
				   break;
	case SYS_brk:  _heap.end = (void*)SYSCALL_ARG2(r);//printf("end:0x%x\n",_heap.end); 
				   SYSCALL_ARG1(r)=0; 
				   break;
	case SYS_open: SYSCALL_ARG1(r) = fs_open((const char*)SYSCALL_ARG2(r),(int) SYSCALL_ARG3(r),(int) SYSCALL_ARG4(r));
				   break;
	case SYS_close:SYSCALL_ARG1(r) = fs_close((int)SYSCALL_ARG2(r));   
				   break;
	case SYS_read: SYSCALL_ARG1(r) = fs_read((int)SYSCALL_ARG2(r),(void*) SYSCALL_ARG3(r),(size_t) SYSCALL_ARG4(r));
				   break;
	case SYS_lseek:SYSCALL_ARG1(r) = fs_lseek((int)SYSCALL_ARG2(r), (off_t)SYSCALL_ARG3(r), (int)SYSCALL_ARG4(r));
				   break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }

  return NULL;
}
