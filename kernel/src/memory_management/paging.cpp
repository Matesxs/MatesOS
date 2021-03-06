#include "paging.hpp"

namespace memory
{
  void PageDirectoryEntry::SetFlag(PT_Flag flag, bool enabled)
  {
    uint64_t bitSelector = (uint64_t)1 << flag;
    Value &= ~bitSelector;
    if (enabled)
    {
      Value |= bitSelector;
    }
  }

  void PageDirectoryEntry::Clear() { Value = 0; }

  bool PageDirectoryEntry::GetFlag(PT_Flag flag)
  {
    uint64_t bitSelector = (uint64_t)1 << flag;
    return (Value & bitSelector) > 0 ? true : false;
  }

  void PageDirectoryEntry::SetFlags(uint64_t flags)
  {
    flags &= 0xfff0000000000fff;
    Value |= flags;
  }

  uint64_t PageDirectoryEntry::GetFlags()
  {
    return Value & 0xfff0000000000fff;
  }

  uint64_t PageDirectoryEntry::GetAddress()
  {
    return (Value & 0x000ffffffffff000) >> 12;
  }

  void PageDirectoryEntry::SetAddress(uint64_t address)
  {
    address &= 0x000000ffffffffff;
    Value &= 0xfff0000000000fff;
    Value |= (address << 12);
  }
}