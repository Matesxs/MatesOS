#include "strlen.hpp"

size_t strlen(const char *s)
{
  size_t n = 0;
  while (*s)
  {
    n++;
    s++;
  }
  return n;
}