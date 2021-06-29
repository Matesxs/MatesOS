#pragma once

#include "stddef.h"
#include "madt_types.h"

#define IOAPIC_IOREGSEL 0x00
#define IOAPIC_IOWIN 0x10

namespace APIC
{
  struct IOAPIC 
	{
  	MADTIOAPIC*	ioapic;
  	uint8_t max_interrupts;
	};

	uint32_t ioapic_read(uintptr_t ioapic_address, size_t reg);
	void ioapic_write(uintptr_t ioapic_address, size_t reg, uint32_t data);
}