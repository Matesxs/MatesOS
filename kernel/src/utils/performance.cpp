#include "performance.h"
#include "cstr.h"
#include "../paging/PageFrameAllocator.h"

void ShowStats(unsigned int x, unsigned int y)
{
  GlobalBasicRenderer.SetCursor(x, y);
  GlobalBasicRenderer.SetColor(BR_WHITE);
  GlobalBasicRenderer.Print("OS Stats:");
  GlobalBasicRenderer.NewLine();
  ShowRAM(GlobalBasicRenderer.GetCursorX(), GlobalBasicRenderer.GetCursorY());
  GlobalBasicRenderer.SetColor(BR_WHITE);
}

void ShowRAM(unsigned int x, unsigned int y)
{
  uint64_t freeRam = GlobalAllocator.GetFreeRAM();
  uint64_t usedRam = GlobalAllocator.GetUsedRAM();
  uint64_t reservedRam = GlobalAllocator.GetReservedRAM();

  GlobalBasicRenderer.SetCursor(x, y);

  GlobalBasicRenderer.SetColor(BR_BLUE);
  GlobalBasicRenderer.Print("Free RAM: ");
  GlobalBasicRenderer.SetColor(BR_CYAN);
  if (freeRam / 1099511627776 > 1)
  {
    GlobalBasicRenderer.Print(to_string((uint64_t)(freeRam / 1099511627776)));
    GlobalBasicRenderer.Print(" TB");
  }
  else if (freeRam / 1073741824 > 1)
  {
    GlobalBasicRenderer.Print(to_string((uint64_t)(freeRam / 1073741824)));
    GlobalBasicRenderer.Print(" GB");
  }
  else if (freeRam / 1048576 > 1)
  {
    GlobalBasicRenderer.Print(to_string((uint64_t)(freeRam / 1048576)));
    GlobalBasicRenderer.Print(" MB");
  }
  else if (freeRam / 1024 > 1)
  {
    GlobalBasicRenderer.Print(to_string((uint64_t)(freeRam / 1024)));
    GlobalBasicRenderer.Print(" KB");
  }
  else
  {
    GlobalBasicRenderer.Print(to_string((uint64_t)freeRam));
    GlobalBasicRenderer.Print(" B");
  }

  GlobalBasicRenderer.NewLine();
  GlobalBasicRenderer.SetColor(BR_BLUE);
  GlobalBasicRenderer.Print("Used RAM: ");
  GlobalBasicRenderer.SetColor(BR_CYAN);
  if (usedRam / 1099511627776 > 1)
  {
    GlobalBasicRenderer.Print(to_string((uint64_t)(usedRam / 1099511627776)));
    GlobalBasicRenderer.Print(" TB");
  }
  else if (usedRam / 1073741824 > 1)
  {
    GlobalBasicRenderer.Print(to_string((uint64_t)(usedRam / 1073741824)));
    GlobalBasicRenderer.Print(" GB");
  }
  else if (usedRam / 1048576 > 1)
  {
    GlobalBasicRenderer.Print(to_string((uint64_t)(usedRam / 1048576)));
    GlobalBasicRenderer.Print(" MB");
  }
  else if (usedRam / 1024 > 1)
  {
    GlobalBasicRenderer.Print(to_string((uint64_t)(usedRam / 1024)));
    GlobalBasicRenderer.Print(" KB");
  }
  else
  {
    GlobalBasicRenderer.Print(to_string((uint64_t)usedRam));
    GlobalBasicRenderer.Print(" B");
  }

  GlobalBasicRenderer.NewLine();
  GlobalBasicRenderer.SetColor(BR_BLUE);
  GlobalBasicRenderer.Print("Reserved RAM: ");
  GlobalBasicRenderer.SetColor(BR_CYAN);
  if (reservedRam / 1099511627776 > 1)
  {
    GlobalBasicRenderer.Print(to_string((uint64_t)(reservedRam / 1099511627776)));
    GlobalBasicRenderer.Print(" TB");
  }
  else if (reservedRam / 1073741824 > 1)
  {
    GlobalBasicRenderer.Print(to_string((uint64_t)(reservedRam / 1073741824)));
    GlobalBasicRenderer.Print(" GB");
  }
  else if (reservedRam / 1048576 > 1)
  {
    GlobalBasicRenderer.Print(to_string((uint64_t)(reservedRam / 1048576)));
    GlobalBasicRenderer.Print(" MB");
  }
  else if (reservedRam / 1024 > 1)
  {
    GlobalBasicRenderer.Print(to_string((uint64_t)(reservedRam / 1024)));
    GlobalBasicRenderer.Print(" KB");
  }
  else
  {
    GlobalBasicRenderer.Print(to_string((uint64_t)reservedRam));
    GlobalBasicRenderer.Print(" B");
  }
}