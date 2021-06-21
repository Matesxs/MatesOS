#pragma once

#include <stdint.h>
#include "efiMemory.h"
#include "../utils/Bitmap.h"
#include "../memory_management/PageFrameAllocator.h"
#include "../memory_management/PageMapIndexer.h"
#include "../memory_management/paging.h"
#include "../memory_management/PageTableManager.h"
#include "../userinput/mouse.h"
#include "../acpi/acpi.h"
#include "../PCI/pci.h"
#include "driver.h"

struct BootInfo
{
	FrameBuffer* framebuffer;
	PSF1_FONT* psf1_Font;
	EFI_MEMORY_DESCRIPTOR* mMap;
	uint64_t mMapSize;
	uint64_t mMapDescSize;
	ACPI::RSDP2 *rsdp;
};

extern uint64_t _KernelStart;
extern uint64_t _KernelEnd;

void InitializeKernel(BootInfo* BootInfo);