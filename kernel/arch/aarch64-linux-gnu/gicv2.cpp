/*
    @file    gicv2.cpp

    ARM GICv2 and GICv2m (GICv2m is the MSI-X interrupt handler for GICv2)

*/

#include "string.h"
#include "stddef.h"
#include "gicv2.hpp"
#include "iofunc.hpp"
#include "logger.hpp"
#include "error.hpp"

// register address (qemu aarch64)
#define GICV2_DISTRIBUTOR_BASE (0x08000000)
#define GICV2_CPUINTERFACE_BASE (0x08010000)
#define GICV2M_BASE (0x08020000)

namespace interrupt::gicv2 {
  GICv2::GICv2(void):
    DistributorBaseAddress(GICV2_DISTRIBUTOR_BASE),
    CpuInterfaceBaseAddress(GICV2_CPUINTERFACE_BASE),
    GICv2mBaseAddress(GICV2M_BASE) {
  }

  void GICv2::setup(void) {
    Log(kDebug, "DistributorBaseAddress: %p\n", this->DistributorBaseAddress);
    Log(kDebug, "CpuInterfaceBaseAddress: %p\n", this->CpuInterfaceBaseAddress);
    Log(kDebug, "GICv2mBaseAddress: %p\n", this->GICv2mBaseAddress);

    uint32_t typer = 0;
    Error result = get_destribution_register("TYPER", &typer);
    if (result) {
      Log(kError, "Register read error: %s", result.Name());
      return;
    }
    this->int_num = (1 << (typer & 0b11111)) - 1;
    Log(kDebug, "TYPER = %08x, max_int:%d\n", typer, this->int_num);

    if (this->GICv2mBaseAddress != 0) {
      uint32_t msi_typer = 0;
      Error result = get_gicv2m_register("MSI_TYPER", &msi_typer);
      if (result) {
        Log(kError, "Register read error: %s", result.Name());
        return;
      }
      this->msi_base = (msi_typer >> 16) & 0b1111111111;
      this->msi_num = msi_typer & 0b111111111;
      Log(kDebug, "MSI_TYPER = %08x, base:%d num:%d\n", msi_typer, this->msi_base, this->msi_num);
    }

    // disable all interrupts
    for (uint64_t i = 0; i <= this->int_num; i++) {
      if (i % 2) {
        enable_interrput(i);
      } else {
        disable_interrput(i);
      }
    }
  }

  void GICv2::shutdown(void) {
  }

  void GICv2::register_handler(uint64_t i_num, uint64_t handler, InterruptControllerParam *param) {
  }

  void GICv2::unregister_handler(uint64_t i_num) {
  }

  Error GICv2::enable_interrput(uint64_t i_num) {
    if (i_num > this->int_num) {
      Log(kError, "interrupt number %s is not available\n", i_num);
      return MAKE_ERROR(Error::Code::kInvalidParameter);
    }

    uint64_t isenabler;
    int result = get_destribution_register_address("ISENABLER", &isenabler);
    if (result) {
      Log(kError, "%s:ISENABLER is not found (bug?)\n", __func__);
      return MAKE_ERROR(Error::Code::kInvalidParameter);
    }
    int isenabler_ofst = i_num / 32;
    int isenabler_bit = i_num % 32;
    uint32_t isenabler_value = 1 << isenabler_bit;
    io_write32(isenabler + (isenabler_ofst * 4), isenabler_value);
    return MAKE_ERROR(Error::Code::kSuccess);
  }

  Error GICv2::disable_interrput(uint64_t i_num) {
    if (i_num > this->int_num) {
      Log(kError, "interrupt number %s is not available\n", i_num);
      return MAKE_ERROR(Error::Code::kInvalidParameter);
    }

    uint64_t icenabler;
    int result = get_destribution_register_address("ICENABLER", &icenabler);
    if (result) {
      Log(kError, "%s:ICENABLER is not found (bug?)\n", __func__);
      return MAKE_ERROR(Error::Code::kSystemInternal);
    }
    int icenabler_ofst = i_num / 32;
    int icenabler_bit = i_num % 32;
    uint32_t icenabler_value = 1 << icenabler_bit;
    io_write32(icenabler + (icenabler_ofst * 4), icenabler_value);
    return MAKE_ERROR(Error::Code::kSuccess);
  }

  void GICv2::clear_interrupt(uint64_t i_num) {
  }

  // ********************************************************************************
  // architecture specific interfaces

