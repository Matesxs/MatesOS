#include <stdint.h>
#include <stddef.h>
#include "PageTableManager.hpp"
#include "PageMapIndexer.hpp"
#include "PageFrameAllocator.hpp"
#include "../library/memory.hpp"

#define ROUND_UP(n, multiple)	((n) % (multiple) == 0 ? (n) : (n) - (n) % (multiple) + (multiple))

namespace memory
{
  PageTableManager g_PageTableManager = NULL;

  PageTableManager::PageTableManager(PageTable *PML4Address)
  {
    this->PML4 = PML4Address;
  }

  void* PageTableManager::MapMemory(void *virtualMemory, void *physicalMemory)
  {
    PageMapIndexer indexer = PageMapIndexer((uint64_t)virtualMemory);
    PageDirectoryEntry PDE;

    PDE = PML4->entries[indexer.PDP_i];
    PageTable *PDP;
    if (!PDE.GetFlag(PT_Flag::Present))
    {
      PDP = (PageTable *)g_Allocator.RequestPage();
      memset(PDP, 0, 0x1000);
      PDE.SetAddress((uint64_t)PDP >> 12);
      PDE.SetFlag(PT_Flag::Present, true);
      PDE.SetFlag(PT_Flag::ReadWrite, true);
      PML4->entries[indexer.PDP_i] = PDE;
    }
    else
    {
      PDP = (PageTable *)((uint64_t)PDE.GetAddress() << 12);
    }

    PDE = PDP->entries[indexer.PD_i];
    PageTable *PD;
    if (!PDE.GetFlag(PT_Flag::Present))
    {
      PD = (PageTable *)g_Allocator.RequestPage();
      memset(PD, 0, 0x1000);
      PDE.SetAddress((uint64_t)PD >> 12);
      PDE.SetFlag(PT_Flag::Present, true);
      PDE.SetFlag(PT_Flag::ReadWrite, true);
      PDP->entries[indexer.PD_i] = PDE;
    }
    else
    {
      PD = (PageTable *)((uint64_t)PDE.GetAddress() << 12);
    }

    PDE = PD->entries[indexer.PT_i];
    PageTable *PT;
    if (!PDE.GetFlag(PT_Flag::Present))
    {
      PT = (PageTable *)g_Allocator.RequestPage();
      memset(PT, 0, 0x1000);
      PDE.SetAddress((uint64_t)PT >> 12);
      PDE.SetFlag(PT_Flag::Present, true);
      PDE.SetFlag(PT_Flag::ReadWrite, true);
      PD->entries[indexer.PT_i] = PDE;
    }
    else
    {
      PT = (PageTable *)((uint64_t)PDE.GetAddress() << 12);
    }

    PDE = PT->entries[indexer.P_i];
    PDE.SetAddress((uint64_t)physicalMemory >> 12);
    PDE.SetFlag(PT_Flag::Present, true);
    PDE.SetFlag(PT_Flag::ReadWrite, true);
    PT->entries[indexer.P_i] = PDE;

    return virtualMemory;
  }

  void* PageTableManager::IndentityMapMemory(void *address)
  {
    return MapMemory(address, address);
  }

  void PageTableManager::IndentityMapMemory(void *address, size_t size)
  {
    size = ROUND_UP(size, 0x1000);
    for(void* ptr = address; (uint64_t)ptr < (uint64_t)address + size; ptr = (void*)((uint64_t)ptr + 0x1000)) 
    {
		  MapMemory(ptr, ptr);
	  }
  }

  void* PageTableManager::WalkMemory(void *virtualMemory)
  {
    PageMapIndexer indexer = PageMapIndexer((uint64_t)virtualMemory);
    PageDirectoryEntry PDE;

    PDE = PML4->entries[indexer.PDP_i];
    PageTable *PDP;

    if (!PDE.GetFlag(PT_Flag::Present))
      return NULL;

    PDP = (PageTable *)((uint64_t)PDE.GetAddress() << 12);
    PDE = PDP->entries[indexer.PD_i];
    PageTable *PD;

    if (!PDE.GetFlag(PT_Flag::Present))
      return NULL;

    PD = (PageTable *)((uint64_t)PDE.GetAddress() << 12);
    PDE = PD->entries[indexer.PT_i];
    PageTable *PT;

    if (!PDE.GetFlag(PT_Flag::Present))
      return NULL;

    PT = (PageTable *)((uint64_t)PDE.GetAddress() << 12);
    PDE = PT->entries[indexer.P_i];

    return (void*)((uint64_t)PDE.GetAddress() << 12);
  }

  void PageTableManager::UnmapMemory(void *virtualMemory)
  {
    PageMapIndexer indexer = PageMapIndexer((uint64_t)virtualMemory);
    PageDirectoryEntry PDE;

    PDE = PML4->entries[indexer.PDP_i];
    PageTable *PDP;

    if (!PDE.GetFlag(PT_Flag::Present))
      return;

    PDP = (PageTable *)((uint64_t)PDE.GetAddress() << 12);
    PDE = PDP->entries[indexer.PD_i];
    PageTable *PD;

    if (!PDE.GetFlag(PT_Flag::Present))
      return;

    PD = (PageTable *)((uint64_t)PDE.GetAddress() << 12);
    PDE = PD->entries[indexer.PT_i];
    PageTable *PT;

    if (!PDE.GetFlag(PT_Flag::Present))
      return;

    PT = (PageTable *)((uint64_t)PDE.GetAddress() << 12);

    PDE = PT->entries[indexer.P_i];
    PDE.Clear();
    PT->entries[indexer.P_i] = PDE;
  }

  void* PageTableManager::RemapMemory(void *oldMemory, void *newMemory)
  {
    void *physAddr = WalkMemory(oldMemory);
    if (physAddr == NULL) return NULL;

    UnmapMemory(oldMemory);
    return MapMemory(newMemory, physAddr);
  }
}