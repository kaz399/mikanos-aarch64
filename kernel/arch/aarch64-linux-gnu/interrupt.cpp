#include "interrupt.hpp"
#include "logger.hpp"

volatile void DisableIrq(void)
{
  __asm__("msr daifset, #2");
}

volatile void EnableIrq(void)
{
  __asm__("msr daifclr, #2");
}

volatile void LoadVector(int el, uint64_t vector_base) {
    switch (el) {
      case 1:
        {
          __asm__("msr vbar_el1, %0" : : "r"(vector_base) : );
        }
        break;
      case 2:
        {
          __asm__("msr vbar_el2, %0" : : "r"(vector_base) : );
        }
        break;
      case 3:
        {
          __asm__("msr vbar_el3, %0" : : "r"(vector_base) : );
        }
        break;
      default:
        break;
    }
  }

extern "C" {
  volatile void IRQHandler(void) {
    Log(kDebug, "IRQ Hander\n");
  }
}
