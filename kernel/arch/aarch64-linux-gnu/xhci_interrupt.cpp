#include <usb/xhci/xhci.hpp>
#include "interrupt.hpp"
#include "asmfunc.h"
#include "pci.hpp"
#include "logger.hpp"

extern usb::xhci::Controller* xhc;
extern InterruptController* intctrl;

void IntHandlerXHCI(void *param) {
  while (xhc->PrimaryEventRing()->HasFront()) {
    if (auto err = ProcessEvent(*xhc)) {
      Log(kError, "Error while ProcessEvent: %s at %s:%d\n",
          err.Name(), err.File(), err.Line());
    }
  }
  intctrl->clear_interrupt(0);
}


int XHCIRegisterHandler(uint64_t IntHandlerXHCI) {
  intctrl->register_handler(100,
      IntHandlerXHCI,
      NULL);
  return 0;
}

int XHCIEnableMsi(pci::Device* xhc_dev) {
  return 0;
}
