/**
 * @file interrupt.cpp
 *
 * 割り込み用のプログラムを集めたファイル．
 */

#include "interrupt.hpp"
#include "asmfunc.h"

// #@@range_begin(idt_array)
std::array<InterruptDescriptor, 256> idt;
// #@@range_end(idt_array)

// #@@range_begin(set_idt_entry)
void SetIDTEntry(InterruptDescriptor& desc,
                 InterruptDescriptorAttribute attr,
                 uint64_t offset,
                 uint16_t segment_selector) {
  desc.attr = attr;
  desc.offset_low = offset & 0xffffu;
  desc.offset_middle = (offset >> 16) & 0xffffu;
  desc.offset_high = offset >> 32;
  desc.segment_selector = segment_selector;
}
// #@@range_end(set_idt_entry)

// #@@range_begin(notify_eoi)
void NotifyEndOfInterrupt() {
  volatile auto end_of_interrupt = reinterpret_cast<uint32_t*>(0xfee000b0);
  *end_of_interrupt = 0;
}
// #@@range_end(notify_eoi)

void clear_interrupt(void) {
  NotifyEndOfInterrupt();
}

volatile void DisableIrq(void)
{
  __asm__("cli");
}

volatile void EnableIrq(void)
{
  __asm__("sti");
}

void ACPI::setup(void) {
}

void ACPI::shutdown(void) {
}

void ACPI::register_handler(InterruptDescriptor& desc,
    InterruptDescriptorAttribute attr,
    uint64_t offset) {
  const uint16_t cs = GetCS();
  SetIDTEntry(desc, attr, offset, cs);
  LoadIDT(sizeof(idt) - 1, reinterpret_cast<uintptr_t>(&idt[0]));

}

void ACPI::unregister_handler(void) {
}

void ACPI::clear_interrupt(void) {
  NotifyEndOfInterrupt();
}
