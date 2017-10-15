#include "common.h"

extern _RegSet* do_syscall(_RegSet *r);
static _RegSet* do_event(_Event e, _RegSet* r) {
  switch (e.event) {
	case _EVENT_IRQ_TIME: break;
	case _EVENT_IRQ_IODEV: break;
	case _EVENT_ERROR: break;
	case _EVENT_PAGE_FAULT: break;
	case _EVENT_BUS_ERROR: break;
	case _EVENT_NUMERIC: break;
	case _EVENT_TRAP: break;
	case _EVENT_SYSCALL: do_syscall(r); break;
    default: panic("Unhandled event ID = %d", e.event);
  }

  return NULL;
}

void init_irq(void) {
  _asye_init(do_event);
}
