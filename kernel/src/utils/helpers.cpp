#include "helpers.h"

void halt()
{
  while (1)
  {
    asm("hlt");
  }
}

uint8_t checksum(const char *addr, size_t size) noexcept
{
  const char *end = addr + size;
  uint8_t sum = 0;
  while (addr < end)
  {
    sum += *addr;
    addr++;
  }
  return sum;
}