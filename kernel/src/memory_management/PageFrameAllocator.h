#pragma once
#include <stdint.h>
#include "../utils/Bitmap.h"
#include "../stivale/stivale2.h"

namespace memory
{
  class PageFrameAllocator
  {
  public:
    void ReadMemoryMap(stivale2_mmap_entry *memory_map, size_t memory_entries);
    Bitmap PageBitmap;
    void FreePage(void *address);
    void FreePages(void *address, uint64_t pageCount);
    void LockPage(void *address);
    void LockPages(void *address, uint64_t pageCount);
    void ReservePage(void *address);
    void ReservePages(void *address, uint64_t pageCount);
    void UnreservePage(void *address);
    void UnreservePages(void *address, uint64_t pageCount);
    void *RequestPage();

    inline uint64_t GetTotalMemory() { return totalMemory; };
    inline uint64_t GetFreeMemory() { return freeMemory; };
    inline uint64_t GetReservedMemory() { return reservedMemory; };
    inline uint64_t GetUsedMemory() { return usedMemory; };

  private:
    void InitBitmap(size_t bitmapSize, void *bufferAddress);

    // bool Initialized = false;
    uint64_t totalMemory = 0;
    uint64_t freeMemory = 0;
    uint64_t reservedMemory = 0;
    uint64_t usedMemory = 0;
  };

  extern PageFrameAllocator g_Allocator;
}