#pragma once

#include "stdint.h"

namespace APIC
{
  struct MADTRecord
	{
		uint8_t type;
		uint8_t length;
	} __attribute__((packed));

	struct MADTLocalProcessor
	{
		MADTRecord record;
		uint8_t processor_id;
		uint8_t local_apic_id;
		uint32_t flags;
	} __attribute__((packed));

	struct MADTIOAPIC
	{
		MADTRecord record;
		uint8_t ioapic_id;
		uint8_t reserved;
		uint32_t ioapic_address;
		uint32_t global_interrupt_base;
	} __attribute__((packed));

	struct MADTInterruptSourceOverride
	{
		MADTRecord record;
		uint8_t bus; // Always 0 (ISA)
		uint8_t source;
		uint32_t global_interrupt;
		uint16_t flags;
	} __attribute__((packed));

	struct MADTNonMaskableInterrupt
	{
		MADTRecord record;
		uint8_t local_apic_id;
		uint16_t flags;
		uint8_t lint;
	} __attribute__((packed));

	struct MADTLocalAPICAddressOverride
	{
		MADTRecord record;
		uint8_t reserved[2];
		uint64_t local_apic_address;
	} __attribute__((packed));
}