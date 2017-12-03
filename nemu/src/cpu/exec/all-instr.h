#include "cpu/exec.h"
// data-mov
make_EHelper(mov);
make_EHelper(movsx);
make_EHelper(movzx);
make_EHelper(push);
make_EHelper(push_I8);
make_EHelper(pusha);
make_EHelper(pop);
make_EHelper(popa);
make_EHelper(leave);
make_EHelper(cltd);
make_EHelper(cwtl);
make_EHelper(lea);
make_EHelper(xchg);
make_EHelper(rol);

// arith
make_EHelper(add);
make_EHelper(inc);
make_EHelper(sub);
make_EHelper(dec);
make_EHelper(cmp);
make_EHelper(neg);
make_EHelper(adc);
make_EHelper(sbb);
make_EHelper(mul);
make_EHelper(imul1);
make_EHelper(imul2);
make_EHelper(imul3);
make_EHelper(div);
make_EHelper(idiv);

// logic
make_EHelper(not);
make_EHelper(and);
make_EHelper(or);
make_EHelper(xor);
make_EHelper(shl);
make_EHelper(shr);
make_EHelper(sar);
make_EHelper(setcc);
make_EHelper(test);
make_EHelper(clr_CF);

// control
make_EHelper(jmp);
make_EHelper(jmp_rm);
make_EHelper(jcc);
make_EHelper(call);
make_EHelper(call_rm);
make_EHelper(ret);
make_EHelper(ret_I);

//prefix
make_EHelper(operand_size);

//system
make_EHelper(in);
make_EHelper(out);
make_EHelper(int_);
make_EHelper(lidt);
make_EHelper(iret);

// special
make_EHelper(nop);
make_EHelper(inv);
make_EHelper(nemu_trap);
