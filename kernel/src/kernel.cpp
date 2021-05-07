#include "utils/kernelUtil.h"
#include "renderer/basic_renderer.h"
#include "memory/heap.h"
#include "scheduling/pit/pit.h"

extern "C" void _start(BootInfo *bootInfo)
{
  InitializeKernel(bootInfo);

  while (true)
  {
    asm("hlt");
  }
}