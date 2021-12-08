/**
    @file    gicv2.hpp

    ARM GICv2

*/

#pragma once

#include <cstdint>
#include <string>
#include "error.hpp"

#define GICV2_MAX_INT (1024)

namespace interrupt::gicv2 {
  typedef struct {
    uint8_t priority;
    uint8_t target_cpu;
  } InterruptControllerParam;

  typedef struct {
    const char *name;
    uint64_t ofst;
  } RegisterInfo;

  typedef void InterruptHandler(void);

  namespace reg {
    const RegisterInfo GICD[] = {
      {"CTLR", 0x000},
      {"TYPER", 0x004},
      {"IIDR", 0x008},
      {"IGROUP", 0x080},
      {"ISENABLER", 0x100},
      {"ICENABLER", 0x180},
      {"ISPENDR", 0x200},
      {"ICPENDR", 0x280},
      {"ISACTIVER", 0x300},
      {"ICACTIVER", 0x380},
      {"IPRIORITYR", 0x400},
      {"ITARGETSR", 0x800},
      {"ICFGR", 0xc00},
      {"NSACR", 0xe00},
      {"SGIR", 0xf00},
      {"CPENDSGIR", 0xf10},
      {"SPENDSGIR", 0xf20},
      {"ID", 0xfd0},
    };

    const RegisterInfo GICC[] = {
      {"CTLR", 0x0000},
      {"PMR", 0x0004},
      {"BPR", 0x0008},
      {"IAR", 0x000c},
      {"EOIR", 0x0010},
      {"RPR", 0x0014},
      {"HPPIR", 0x0018},
      {"ABPR", 0x001c},
      {"AIAR", 0x0020},
      {"AEOIR", 0x0024},
      {"AHPPIR", 0x0028},
      {"APR", 0x00d0},
      {"NSAPR", 0x00e0},
      {"IIDR", 0x00fc},
      {"DIR", 0x1000},
    };

    const RegisterInfo GICv2M[] = {
      {"MSI_TYPER", 0x008},
      {"MSI_SETSPI_S", 0x040},
      {"MSI_IIDR", 0xfcc},
    };
  }

  class GICv2 {
    private:
      const uint64_t DistributorBaseAddress;
      const uint64_t CpuInterfaceBaseAddress;
      const uint64_t GICv2mBaseAddress;
      uint32_t int_num;
      uint32_t msi_base;
      uint32_t msi_num;
      InterruptHandler *handler[GICV2_MAX_INT];

    public:
      GICv2(void);

      // common interfaces

      void setup(void);
      void shutdown(void);
      void register_handler(uint64_t i_num,
          InterruptHandler *handler,
          interrupt::gicv2::InterruptControllerParam *param);
      void unregister_handler(uint64_t i_num);
      Error enable_interrput(uint64_t i_num);
      Error disable_interrput(uint64_t i_num);
      void clear_interrupt(uint64_t i_num);
      Error get_msi_adrs(uint32_t *adrs);
      Error get_msi_data(uint32_t *data, uint64_t i_num);
      void run_handler(uint64_t i_num);

      // architecture specific interfaces

      Error get_destribution_register_address(const char *reg_name, uint64_t *reg_address);
      Error get_cpu_interface_register_address(const char *reg_name, uint64_t *reg_address);
      Error get_gicv2m_register_address(const char *reg_name, uint64_t *reg_address);

      Error set_destribution_register(const char *reg_name, uint32_t reg_value);
      Error get_destribution_register(const char *reg_name, uint32_t *reg_value);
      Error set_cpu_interface_register(const char *reg_name, uint32_t reg_value);
      Error get_cpu_interface_register(const char *reg_name, uint32_t *reg_value);
      Error set_gicv2m_register(const char *reg_name, uint32_t reg_value);
      Error get_gicv2m_register(const char *reg_name, uint32_t *reg_value);

      Error set_priority(uint64_t i_num, uint8_t priority);
      Error get_priority(uint64_t i_num, uint8_t *priority);

      Error set_target_cpu(uint64_t i_num, uint8_t cpu);
      Error get_target_cpu(uint64_t i_num, uint8_t *cpu);

      Error set_interrupt_type(uint64_t i_num, uint8_t i_type);

      Error set_interrupt_group(uint64_t i_num, uint8_t i_group);

      void enable_gicv2(void);
      void disable_gicv2(void);

      // for development

      void get_information(void);
  };


}

typedef interrupt::gicv2::GICv2 InterruptController;

