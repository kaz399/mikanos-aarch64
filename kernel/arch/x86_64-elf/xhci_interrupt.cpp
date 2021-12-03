#include <usb/xhci/xhci.hpp>
#include "interrupt.hpp"
#include "asmfunc.h"
#include "pci.hpp"

extern usb::xhci::Controller* xhc;
extern InterruptController* intctrl;

__attribute__((interrupt))
void IntHandlerXHCI(InterruptFrame* frame) {
  while (xhc->PrimaryEventRing()->HasFront()) {
    if (auto err = ProcessEvent(*xhc)) {
      Log(kError, "Error while ProcessEvent: %s at %s:%d\n",
          err.Name(), err.File(), err.Line());
    }
  }
  intctrl->clear_interrupt();
}

int XHCIRegisterHandler(uint64_t IntHandlerXHCI) {
  intctrl->register_handler(idt[InterruptVector::kXHCI],
      MakeIDTAttr(DescriptorType::kInterruptGate, 0),
      reinterpret_cast<uint64_t>(IntHandlerXHCI));
  return 0;
}

int XHCIEnableMsi(pci::Device* xhc_dev) {
  const uint8_t bsp_local_apic_id =
    *reinterpret_cast<const uint32_t*>(0xfee00020) >> 24;
  pci::ConfigureMSIFixedDestination(
      *xhc_dev, bsp_local_apic_id,
      pci::MSITriggerMode::kLevel, pci::MSIDeliveryMode::kFixed,
      InterruptVector::kXHCI, 0);

  return 0;
}
