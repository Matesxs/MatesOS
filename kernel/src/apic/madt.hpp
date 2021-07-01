#pragma once

#include "stddef.h"
#include "stdint.h"
#include "madt_types.hpp"
#include "ioapic.hpp"
#include "local_apic.hpp"
#include "../acpi/acpi.hpp"

#define MADT_MAX_PROCESSORS 128
#define MADT_MAX_IO_APICS 8
#define MADT_MAX_INTERRUPT_SOURCE_OVERRIDES 16
#define MADT_MAX_NONMASKABLE_INTERRUPTS 8

namespace APIC
{
	enum MADTType
	{
		MADT_TYPE_LOCAL_PROCESSOR = 0,
		MADT_TYPE_IOAPIC = 1,
		MADT_TYPE_INTERRUPT_SOURCE_OVERRIDE = 2,
		MADT_TYPE_NONMASKABLE_INTERRUPT = 4,
		MADT_TYPE_LOCAL_APIC_ADDRESS_OVERRIDE = 5,
		MADT_TYPE_IO_SAPIC = 6,
		MADT_TYPE_LOCAL_SAPIC = 7,
		MADT_TYPE_PLAT_INTERRUPT_SRC = 8,
		MADT_TYPE_PROCESSOR_LOCAL_X2APIC = 9,
		MADT_TYPE_LOCAL_X2APIC_NMI = 10,
		MADT_TYPE_GIC_CPU_INTERFACE = 11,
		MADT_TYPE_GIC_DISTRIBUTOR = 12,
		MADT_TYPE_GIC_MSI_FRAME = 13,
		MADT_TYPE_GIC_REDISTRIBUTOR = 14,
		MADT_TYPE_GIC_INT_TRANSLATION = 15,
		MADT_TYPE_MULTIPROCESSOR_WAKEUP = 16
	};

	struct MADTInformation
	{
		void *local_apic_ptr;
		LocalAPICProcessor processors[MADT_MAX_PROCESSORS];
		size_t processors_num;
		IOAPIC ioapics[MADT_MAX_IO_APICS];
		size_t ioapics_num;
		MADTInterruptSourceOverride *isos[MADT_MAX_INTERRUPT_SOURCE_OVERRIDES];
		size_t isos_num;
		MADTNonMaskableInterrupt *nmis[MADT_MAX_NONMASKABLE_INTERRUPTS];
		size_t nmis_num;
	};

	bool MADTInit(ACPI::MADTHeader *madt);

	// getters
	ACPI::MADTHeader *madt_get();
	MADTInformation *madt_get_info();
}