#include "stivale/stivale_main.h"
#include "utils/kernelUtil.h"
#include "renderer/basic_renderer.h"
#include "memory_management/heap.h"
#include "scheduling/pit/pit.h"
#include "renderer/stat_logger.h"
#include "utils/helpers.h"
#include "library/memory.h"

#include "facp/facp.h"

extern "C" void _start(stivale2_struct *bootloader_info)
{
  InitializeKernel(bootloader_info);

  halt();
}