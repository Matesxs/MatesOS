#include "IO.h"

void WriteBus(uint16_t port, uint8_t value)
{
  asm volatile("outb %0, %1"
               :
               : "a"(value), "Nd"(port));
}

uint8_t ReadBus(uint16_t port)
{
  uint8_t returnVal;
  asm volatile("inb %1, %0"
               : "=a"(returnVal)
               : "Nd"(port));
  return returnVal;
}

void IOWait()
{
  asm volatile("outb %%al, $0x80"
               :
               : "a"(0));
}