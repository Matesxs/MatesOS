#include "os_stats.h"
#include "../memory_management/PageFrameAllocator.h"
#include "../memory_management/heap.h"
#include "../renderer/basic_renderer.h"

namespace osStats
{
  uint64_t totalRam;
  uint64_t freeRam;
  uint64_t usedRam;
  uint64_t reservedRam;
  uint64_t frameBufferAddr;
  uint64_t frameBufferSize;
  uint64_t frameBufferWidth;
  uint64_t frameBufferHeigth;
  uint64_t heapPages;

  void updateOSStats()
  {
    freeRam = memory::g_Allocator.GetFreeMemory();
    reservedRam = memory::g_Allocator.GetReservedMemory();
    usedRam = memory::g_Allocator.GetUsedMemory();
    totalRam = memory::g_Allocator.GetTotalMemory();

    FrameBuffer fbuff = BasicRenderer::g_Renderer.GetFramebuffer();
    frameBufferAddr = (uint64_t)fbuff.BaseAddress;
    frameBufferSize = fbuff.BufferSize;
    frameBufferWidth = fbuff.Width;
    frameBufferHeigth = fbuff.Height;

    heapPages = memory::GetHeapPages();
  }
}