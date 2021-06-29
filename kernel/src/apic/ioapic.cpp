#include "ioapic.h"

namespace APIC
{
  uint32_t ioapic_read(uintptr_t ioapic_address, size_t reg)
  {
    *((volatile uint32_t *)(ioapic_address + IOAPIC_IOREGSEL)) = reg;
    return *((volatile uint32_t *)(ioapic_address + IOAPIC_IOWIN));
  }

  void ioapic_write(uintptr_t ioapic_address, size_t reg, uint32_t data)
  {
    *((volatile uint32_t *)(ioapic_address + IOAPIC_IOREGSEL)) = reg;
    *((volatile uint32_t *)(ioapic_address + IOAPIC_IOWIN)) = data;
  }
}