#pragma once

#include <stdint.h>

namespace osStats
{
  extern uint64_t totalRam;
  extern uint64_t freeRam;
  extern uint64_t usedRam;
  extern uint64_t reservedRam;
  extern uint64_t frameBufferAddr;
  extern uint64_t frameBufferSize;
  extern uint64_t heapPages;

  void updateOSStats();
}