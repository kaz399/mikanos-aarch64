/*
    @file    gicv2.cpp

    ARM GICv2 and GICv2m (GICv2m is the MSI-X interrupt handler for GICv2)

*/

#include "string.h"
#include "stddef.h"
#include "gicv2.hpp"
#include "interrupt.hpp"
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
    if (result.Cause() != Error::Code::kSuccess) {
      Log(kError, "Register read error: %s", result.Name());
      return;
    }
    this->int_num = (1 << (typer & 0b11111)) - 1;
    Log(kDebug, "TYPER = %08x, max_int:%d\n", typer, this->int_num);

    if (this->GICv2mBaseAddress != 0) {
      uint32_t msi_typer = 0;
      Error result = get_gicv2m_register("MSI_TYPER", &msi_typer);
      if (result.Cause() != Error::Code::kSuccess) {
        Log(kError, "Register read error: %s", result.Name());
        return;
      }
      this->msi_base = (msi_typer >> 16) & 0b1111111111;
      this->msi_num = msi_typer & 0b111111111;
      Log(kDebug, "MSI_TYPER = %08x, base:%d num:%d\n", msi_typer, this->msi_base, this->msi_num);
    }

    // GICD registers
    for (uint64_t i = 0; i <= this->int_num; i++) {
      disable_interrput(i);
      set_priority(i, 0x80);
      if (i >= 64) {
        // edge
        set_interrupt_type(i, 0b10);
      } else {
        // level
        set_interrupt_type(i, 0b00);
      }
      set_target_cpu(i, 1);
      if (i == InterruptVector::kXHCI) {
        enable_interrput(i);
      } else {
        disable_interrput(i);
      }
    }

    // GICC registers
    set_cpu_interface_register("PMR", 0xff);
    set_cpu_interface_register("BPR", 0x0);

    enable_gicv2();
    Log(kDebug, "GICv2 setup completed\n");
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
    Error result = get_destribution_register_address("ISENABLER", &isenabler);
    if (result.Cause() != Error::Code::kSuccess) {
      Log(kError, "%s:ISENABLER is not found (bug?)\n", __func__);
      return MAKE_ERROR(Error::Code::kSystemInternal);
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
    Error result = get_destribution_register_address("ICENABLER", &icenabler);
    if (result.Cause() != Error::Code::kSuccess) {
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

  Error GICv2::get_msi_adrs(uint32_t *adrs) {
    if (this->GICv2mBaseAddress == 0) {
      return MAKE_ERROR(Error::Code::kNotSupported);
    }
    if (adrs == NULL) {
      return MAKE_ERROR(Error::Code::kNull);
    }
    uint64_t adrs64;
    Error result = get_gicv2m_register_address("MSI_SETSPI_S", &adrs64);
    if (result.Cause() == Error::Code::kSuccess) {
      if (adrs64 > UINT32_MAX) {
        *adrs = 0;
        return MAKE_ERROR(Error::Code::kSystemInternal);
      } else {
        *adrs = static_cast<uint32_t>(adrs64);
      }
    }
    return result;

  }

  Error GICv2::get_msi_data(uint32_t *data, uint64_t i_num) {
    if (this->GICv2mBaseAddress == 0) {
      return MAKE_ERROR(Error::Code::kNotSupported);
    }
    if (data == NULL) {
      return MAKE_ERROR(Error::Code::kNull);
    }
    if (i_num > UINT32_MAX) {
      *data = 0;
      return MAKE_ERROR(Error::Code::kSystemInternal);
    } else {
      *data = static_cast<uint32_t>(i_num);
    }
    return MAKE_ERROR(Error::Code::kSuccess);
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

  Error GICv2::set_destribution_register(const char *reg_name, uint32_t reg_value) {
    uint64_t address;
    Error result = get_destribution_register_address(reg_name, &address);
    if (result.Cause() == Error::Code::kSuccess) {
      io_write32(address, reg_value);
    }
    return result;
  }

  Error GICv2::get_destribution_register(const char *reg_name, uint32_t *reg_value) {
    if (reg_value == NULL) {
      Log(kError, "%s:reg_address is NULL\n", __func__);
      return MAKE_ERROR(Error::Code::kNull);
    }
    *reg_value = 0;
    uint64_t address;
    Error result = get_destribution_register_address(reg_name, &address);
    if (result.Cause() == Error::Code::kSuccess) {
      *reg_value = io_read32(address);
    }
    return result;
  }

  Error GICv2::set_cpu_interface_register(const char *reg_name, uint32_t reg_value) {
    uint64_t address;
    Error result = get_cpu_interface_register_address(reg_name, &address);
    if (result.Cause() == Error::Code::kSuccess) {
      io_write32(address, reg_value);
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
    if (result.Cause() == Error::Code::kSuccess) {
      *reg_value = io_read32(address);
    }
    return result;
  }

  Error GICv2::set_gicv2m_register(const char *reg_name, uint32_t reg_value) {
    uint64_t address;
    Error result = get_gicv2m_register_address(reg_name, &address);
    if (result.Cause() == Error::Code::kSuccess) {
      io_write32(address, reg_value);
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
    if (result.Cause() == Error::Code::kSuccess) {
      *reg_value = io_read32(address);
    }
    return result;
  }

  Error GICv2::set_priority(uint64_t i_num, uint8_t priority) {
    if (i_num > this->int_num) {
      Log(kError, "interrupt number %s is not available\n", i_num);
      return MAKE_ERROR(Error::Code::kInvalidParameter);
    }
    uint64_t address;
    Error result = get_destribution_register_address("IPRIORITYR", &address);
    if (result.Cause() == Error::Code::kSuccess) {
      uint64_t ofst = i_num;
      io_write8(address + ofst, priority);
    }
    return result;
  }

  Error GICv2::get_priority(uint64_t i_num, uint8_t *priority) {
    if (i_num > this->int_num) {
      Log(kError, "interrupt number %s is not available\n", i_num);
      return MAKE_ERROR(Error::Code::kInvalidParameter);
    }
    if (priority == NULL) {
      Log(kError, "%s:priority is NULL\n", __func__);
      return MAKE_ERROR(Error::Code::kNull);
    }
    *priority = 0;
    uint64_t address;
    Error result = get_destribution_register_address("IPRIORITYR", &address);
    if (result.Cause() == Error::Code::kSuccess) {
      uint64_t ofst = i_num;
      *priority = io_read8(address + ofst);
    }
    return result;
  }

  Error GICv2::set_target_cpu(uint64_t i_num, uint8_t cpu) {
    if (i_num > this->int_num) {
      Log(kError, "interrupt number %s is not available\n", i_num);
      return MAKE_ERROR(Error::Code::kInvalidParameter);
    }
    uint64_t address;
    Error result = get_destribution_register_address("ITARGETSR", &address);
    if (result.Cause() == Error::Code::kSuccess) {
      uint64_t ofst = i_num;
      io_write8(address + ofst, cpu);
    }
    return result;
  }

  Error GICv2::get_target_cpu(uint64_t i_num, uint8_t *cpu) {
    if (i_num > this->int_num) {
      Log(kError, "interrupt number %s is not available\n", i_num);
      return MAKE_ERROR(Error::Code::kInvalidParameter);
    }
    if (cpu == NULL) {
      Log(kError, "%s:priority is NULL\n", __func__);
      return MAKE_ERROR(Error::Code::kNull);
    }
    *cpu = 0;
    uint64_t address;
    Error result = get_destribution_register_address("ITARGETSR", &address);
    if (result.Cause() == Error::Code::kSuccess) {
      uint64_t ofst = i_num;
      *cpu = io_read8(address + ofst);
    }
    return result;
  }

  void GICv2::enable_gicv2(void) {
    set_destribution_register("CTLR", 3);
    set_cpu_interface_register("CTLR", 0x1e7);
  }

  void GICv2::disable_gicv2(void) {
    uint64_t address;
    Error result = get_destribution_register_address("CTLR", &address);
    if (result.Cause() == Error::Code::kSuccess) {
      io_write32(address, 0x00000000);
    }
  }

  Error GICv2::set_interrupt_type(uint64_t i_num, uint8_t i_type) {
    if (i_num > this->int_num) {
      Log(kError, "interrupt number %s is not available\n", i_num);
      return MAKE_ERROR(Error::Code::kInvalidParameter);
    }
    uint64_t address;
    Error result = get_destribution_register_address("ICFGR", &address);
    if (result.Cause() == Error::Code::kSuccess) {
      uint64_t ofst = i_num / 16;
      uint64_t shift = (i_num % 16) * 2;
      uint32_t value = io_read32(address + (ofst * 4));
      value &= ~(0b11 << shift);
      value |= (i_type & 0b11) << shift;
      io_write32(address + (ofst * 4), value);
    }
    return result;
  }

  Error GICv2::set_interrupt_group(uint64_t i_num, uint8_t i_group) {
    if (i_num > this->int_num) {
      Log(kError, "interrupt number %s is not available\n", i_num);
      return MAKE_ERROR(Error::Code::kInvalidParameter);
    }
    uint64_t address;
    Error result = get_destribution_register_address("IGROUP", &address);
    if (result.Cause() == Error::Code::kSuccess) {
      uint64_t ofst = i_num / 32;
      uint64_t shift = i_num % 32;
      uint32_t value = io_read32(address + (ofst * 4));
      value &= ~(0b1 << shift);
      value |= (i_group & 0b1) << shift;
      io_write32(address + (ofst * 4), value);
    }
    return result;
  }

  // for development

  void GICv2::get_information(void) {
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

