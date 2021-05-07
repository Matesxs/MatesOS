#pragma once

#include <stdint.h>
#include <stddef.h>

struct HeapSegHeader
{
  size_t Length;
  HeapSegHeader *Next;
  HeapSegHeader *Last;
  bool Free;
  void CombineForward();
  void CombineBackward();
  HeapSegHeader* Split(size_t splitLength);
};

uint64_t InitializeHeap(void *heapAddress, size_t pageCount);
void ExpandHeap(size_t length);

void* malloc(size_t size);
void free(void *address);

inline void* operator new(size_t size) {return malloc(size);}
inline void* operator new[](size_t size) {return malloc(size);}

inline void operator delete(void *p) {free(p);}