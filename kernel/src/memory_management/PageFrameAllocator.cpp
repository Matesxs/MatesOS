#include "PageFrameAllocator.h"
#include "../renderer/stat_logger.h"
#include "../utils/panic.h"
#include "../library/string.h"
#include "../utils/helpers.h"
#include "memory_entry.h"

namespace memory
{
  bool Initialized = false;
  PageFrameAllocator g_Allocator;
  Bitmap g_pageframemap;

  void PageFrameAllocator::ReadMemoryMap(stivale2_mmap_entry *memory_map, size_t memory_entries)
  {
    if (Initialized)
      return;
    Initialized = true;

    void *largestMemory = NULL;
    size_t largestMemorySize = 0;

    // Find largest primary memory region
    for (size_t i = 0; i < memory_entries; i++)
    {
      stivale2_mmap_entry memorymap_entry = memory_map[i];

      if (memorymap_entry.type == STIVALE2_MMAP_USABLE ||
           memorymap_entry.type == STIVALE2_MMAP_BOOTLOADER_RECLAIMABLE)
      {
        if (largestMemory == NULL || memorymap_entry.length > largestMemorySize)
        {
          largestMemory = (void *)memorymap_entry.base;
          largestMemorySize = memorymap_entry.length;
        }
      }
    }

    totalMemory = GetMemorySize(memory_map, memory_entries);
    bitmap_initialize(&g_pageframemap, largestMemory, totalMemory / 4096 / BITMAP_SCALE);
    ReservePages(0, NEAREST_PAGE(totalMemory));

    for (size_t i = 0; i < memory_entries; i++)
    {
      stivale2_mmap_entry memorymap_entry = memory_map[i];
      if (memorymap_entry.type != STIVALE2_MMAP_USABLE &&
          memorymap_entry.type != STIVALE2_MMAP_BOOTLOADER_RECLAIMABLE)
      {
        continue;
      }

      UnreservePages((void *)memorymap_entry.base, NEAREST_PAGE(memorymap_entry.length));
    }

    // Lock the s_pageframemap itself
    size_t pageframemap_pages = NEAREST_PAGE(g_pageframemap.size);
    LockPages(g_pageframemap.buffer, pageframemap_pages);

    // Lock kernel space
    size_t kernel_size = (size_t)&_KernelEnd - (size_t)&_KernelStart;
    size_t kernel_pages = NEAREST_PAGE(kernel_size);
    LockPages(KERNEL_PHYSICAL_ADDRESS(&_KernelStart), kernel_pages);

    // Reserve first 256 pages
    ReservePages(0, 256);
  }

  uint64_t pageBitmapIndex = 0;
  void *PageFrameAllocator::RequestPage()
  {
    for (; pageBitmapIndex < bitmap_adjusted_size(&g_pageframemap); pageBitmapIndex++)
    {
      if (bitmap_get(&g_pageframemap, pageBitmapIndex) == true)
      {
        continue;
      }
      void *page = (void *)(pageBitmapIndex * 4096);
      
      pageBitmapIndex++;
      LockPage(page);

      return page;
    }

    return NULL; // Page Frame Swap to file
  }

  bool pageframe_manipulate(uint64_t index, bool state)
  {
    if (bitmap_get(&g_pageframemap, index) == state)
    {
      return true;
    }
    return bitmap_set(&g_pageframemap, index, state);
  }

  void PageFrameAllocator::FreePage(void *address)
  {
    uint64_t index = (uint64_t)address / 4096;
    if (pageframe_manipulate(index, false))
    {
      if (usedMemory >= 4096) usedMemory -= 4096;
      freeMemory += 4096;

      if (pageBitmapIndex > index)
      {
        pageBitmapIndex = index;
      }
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
    if (pageframe_manipulate(index, true))
    {
      usedMemory += 4096;
      if (freeMemory >= 4096) freeMemory -= 4096;
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
    if (pageframe_manipulate(index, false))
    {
      if (reservedMemory >= 4096) reservedMemory -= 4096;
      freeMemory += 4096;

      if (pageBitmapIndex > index)
      {
        pageBitmapIndex = index;
      }
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
    if (pageframe_manipulate(index, true))
    {
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