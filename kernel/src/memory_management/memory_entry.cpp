#include "memory_entry.h"

size_t GetMemorySize(stivale2_mmap_entry *memory_map, size_t memory_entries)
{
  size_t size = 0;
  for (size_t i = 0; i < memory_entries; i++)
  {
    stivale2_mmap_entry entry = memory_map[i];
    size += entry.length;
  }

  return size;
}