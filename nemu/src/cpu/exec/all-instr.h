#include "cpu/exec.h"

make_EHelper(mov);
make_EHelper(movsx);
make_EHelper(movzx);
make_EHelper(leave);
make_EHelper(cltd);
make_EHelper(call);
make_EHelper(call_rm);
make_EHelper(jmp);
make_EHelper(jmp_rm);
make_EHelper(jcc);
make_EHelper(push);
make_EHelper(pusha);
make_EHelper(pop);
make_EHelper(popa);
make_EHelper(xor);
make_EHelper(ret);
make_EHelper(ret_I);
make_EHelper(sub);
make_EHelper(adc);
make_EHelper(operand_size);
make_EHelper(nop);
make_EHelper(inv);
make_EHelper(nemu_trap);
make_EHelper(lea);
