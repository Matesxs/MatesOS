#include "heap.h"
#include "PageTableManager.h"
#include "PageFrameAllocator.h"
#include "../library/memory.h"
#include "../utils/panic.h"
#include "../renderer/stat_logger.h"

namespace memory
{
  void *heapStart = NULL;
  void *heapEnd = NULL;

  HeapSegHdr *LastHdr = NULL;
  uint64_t usedPages = 0;
  size_t start_page_count = 0;

  void CreateHeap(void *heapAddress, size_t pageCount)
  {
    if (heapStart != NULL) Panic("Heap already initialized");

    void *pos = heapAddress;

    for (size_t i = 0; i < pageCount; i++)
    {
      void *temp = g_Allocator.RequestPage();
      if (temp == NULL) Panic("Cant request heap page");

      g_PageTableManager.MapMemory(pos, temp);

      pos = (void *)((size_t)pos + 0x1000);
      usedPages++;
    }

    size_t heapLength = pageCount * 0x1000;

    heapStart = heapAddress;
    heapEnd = (void *)((size_t)heapStart + heapLength);

    HeapSegHdr *startSeg = (HeapSegHdr *)heapAddress;
    startSeg->length = heapLength - sizeof(HeapSegHdr);
    startSeg->next = NULL;
    startSeg->last = NULL;
    startSeg->free = true;

    LastHdr = startSeg;
    start_page_count = pageCount;
  }

  void WalkHeap()
  {
    showInfo("Heap walkthru");

    HeapSegHdr *hdr = (HeapSegHdr*)heapStart;
    uint64_t c = 0;

    while (hdr)
    {
      printStatsSpacing();
      printStats("[");
      printStats(to_string(c));
      printStats("] Base: 0x");
      printStats(to_hstring((uint64_t)hdr + sizeof(HeapSegHdr)));
      printStats(", Size: ");
      printStats(to_string(hdr->length));
      printStats(", Free: ");
      printStats(hdr->free ? "True" : "False");
      printStats(", Next: 0x");
      printStats(to_hstring((uint64_t)hdr->next));
      statNewLine();

      hdr = hdr->next;
      c++;
    }
  }

  HeapSegHdr *HeapSegHdr::Split(size_t splitLength)
  {
    if (splitLength < 0x10)
      return NULL;

    int64_t splitSegLength = length - splitLength - (sizeof(HeapSegHdr));
    if (splitSegLength < 0x10)
      return NULL;

    HeapSegHdr *newSplitHdr = (HeapSegHdr *)((size_t)this + splitLength + sizeof(HeapSegHdr));
    next->last = newSplitHdr;                         // Set the next segment's last segment to our new segment
    newSplitHdr->next = next;                         // Set the new segment's next segment to out original next segment
    next = newSplitHdr;                               // Set our new segment to the new segment
    newSplitHdr->last = this;                         // Set our new segment's last segment to the current segment
    newSplitHdr->length = splitSegLength;             // Set the new header's length to the calculated value
    newSplitHdr->free = free;                         // make sure the new segment's free is the same as the original
    length = splitLength;                             // set the length of the original segment to its new length

    if (LastHdr == this)
      LastHdr = newSplitHdr;

    return newSplitHdr;
  }

  bool ExpandHeap(size_t length)
  {
    if (length % 0x1000)
    {
      length -= length % 0x1000;
      length += 0x1000;
    }

    size_t pageCount = length / 0x1000;
    HeapSegHdr *newSegment = (HeapSegHdr *)heapEnd;

    for (size_t i = 0; i < pageCount; i++)
    {
      void *tmp = g_Allocator.RequestPage();
      if (tmp == NULL) return false;

      g_PageTableManager.MapMemory(heapEnd, tmp);

      heapEnd = (void *)((size_t)heapEnd + 0x1000);
      usedPages++;
    }

    newSegment->free = true;
    newSegment->last = LastHdr;
    LastHdr->next = newSegment;
    LastHdr = newSegment;
    newSegment->next = NULL;
    newSegment->length = length - sizeof(HeapSegHdr);
    newSegment->CombineBackward();

    return true;
  }

  void HeapSegHdr::CombineForward()
  {
    if (next == NULL)
      return;

    if (!next->free)
      return;

    if (next == LastHdr)
      LastHdr = this;

    if (next->next != NULL)
    {
      next->next->last = this;
    }

    length = length + next->length + sizeof(HeapSegHdr);

    next = next->next;
  }

  void HeapSegHdr::CombineBackward()
  {
    if (last != NULL && last->free)
      last->CombineForward();
  }

  void *malloc(size_t size)
  {
    if (size == 0)
      return NULL;

    if (size % 0x10 > 0)
    {
      size -= (size % 0x10);
      size += 0x10;
    }

    HeapSegHdr *currentSeg = (HeapSegHdr *)heapStart;
    while (true)
    {
      if (currentSeg->free)
      {
        size_t curSize = currentSeg->length - sizeof(HeapSegHdr);

        if (curSize > size)
        {
          currentSeg->Split(size);
          currentSeg->free = false;
          return (void *)((uint64_t)currentSeg + sizeof(HeapSegHdr));
        }
        else if (curSize == size)
        {
          currentSeg->free = false;
          return (void *)((uint64_t)currentSeg + sizeof(HeapSegHdr));
        }
      }

      if (currentSeg->next == NULL)
        break;

      currentSeg = currentSeg->next;
    }

    if (!ExpandHeap(size))
      return NULL;

    return malloc(size);
  }

  void *calloc(size_t size)
  {
    if (size == 0)
      return NULL;

    if (size % 0x10 > 0)
    {
      size -= (size % 0x10);
      size += 0x10;
    }

    void *mem = malloc(size);
    if (mem == NULL) return NULL;

    memset(mem, 0, size);
    return mem;
  }

  void free(void *address)
  {
    if (address == NULL) return;

    HeapSegHdr *segment = (HeapSegHdr *)address - 1;
    segment->free = true;
    segment->CombineForward();
    segment->CombineBackward();
  }

  uint64_t GetHeapPages()
  {
    return usedPages;
  }
}

void *operator new(size_t n)
{
  return memory::calloc(n);
}

void *operator new(size_t n, void *p)
{
  return p;
}

void *operator new[](size_t s)
{
  return memory::calloc(s);
}

void operator delete(void *p)
{
  memory::free(p);
}

void operator delete[](void *p)
{
  memory::free(p);
}