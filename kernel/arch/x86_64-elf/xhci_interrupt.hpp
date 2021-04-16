#pragma once

#include <cstdint>

void IntHandlerXHCI(InterruptFrame* frame);
int XHCIRegisterHandler(uint64_t IntHandlerXHCI);
int XHCIEnableMsi(pci::Device* xhc_dev);
