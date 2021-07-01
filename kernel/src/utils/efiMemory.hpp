#pragma once
#include <stdint.h>

enum EfiMemoryType
{
  EfiReservedMemoryType,
  EfiLoaderCode,
  EfiLoaderData,
  EfiBootServicesCode,
  EfiBootServicesData,
  EfiRuntimeServicesCode,
  EfiRuntimeServicesData,
  EfiConventionalMemory,
  EfiUnusableMemory,
  EfiACPIReclaimMemory,
  EfiACPIMemoryNVS,
  EfiMemoryMappedIO,
  EfiMemoryMappedIOPortSpace,
  EfiPalCode,
};

struct EFI_MEMORY_DESCRIPTOR
{
  uint32_t type;
  void *physAddr;
  void *virtAddr;
  uint64_t numPages;
  uint64_t attribs;
};

uint64_t GetMemorySize(EFI_MEMORY_DESCRIPTOR *mMap, uint64_t mMapEntries, uint64_t mMapDescSize);

extern const char *EFI_MEMORY_TYPE_STRINGS[];