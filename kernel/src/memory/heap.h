#pragma once

#include <stdint.h>
#include <stddef.h>

namespace memory
{
  struct HeapSegHdr
  {
    size_t length;
    HeapSegHdr *next;
    HeapSegHdr *last;
    bool free;
    void CombineForward();
    void CombineBackward();
    HeapSegHdr *Split(size_t splitLength);
  };

  bool InitializeHeap(void *heapAddress, size_t pageCount);

  void *malloc(size_t size);
  void free(void *address);

  bool ExpandHeap(size_t length);
}

void *operator new(size_t n);
void *operator new(size_t n, void *p);
void *operator new[](size_t s);
void operator delete(void *p);
void operator delete[](void *p);