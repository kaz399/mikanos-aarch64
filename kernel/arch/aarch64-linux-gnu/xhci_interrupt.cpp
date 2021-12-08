#include <usb/xhci/xhci.hpp>
#include "interrupt.hpp"
#include "asmfunc.h"
#include "pci.hpp"
#include "logger.hpp"

extern usb::xhci::Controller* xhc;
extern InterruptController* intctrl;

void IntHandlerXHCI(void) {
  while (xhc->PrimaryEventRing()->HasFront()) {
    if (auto err = ProcessEvent(*xhc)) {
      Log(kError, "Error while ProcessEvent: %s at %s:%d\n",
          err.Name(), err.File(), err.Line());
    }
  }
  intctrl->clear_interrupt(0);
}


int XHCIRegisterHandler(interrupt::gicv2::InterruptHandler IntHandlerXHCI) {
  Log(kDebug, "register XHCI interrupt hander\n");
  intctrl->register_handler(InterruptVector::kXHCI,
      IntHandlerXHCI,
      NULL);
  return 0;
}

int XHCIEnableMsi(pci::Device* xhc_dev) {
  Log(kDebug, "Enable MSI\n");
  uint32_t adrs;
  uint32_t data;
  Error result_adrs = intctrl->get_msi_adrs(&adrs);
  if (result_adrs.Cause() != Error::Code::kSuccess) {
    Log(kError, "%s:%s\n", __func__, result_adrs.Name());
    return -1;
  }
  Error result_data = intctrl->get_msi_data(&data, InterruptVector::kXHCI);
  if (result_data.Cause() != Error::Code::kSuccess) {
    Log(kError, "%s:%s\n", __func__, result_data.Name());
    return -1;
  }
  Error result = pci::ConfigureMSI(*xhc_dev, adrs, data, 0);
  Log(kDebug, "EnableMSI:result:%s\n", result.Name());
  return 0;
}
