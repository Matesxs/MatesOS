#pragma once

#include <stdint.h>
#include "efiMemory.hpp"
#include "../utils/Bitmap.hpp"
#include "../memory_management/PageFrameAllocator.hpp"
#include "../memory_management/PageMapIndexer.hpp"
#include "../memory_management/paging.hpp"
#include "../memory_management/PageTableManager.hpp"
#include "../userinput/mouse.hpp"
#include "../acpi/acpi.hpp"
#include "../PCI/pci.hpp"
#include "../cpu/cpuInfo.hpp"
#include "driver.hpp"

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
extern uint64_t _WritableStart;
extern uint64_t _WritableEnd;

void InitializeKernel(BootInfo* BootInfo);