/**
 * @file pci.cpp
 *
 * PCI バス制御のプログラムを集めたファイル．
 */

#include "pci.hpp"
#include <error.hpp>

#include "iofunc.hpp"

extern int printk(const char* format, ...);

namespace {
  using namespace pci;
  /** @brief CONFIG_ADDRESS 用の 32 ビット整数を生成する */
  pci_adrs_t MakeIoAddress(uint64_t base, uint8_t bus, uint8_t device,
                       uint8_t function, uint8_t reg_addr) {
    auto shl = [](uint64_t x, unsigned int bits) {
        return x << bits;
    };

    pci_adrs_t adrs = base
        | shl((bus & 0xffu), 16)
        | shl((device & 0x1fu), 11)
        | shl((function & 0x07u), 8)
        | (reg_addr & 0xfcu);
    printk("PCI IO ADRS:%0p\n", adrs);
    return adrs;
  }

  // address for ECAM
  pci_adrs_t MakeEcamAddress(uint64_t base, uint8_t bus, uint8_t device,
                       uint8_t function, uint16_t reg_addr) {
    auto shl = [](uint64_t x, unsigned int bits) {
        return x << bits;
    };

    pci_adrs_t adrs = base
        | shl((bus & 0xffu), 20)
        | shl((device & 0x1fu), 15)
        | shl((function & 0x07u), 12)
        | (reg_addr & 0xfffu);
    printk("PCI ECAM ADRS:%0p\n", adrs);
    return adrs;
  }

  // #@@range_begin(add_device)
  /** @brief devices[num_device] に情報を書き込み num_device をインクリメントする． */
  Error AddDevice(const Device& device) {
    if (num_device == devices.size()) {
      return MAKE_ERROR(Error::kFull);
    }

    devices[num_device] = device;
    ++num_device;
    return MAKE_ERROR(Error::kSuccess);
  }
  // #@@range_end(add_device)

  Error ScanBus(uint8_t bus);

  // #@@range_begin(scan_function)
  /** @brief 指定のファンクションを devices に追加する．
   * もし PCI-PCI ブリッジなら，セカンダリバスに対し ScanBus を実行する
   */
  Error ScanFunction(uint8_t bus, uint8_t device, uint8_t function) {
    auto class_code = ReadClassCode(bus, device, function);
    auto header_type = ReadHeaderType(bus, device, function);
    Device dev{bus, device, function, header_type, class_code};
    if (auto err = AddDevice(dev)) {
      return err;
    }

    if (class_code.Match(0x06u, 0x04u)) {
      // standard PCI-PCI bridge
      auto bus_numbers = ReadBusNumbers(bus, device, function);
      uint8_t secondary_bus = (bus_numbers >> 8) & 0xffu;
      return ScanBus(secondary_bus);
    }

    return MAKE_ERROR(Error::kSuccess);
  }
  // #@@range_end(scan_function)

  // #@@range_begin(scan_device)
  /** @brief 指定のデバイス番号の各ファンクションをスキャンする．
   * 有効なファンクションを見つけたら ScanFunction を実行する．
   */
  Error ScanDevice(uint8_t bus, uint8_t device) {
    if (auto err = ScanFunction(bus, device, 0)) {
      return err;
    }
    if (IsSingleFunctionDevice(ReadHeaderType(bus, device, 0))) {
      return MAKE_ERROR(Error::kSuccess);
    }

    for (uint8_t function = 1; function < 8; ++function) {
      if (ReadVendorId(bus, device, function) == 0xffffu) {
        continue;
      }
      if (auto err = ScanFunction(bus, device, function)) {
        return err;
      }
    }
    return MAKE_ERROR(Error::kSuccess);
  }
  // #@@range_end(scan_device)

  // #@@range_begin(scan_bus)
  /** @brief 指定のバス番号の各デバイスをスキャンする．
   * 有効なデバイスを見つけたら ScanDevice を実行する．
   */
  Error ScanBus(uint8_t bus) {
    for (uint8_t device = 0; device < 32; ++device) {
      if (ReadVendorId(bus, device, 0) == 0xffffu) {
        continue;
      }
      if (auto err = ScanDevice(bus, device)) {
        return err;
      }
    }
    return MAKE_ERROR(Error::kSuccess);
  }
  // #@@range_end(scan_bus)
}

