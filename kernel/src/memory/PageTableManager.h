#pragma once
#include "paging.h"

namespace memory
{
  class PageTableManager
  {
  public:
    PageTableManager(PageTable *PML4Address);
    PageTable *PML4;

    void* MapMemory(void *virtualMemory, void *physicalMemory);
    void* WalkMemory(void *virtualMemory);
    void UnmapMemory(void *virtualMemory);
    void* RemapMemory(void *oldMemory, void *newMemory);
  };

  extern PageTableManager g_PageTableManager;
}