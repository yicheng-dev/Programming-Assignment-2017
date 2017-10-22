#include "common.h"
#include "syscall.h"
_RegSet* do_syscall(_RegSet *r) {
  uintptr_t a[4];
  int i=0;
  a[0] = SYSCALL_ARG1(r);
  a[1] = SYSCALL_ARG2(r);
  a[2] = SYSCALL_ARG3(r);
  a[3] = SYSCALL_ARG4(r);
/*  	
  printf("eax:0x%x;ecx:0x%x;edx:0x%x;ebx:0x%x;esi:0x%x;edi:0x%x;esp:0x%x;ebp:0x%x\n",r->eax,r->ecx,r->edx,r->ebx,r->esi,r->edi,r->esp,r->ebp);
*/
  switch (a[0]) {
	case SYS_none: a[0]=1; break;
	case SYS_exit: //printf("eax:0x%x; ecx:0x%x; edx:0x%x; ebx:0x%x\n",r->eax,r->ecx,r->edx,r->ebx);
				   _halt(a[1]);break;
	case SYS_write: //printf("eax:0x%x; ecx:0x%x; edx:0x%x; ebx:0x%x\n",r->eax,r->ecx,r->edx,r->ebx);break;
				   if (a[1]==1 || a[1]==2){
					   a[0]=0;
					   for (i=0;i<a[3];i++){
						   char *tmp=(char*)(a[2]);
						   _putc(tmp[i]);
						   a[0]++;
					   }

				   }
				   break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }

  return NULL;
}
