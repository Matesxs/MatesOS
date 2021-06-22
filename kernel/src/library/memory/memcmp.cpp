#include "memcmp.h"

int memcmp (const void* str1, const void* str2, size_t count)
{
  char *s1 = (char*)str1;
  char *s2 = (char*)str2;

  while (count-- > 0)
  {
    if (*s1++ != *s2++)
      return s1[-1] < s2[-1] ? -1 : 1;
  }
  
  return 0;
}