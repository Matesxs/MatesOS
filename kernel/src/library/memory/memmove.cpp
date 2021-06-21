#include "memmove.h"

void *memmove(void *__restrict__ dest, const void *__restrict__ src, size_t n)
{
  char *csrc = (char *)src;
  char *cdest = (char *)dest;

  char *temp = new char[n];

  for (size_t i=0; i<n; i++)
    temp[i] = csrc[i];

  for (size_t i=0; i<n; i++)
    cdest[i] = temp[i];

  delete [] temp;
  return cdest;
}