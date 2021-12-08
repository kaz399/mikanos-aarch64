#pragma once

#include "gicv2.hpp"

volatile void DisableIrq(void);
volatile void EnableIrq(void);
volatile void LoadVector(int el, uint64_t vector_base);

class InterruptVector {
 public:
  enum Number {
    kXHCI = 80,
  };
};


