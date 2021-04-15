#include "interrupt.hpp"

volatile void DisableIrq(void)
{
  __asm__("msr daifset, #2");
}

volatile void EnableIrq(void)
{
  __asm__("msr daifclr, #2");
}


