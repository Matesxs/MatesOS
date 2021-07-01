#include "PageFrameAllocator.hpp"
#include "../renderer/stat_logger.hpp"
#include "../utils/panic.hpp"
#include "../library/string.hpp"

namespace memory
{
  bool Initialized = false;
  PageFrameAllocator g_Allocator;

  void PageFrameAllocator::ReadEFIMemoryMap(EFI_MEMORY_DESCRIPTOR *mMap, size_t mMapSize, size_t mMapDescSize)
  {
    if (Initialized) return;
    Initialized = true;

    showInfo("Reading EFI Memory Map");

    if (mMap == nullptr || mMapSize == 0 || mMapDescSize == 0 || \
        mMapDescSize > mMapSize)
    {
      Panic("PageFrameAllocator get invalid EFI_MEMORY_DESCRIPTOR");
    }

    uint64_t mMapEntries = mMapSize / mMapDescSize;

    void *largestFreeMemSeg = NULL;
    size_t largestFreeMemSegSize = 0;

    for (uint64_t i = 0; i < mMapEntries; i++)
    {
      EFI_MEMORY_DESCRIPTOR *desc = (EFI_MEMORY_DESCRIPTOR *)((uint64_t)mMap + (i * mMapDescSize));
      if (desc->type == EfiConventionalMemory)
      {
        if (desc->numPages * 4096 > largestFreeMemSegSize)
        {
          largestFreeMemSeg = desc->physAddr;
          largestFreeMemSegSize = desc->numPages * 4096;
        }
      }
    }

    if (largestFreeMemSeg == NULL) Panic("Failed to get free memory segment from EFI memory descriptor");

    totalMemory = GetMemorySize(mMap, mMapEntries, mMapDescSize);
    freeMemory = totalMemory;
    uint64_t bitmapSize = totalMemory / 4096 / 8 + 1;

    InitBitmap(bitmapSize, largestFreeMemSeg);

    uint64_t totalPages = totalMemory / 4096 + 1;
    ReservePages(0, totalPages);

    printStatsSpacing();
    printStats("Found ");
    printStats(to_string(totalPages));
    printStats(" free memory pages");
    statNewLine();

    for (uint64_t i = 0; i < mMapEntries; i++)
    {
      EFI_MEMORY_DESCRIPTOR *desc = (EFI_MEMORY_DESCRIPTOR *)((uint64_t)mMap + (i * mMapDescSize));
      if (desc->type == EfiConventionalMemory)
      {
        UnreservePages(desc->physAddr, desc->numPages);
      }
    }

    ReservePages(0, 0x100);
    LockPages(PageBitmap.Buffer, PageBitmap.Size / 4096 + 1);
  }

  void PageFrameAllocator::InitBitmap(size_t bitmapSize, void *bufferAddress)
  {
    PageBitmap.Size = bitmapSize;
    PageBitmap.Buffer = (uint8_t *)bufferAddress;
    for (size_t i = 0; i < bitmapSize; i++)
    {
      *(uint8_t *)(PageBitmap.Buffer + i) = 0;
    }
  }

  uint64_t pageBitmapIndex = 0;
  void *PageFrameAllocator::RequestPage()
  {
    for (; pageBitmapIndex < PageBitmap.Size * 8; pageBitmapIndex++)
    {
      if (PageBitmap[pageBitmapIndex] == true)
        continue;
      LockPage((void *)(pageBitmapIndex * 4096));
      return (void *)(pageBitmapIndex * 4096);
    }

    return NULL; // Page Frame Swap to file
  }

  void PageFrameAllocator::FreePage(void *address)
  {
    uint64_t index = (uint64_t)address / 4096;
    if (PageBitmap[index] == false)
      return;

    if (PageBitmap.Set(index, false))
    {
      freeMemory += 4096;
      usedMemory -= 4096;
      if (pageBitmapIndex > index)
        pageBitmapIndex = index;
    }
  }

  void PageFrameAllocator::FreePages(void *address, uint64_t pageCount)
  {
    for (uint64_t t = 0; t < pageCount; t++)
    {
      FreePage((void *)((uint64_t)address + (t * 4096)));
    }
  }

  void PageFrameAllocator::LockPage(void *address)
  {
    uint64_t index = (uint64_t)address / 4096;
    if (PageBitmap[index] == true)
      return;

    if (PageBitmap.Set(index, true))
    {
      freeMemory -= 4096;
      usedMemory += 4096;
    }
  }

  void PageFrameAllocator::LockPages(void *address, uint64_t pageCount)
  {
    for (uint64_t t = 0; t < pageCount; t++)
    {
      LockPage((void *)((uint64_t)address + (t * 4096)));
    }
  }

  void PageFrameAllocator::UnreservePage(void *address)
  {
    uint64_t index = (uint64_t)address / 4096;
    if (PageBitmap[index] == false)
      return;
    if (PageBitmap.Set(index, false))
    {
      freeMemory += 4096;
      reservedMemory -= 4096;
      if (pageBitmapIndex > index)
        pageBitmapIndex = index;
    }
  }

  void PageFrameAllocator::UnreservePages(void *address, uint64_t pageCount)
  {
    for (uint64_t t = 0; t < pageCount; t++)
    {
      UnreservePage((void *)((uint64_t)address + (t * 4096)));
    }
  }

  void PageFrameAllocator::ReservePage(void *address)
  {
    uint64_t index = (uint64_t)address / 4096;
    if (PageBitmap[index] == true)
      return;
    if (PageBitmap.Set(index, true))
    {
      freeMemory -= 4096;
      reservedMemory += 4096;
    }
  }

  void PageFrameAllocator::ReservePages(void *address, uint64_t pageCount)
  {
    for (uint64_t t = 0; t < pageCount; t++)
    {
      ReservePage((void *)((uint64_t)address + (t * 4096)));
    }
  }
}