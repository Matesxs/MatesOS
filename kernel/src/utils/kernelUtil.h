#pragma once

#include <stdint.h>
#include "../utils/Bitmap.h"
#include "../memory_management/PageFrameAllocator.h"
#include "../memory_management/PageMapIndexer.h"
#include "../memory_management/paging.h"
#include "../memory_management/PageTableManager.h"
#include "../userinput/mouse.h"
#include "../acpi/acpi.h"
#include "../PCI/pci.h"
#include "../cpu/cpuInfo.h"
#include "driver.h"
#include "../stivale/stivale_main.h"

void InitializeKernel(stivale2_struct *bootloader_info);