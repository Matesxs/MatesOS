#pragma once

#include "madt_types.h"

namespace APIC
{
  struct IOAPIC 
	{
  	MADTIOAPIC*	ioapic;
  	uint8_t max_interrupts;
	};
}