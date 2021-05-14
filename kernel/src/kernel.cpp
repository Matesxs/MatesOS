#include "utils/kernelUtil.h"
#include "renderer/basic_renderer.h"
#include "memory/heap.h"
#include "scheduling/pit/pit.h"
#include "utils/performance.h"
#include "utils/helpers.h"

extern "C" void _start(BootInfo *bootInfo)
{
  InitializeKernel(bootInfo);

  // Only things needed to run
  ShowStats(1100, 50);

  // Allocated some random memory
  void *mem = memory::malloc(sizeof(uint64_t) * 1000000);
  ShowStats(1100, 250);

  // Free that memory
  memory::free(mem);
  ShowStats(1100, 450);

  // Allocate random memory again
  mem = memory::malloc(sizeof(uint64_t) * 1000000);
  ShowStats(1100, 650);

  // And another one
  void *mem2 = memory::malloc(sizeof(uint32_t) * 200000);
  ShowStats(1100, 850);

  // Free both
  memory::free(mem);
  memory::free(mem2);
  ShowStats(1500, 50);

  halt();
}