#include "os_stats.h"
#include "../memory/PageFrameAllocator.h"
#include "../renderer/basic_renderer.h"

namespace osStats
{
  uint64_t totalRam;
  uint64_t freeRam;
  uint64_t usedRam;
  uint64_t reservedRam;
  uint64_t frameBufferAddr;
  uint64_t frameBufferSize;

  void updateOSStats()
  {
    freeRam = memory::g_Allocator.GetFreeMemory();
    reservedRam = memory::g_Allocator.GetReservedMemory();
    usedRam = memory::g_Allocator.GetUsedMemory();
    totalRam = memory::g_Allocator.GetTotalMemory();

    FrameBuffer fbuff = g_BasicRenderer.GetFramebuffer();
    frameBufferAddr = (uint64_t)fbuff.BaseAddress;
    frameBufferSize = fbuff.BufferSize;
  }
}