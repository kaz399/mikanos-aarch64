/**
    @file    gicv2.cpp

    ARM GICv2

*/

#include "gicv2.hpp"
#include "iofunc.hpp"
#include "logger.hpp"

namespace interrupt::gicv2 {
  GICv2::GICv2(void): DistributorBaseAddress(0x08000000), CpuInterfaceBaseAddress(0x08010000) {
    Log(kDebug, "DistributorBaseAddress: %p\n", this->DistributorBaseAddress);
    Log(kDebug, "CpuInterfaceBaseAddress: %p\n", this->CpuInterfaceBaseAddress);
  }

  void GICv2::setup(void) {
    Log(kDebug, "DistributorBaseAddress: %p\n", this->DistributorBaseAddress);
    Log(kDebug, "CpuInterfaceBaseAddress: %p\n", this->CpuInterfaceBaseAddress);
  }

  void GICv2::shutdown(void) {
  }

  void GICv2::register_handler(uint64_t i_num, uint64_t handler, InterruptControllerParam *param) {
  }

  void GICv2::unregister_handler(uint64_t i_num) {
  }

  void GICv2::clear_interrupt(uint64_t i_num) {
  }

  void GICv2::dump_registers(void) {
    setup();
    Log(kDebug, "** GICv2:Distributor Registers %p **\n", this->DistributorBaseAddress);
    for (int i= 0; i < (sizeof(reg::GICD) / sizeof(RegisterInfo)); i++) {
      Log(kDebug, "%-10s: %08x\n", reg::GICD[i].name, io_read32(this->DistributorBaseAddress + reg::GICD[i].ofst));
    }
  }
}

