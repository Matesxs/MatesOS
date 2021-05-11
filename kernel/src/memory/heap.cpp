#include "heap.h"
#include "PageTableManager.h"
#include "PageFrameAllocator.h"

namespace memory
{
  void *heapStart;
  void *heapEnd;
  HeapSegHeader *lastHdr;

  HeapSegHeader *HeapSegHeader::Split(size_t splitLength)
  {
    if (splitLength < 0x10)
      return NULL;
    int64_t splitSegLength = Length - splitLength - (sizeof(HeapSegHeader));
    if (splitSegLength < 0x10)
      return NULL;

    HeapSegHeader *newSplitHeader = (HeapSegHeader *)((size_t)this + splitLength + sizeof(HeapSegHeader));
    Next->Last = newSplitHeader;
    newSplitHeader->Next = Next;
    Next = newSplitHeader;
    newSplitHeader->Last = this;
    newSplitHeader->Length = splitSegLength;
    newSplitHeader->Free = Free;
    Length = splitLength;

    if (lastHdr == this)
      lastHdr = newSplitHeader;
    return newSplitHeader;
  }

  void HeapSegHeader::CombineBackward()
  {
    if (Last != NULL && Last->Free)
    {
      Last->CombineForward();
    }
  }

  void HeapSegHeader::CombineForward()
  {
    if (Next == NULL)
      return;
    if (!Next->Free)
      return;

    if (Next == lastHdr)
      lastHdr = this;

    if (Next->Next != NULL)
    {
      Next->Next->Last = this;
    }

    Length = Length + Next->Length + sizeof(HeapSegHeader);

    Next = Next->Next;
  }

  uint64_t InitializeHeap(void *heapAddress, size_t pageCount)
  {
    void *pos = heapAddress;

    size_t pageCounter = 0;
    for (; pageCounter < pageCount; pageCounter++)
    {
      void *newPage = GlobalAllocator.RequestPage();
      if (newPage == NULL)
        break;

      g_PageTableManager.MapMemory(pos, newPage);
      pos = (void *)((size_t)pos + 0x1000);
    }

    size_t heapLength = pageCounter * 0x1000;
    heapStart = heapAddress;
    heapEnd = (void *)((size_t)heapStart + heapLength);

    HeapSegHeader *startSegment = (HeapSegHeader *)heapAddress;
    startSegment->Length = heapLength - sizeof(HeapSegHeader);
    startSegment->Next = NULL;
    startSegment->Last = NULL;
    startSegment->Free = true;
    lastHdr = startSegment;

    return pageCounter;
  }

  void ExpandHeap(size_t length)
  {
    if (length % 0x1000)
    {
      length -= length % 0x1000;
      length += 0x1000;
    }

    size_t pageCount = length / 0x1000;
    HeapSegHeader *newSegment = (HeapSegHeader *)heapEnd;

    size_t pageCounter = 0;
    for (; pageCounter < pageCount; pageCounter++)
    {
      void *newPage = GlobalAllocator.RequestPage();
      if (newPage == NULL)
        break;

      g_PageTableManager.MapMemory(heapEnd, newPage);

      heapEnd = (void *)((size_t)heapEnd + 0x1000);
    }

    newSegment->Free = true;
    newSegment->Last = lastHdr;
    lastHdr->Next = newSegment;
    lastHdr = newSegment;
    newSegment->Next = NULL;
    newSegment->Length = (pageCounter * 0x1000) - sizeof(HeapSegHeader);
    newSegment->CombineBackward();
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

    HeapSegHeader *currentSegment = (HeapSegHeader *)heapStart;
    while (true)
    {
      if (currentSegment->Free)
      {
        if (currentSegment->Length > size)
        {
          currentSegment->Split(size);
          currentSegment->Free = false;
          return (void *)((uint64_t)currentSegment + sizeof(HeapSegHeader));
        }
        else if (currentSegment->Length == size)
        {
          currentSegment->Free = false;
          return (void *)((uint64_t)currentSegment + sizeof(HeapSegHeader));
        }
      }

      if (currentSegment->Next == NULL)
        break;
      currentSegment = currentSegment->Next;
    }

    ExpandHeap(size);
    return malloc(size);
  }

  void free(void *address)
  {
    HeapSegHeader *segment = (HeapSegHeader *)address - 1;
    segment->Free = true;
    segment->CombineForward();
    segment->CombineBackward();
  }
}

void *operator new(size_t n)
{
  return memory::malloc(n);
}

void *operator new(size_t n, void *p)
{
  return p;
}

void *operator new[](size_t s)
{
  return memory::malloc(s);
}

void operator delete(void *p)
{
  memory::free(p);
}

void operator delete[](void *p)
{
  memory::free(p);
}