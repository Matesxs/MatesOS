#pragma once
#include "paging.hpp"
#include "stddef.h"

namespace memory
{
  class PageTableManager
  {
  public:
    PageTableManager(PageTable *PML4Address);
    PageTable *PML4;

    void* MapMemory(void *virtualMemory, void *physicalMemory);
    void* IndentityMapMemory(void *address);
    void IndentityMapMemory(void *address, size_t size);
    void* WalkMemory(void *virtualMemory);
    void UnmapMemory(void *virtualMemory);
    void* RemapMemory(void *oldMemory, void *newMemory);
  };

  extern PageTableManager g_PageTableManager;
}