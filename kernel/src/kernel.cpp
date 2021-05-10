#include "utils/kernelUtil.h"
#include "renderer/basic_renderer.h"
#include "memory/heap.h"
#include "scheduling/pit/pit.h"
#include "utils/performance.h"

extern "C" void _start(BootInfo *bootInfo)
{
  InitializeKernel(bootInfo);

  ShowStats(1700, 50);

  while (true)
  {
    asm("hlt");
  }
}