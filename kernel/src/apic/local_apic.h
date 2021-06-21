#pragma once

#include "madt_types.h"
#include "stddef.h"

#define LOCAL_APIC_TRAMPOLINE_TARGET		0x8000
#define LOCAL_APIC_TRAMPOLINE_TARGET_SIZE	4096

namespace APIC
{
  struct LocalAPICProcessor {
	  MADTLocalProcessor* local_processor;
	  void* stack_ptr;
	  size_t stack_size;
  };

  // Initializes the bootstrap processor
  void local_apic_init();
}