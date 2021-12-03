#pragma once

#include <cstdint>

void IntHandlerXHCI(void* frame);
int XHCIRegisterHandler(uint64_t IntHandlerXHCI);
int XHCIEnableMsi(pci::Device* xhc_dev);
