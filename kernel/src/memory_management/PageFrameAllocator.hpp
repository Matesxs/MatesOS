#pragma once
#include "../utils/efiMemory.hpp"
#include <stdint.h>
#include "../utils/Bitmap.hpp"

namespace memory
{
  class PageFrameAllocator
  {
  public:
    void ReadEFIMemoryMap(EFI_MEMORY_DESCRIPTOR *mMap, size_t mMapSize, size_t mMapDescSize);
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
    uint64_t totalMemory;
    uint64_t freeMemory;
    uint64_t reservedMemory;
    uint64_t usedMemory;
  };

  extern PageFrameAllocator g_Allocator;
}