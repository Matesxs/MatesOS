#include "helpers.h"

void halt()
{
  while (1)
  {
    asm("hlt");
  }
}