namespace pci {
  void WriteIoConfig(pci_adrs_t address, uint32_t data) {
      io_write32(address, data);
  }

  uint32_t ReadIoConfig(pci_adrs_t address) {
      return io_read32(address);
  }

  void WriteEcamConfig(pci_adrs_t address, uint32_t data) {
      io_write32(address, data);
  }

  uint32_t ReadEcamConfig(pci_adrs_t address) {
      return io_read32(address);
  }

  uint16_t ReadVendorId(uint8_t bus, uint8_t device, uint8_t function) {
      return ReadEcamConfig(MakeEcamAddress(EcamBaseAddress, bus, device, function, 0x00)) & 0xffffu;
  }
  // #@@range_end(config_addr_data)

  uint16_t ReadDeviceId(uint8_t bus, uint8_t device, uint8_t function) {
      return ReadEcamConfig(MakeEcamAddress(EcamBaseAddress, bus, device, function, 0x00)) >> 16;
  }

  uint8_t ReadHeaderType(uint8_t bus, uint8_t device, uint8_t function) {
    return (ReadEcamConfig(MakeEcamAddress(EcamBaseAddress, bus, device, function, 0x0c)) >> 16) & 0xffu;
  }

  ClassCode ReadClassCode(uint8_t bus, uint8_t device, uint8_t function) {
    auto reg = ReadEcamConfig(MakeEcamAddress(EcamBaseAddress, bus, device, function, 0x08));
    ClassCode cc;
    cc.base       = (reg >> 24) & 0xffu;
    cc.sub        = (reg >> 16) & 0xffu;
    cc.interface  = (reg >> 8)  & 0xffu;
    return cc;
  }

  uint32_t ReadBusNumbers(uint8_t bus, uint8_t device, uint8_t function) {
    return ReadEcamConfig(MakeEcamAddress(EcamBaseAddress, bus, device, function, 0x18));
  }

  bool IsSingleFunctionDevice(uint8_t header_type) {
    return (header_type & 0x80u) == 0;
  }

  Error ScanAllBus() {
    num_device = 0;

    auto header_type = ReadHeaderType(0, 0, 0);
    if (IsSingleFunctionDevice(header_type)) {
      return ScanBus(0);
    }

    for (uint8_t function = 0; function < 8; ++function) {
      if (ReadVendorId(0, 0, function) == 0xffffu) {
        continue;
      }
      if (auto err = ScanBus(function)) {
        return err;
      }
    }
    return MAKE_ERROR(Error::kSuccess);
  }

  uint32_t ReadConfReg(const Device& dev, uint8_t reg_addr) {
    return ReadEcamConfig(MakeEcamAddress(EcamBaseAddress, dev.bus, dev.device, dev.function, reg_addr));
  }

  void WriteConfReg(const Device& dev, uint8_t reg_addr, uint32_t value) {
    return WriteEcamConfig(MakeEcamAddress(EcamBaseAddress, dev.bus, dev.device, dev.function, reg_addr), value);
  }

  WithError<uint64_t> ReadBar(Device& device, unsigned int bar_index) {
    if (bar_index >= 6) {
      return {0, MAKE_ERROR(Error::kIndexOutOfRange)};
    }

    const auto addr = CalcBarAddress(bar_index);
    const auto bar = ReadConfReg(device, addr);

    // 32 bit address
    if ((bar & 4u) == 0) {
      return {bar, MAKE_ERROR(Error::kSuccess)};
    }

    // 64 bit address
    if (bar_index >= 5) {
      return {0, MAKE_ERROR(Error::kIndexOutOfRange)};
    }

    const auto bar_upper = ReadConfReg(device, addr + 4);
    return {
      bar | (static_cast<uint64_t>(bar_upper) << 32),
      MAKE_ERROR(Error::kSuccess)
    };
  }
}
