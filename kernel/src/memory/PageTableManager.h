#pragma once
#include "paging.h"

namespace memory
{
  class PageTableManager
  {
  public:
    PageTableManager(PageTable *PML4Address);
    PageTable *PML4;
    void MapMemory(void *virtualMemory, void *physicalMemory);
  };

  extern PageTableManager g_PageTableManager;
}