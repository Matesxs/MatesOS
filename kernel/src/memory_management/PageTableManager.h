#pragma once
#include "paging.h"
#include "stddef.h"
#include "../stivale/stivale2.h"

namespace memory
{
  class PageTableManager
  {
  public:
    PageTableManager();
    PageTableManager(stivale2_mmap_entry *memory_map, size_t memory_entries);
    PageTable *PML4 = NULL;

    void* MapMemory(void *virtualMemory, void *physicalMemory);
    void* IndentityMapMemory(void *address);
    void IndentityMapMemory(void *address, size_t size);
    void* WalkMemory(void *virtualMemory);
    void UnmapMemory(void *virtualMemory);
    void* RemapMemory(void *oldMemory, void *newMemory);
  };

  extern PageTableManager g_PageTableManager;
}