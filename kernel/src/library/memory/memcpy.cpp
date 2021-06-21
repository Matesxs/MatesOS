#include "memcpy.h"

void *memcpy(void *__restrict__ dest, const void *__restrict__ src, size_t n)
{
	char *csrc = (char *)src;
  char *cdest = (char *)dest;

  for (; n; n--) *cdest++ = *csrc++;
	return dest;
}