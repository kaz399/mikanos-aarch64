/**
    @file    exception_handlers.cpp

*/

#include "exception_handlers.hpp"
#include "error.hpp"
#include "interrupt.hpp"
#include "gicv2.hpp"
#include "logger.hpp"

extern InterruptController *intctrl;

extern "C" {
  volatile void SyncHandler(unsigned int elr, unsigned int info) {
    Log(kDebug, "%d:ELR %08x\n", info, elr);
  }

  volatile void IRQHandler(unsigned int elr, unsigned int info) {
    Log(kDebug, "%d:IRQ start %08x\n", info, elr);
    uint32_t irq_number;
    intctrl->get_cpu_interface_register("IAR", &irq_number);
    Log(kDebug, "IRQ %d\n", irq_number);
    intctrl->run_handler(irq_number);
    intctrl->set_cpu_interface_register("EOIR", irq_number);
    Log(kDebug, "IRQ end\n");
  }

  volatile void FIQHandler(unsigned int elr, unsigned int info) {
    Log(kDebug, "%d:FIQ start %08x\n", info, elr);
  }

  volatile void SerrorHandler(unsigned int elr, unsigned int info) {
    Log(kDebug, "%d:SERROR start %08x\n", info, elr);
  }
}
