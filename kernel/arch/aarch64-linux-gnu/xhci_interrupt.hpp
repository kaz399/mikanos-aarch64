#pragma once

#include <cstdint>
#include "gicv2.hpp"
#include "pci.hpp"

void IntHandlerXHCI(void);
int XHCIRegisterHandler(void (*handler)(void));
int XHCIEnableMsi(pci::Device* xhc_dev);
