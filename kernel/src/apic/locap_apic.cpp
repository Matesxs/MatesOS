#include "madt.h"

namespace APIC
{
  uint32_t lapic_read(size_t reg)
  {
    return *((volatile uint32_t *)((uint64_t)madt_get_info()->local_apic_ptr + reg));
  }

  void lapic_write(size_t reg, uint32_t data)
  {
    *((volatile uint32_t *)((uint64_t)madt_get_info()->local_apic_ptr + reg)) = data;
  }

  void lapic_enable()
  {
    lapic_write(LAPIC_SPURIOUS_IVR, lapic_read(LAPIC_SPURIOUS_IVR) | (1 << 8) | 0xf0);
  }

  uint8_t lapic_get_id()
  {
    return (uint8_t) (lapic_read(LAPIC_APIC_ID) >> 24);
  }

  void lapic_ipi(uint8_t lapic_id, uint8_t vector) 
  {
    lapic_write(LAPIC_ICR1, (uint32_t) lapic_id << 24);
    lapic_write(LAPIC_ICR0, vector);
  }

  void lapic_eoi()
  {
    lapic_write(LAPIC_EOI, 0x00);
  }
}