  Error GICv2::get_destribution_register_address(const char *reg_name, uint64_t *reg_address) {
    if (reg_address == NULL) {
      Log(kError, "%s:reg_address is NULL\n", __func__);
      return MAKE_ERROR(Error::Code::kNull);
    }
    *reg_address = 0;
    for (int i= 0; i < (sizeof(reg::GICD) / sizeof(RegisterInfo)); i++) {
      if (strncmp(reg::GICD[i].name, reg_name, 32) == 0) {
        *reg_address = this->DistributorBaseAddress + reg::GICD[i].ofst;
        return MAKE_ERROR(Error::Code::kSuccess);
      }
    }
    Log(kError, "%s:reg_name %s is not found\n", __func__, reg_name);
    return MAKE_ERROR(Error::Code::kInvalidParameter);
  }

  Error GICv2::get_cpu_interface_register_address(const char *reg_name, uint64_t *reg_address) {
    if (reg_address == NULL) {
      Log(kError, "%s:reg_address is NULL\n", __func__);
      return MAKE_ERROR(Error::Code::kNull);
    }
    *reg_address = 0;
    for (int i= 0; i < (sizeof(reg::GICC) / sizeof(RegisterInfo)); i++) {
      if (strncmp(reg::GICC[i].name, reg_name, 32) == 0) {
        *reg_address = this->CpuInterfaceBaseAddress + reg::GICC[i].ofst;
        return MAKE_ERROR(Error::Code::kSuccess);
      }
    }
    Log(kError, "%s:reg_name %s is not found\n", __func__, reg_name);
    return MAKE_ERROR(Error::Code::kInvalidParameter);
  }

  Error GICv2::get_gicv2m_register_address(const char *reg_name, uint64_t *reg_address) {
    if (reg_address == NULL) {
      Log(kError, "%s:reg_address is NULL\n", __func__);
      return MAKE_ERROR(Error::Code::kNull);
    }
    *reg_address = 0;
    for (int i= 0; i < (sizeof(reg::GICv2M) / sizeof(RegisterInfo)); i++) {
      if (strncmp(reg::GICv2M[i].name, reg_name, 32) == 0) {
        *reg_address = this->GICv2mBaseAddress + reg::GICv2M[i].ofst;
        return MAKE_ERROR(Error::Code::kSuccess);
      }
    }
    Log(kError, "%s:reg_name %s is not found\n", __func__, reg_name);
    return MAKE_ERROR(Error::Code::kInvalidParameter);
  }

  Error GICv2::get_destribution_register(const char *reg_name, uint32_t *reg_value) {
    if (reg_value == NULL) {
      Log(kError, "%s:reg_address is NULL\n", __func__);
      return MAKE_ERROR(Error::Code::kNull);
    }
    *reg_value = 0;
    uint64_t address;
    Error result = get_destribution_register_address(reg_name, &address);
    if (result == 0) {
      *reg_value = io_read32(address);
    }
    return result;
  }

  Error GICv2::get_cpu_interface_register(const char *reg_name, uint32_t *reg_value) {
    if (reg_value == NULL) {
      Log(kError, "%s:reg_address is NULL\n", __func__);
      return MAKE_ERROR(Error::Code::kNull);
    }
    *reg_value = 0;
    uint64_t address;
    Error result = get_cpu_interface_register_address(reg_name, &address);
    if (result == 0) {
      *reg_value = io_read32(address);
    }
    return result;
  }

  Error GICv2::get_gicv2m_register(const char *reg_name, uint32_t *reg_value) {
    if (reg_value == NULL) {
      Log(kError, "%s:reg_address is NULL\n", __func__);
      return MAKE_ERROR(Error::Code::kNull);
    }
    *reg_value = 0;
    uint64_t address;
    Error result = get_gicv2m_register_address(reg_name, &address);
    if (result == 0) {
      *reg_value = io_read32(address);
    }
    return result;
  }

  void GICv2::get_information(void) {
    setup();
    Log(kDebug, "** GICv2:Distributor Registers %p **\n", this->DistributorBaseAddress);
    for (int i= 0; i < (sizeof(reg::GICD) / sizeof(RegisterInfo)); i++) {
      Log(kDebug, "%-10s: %08x\n", reg::GICD[i].name, io_read32(this->DistributorBaseAddress + reg::GICD[i].ofst));
    }

    int ret_val;

    uint32_t typer = 0;
    ret_val = get_destribution_register("TYPER", &typer);
    if (ret_val) {
      Log(kError, "Register read error: %d", ret_val);
      return;
    }
    uint32_t max_interrupt = (1 << (typer & 0b11111)) - 1;
    Log(kDebug, "TYPER = %08x, max_int:%d\n", typer, max_interrupt);
  }
}

