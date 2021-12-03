#pragma once

// #ifdef AARCH64_HAVE_GICv2
#include "gicv2.hpp"
// #endif

volatile void DisableIrq(void);
volatile void EnableIrq(void);

