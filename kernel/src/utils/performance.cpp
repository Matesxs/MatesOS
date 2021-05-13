#include "performance.h"
#include "cstr.h"
#include "../memory/PageFrameAllocator.h"
#include "os_stats.h"

void sizePrint(uint64_t size)
{
  if (size / 1099511627776 > 1)
  {
    g_BasicRenderer.Print(to_string((uint64_t)(size / 1099511627776)));
    g_BasicRenderer.Print(" TB");
  }
  else if (size / 1073741824 > 1)
  {
    g_BasicRenderer.Print(to_string((uint64_t)(size / 1073741824)));
    g_BasicRenderer.Print(" GB");
  }
  else if (size / 1048576 > 1)
  {
    g_BasicRenderer.Print(to_string((uint64_t)(size / 1048576)));
    g_BasicRenderer.Print(" MB");
  }
  else if (size / 1024 > 1)
  {
    g_BasicRenderer.Print(to_string((uint64_t)(size / 1024)));
    g_BasicRenderer.Print(" KB");
  }
  else
  {
    g_BasicRenderer.Print(to_string((uint64_t)size));
    g_BasicRenderer.Print(" B");
  }

  g_BasicRenderer.NewLine();
}

void ShowBasicFrameBuffer()
{
  g_BasicRenderer.SetColor(BR_WHITE);
  g_BasicRenderer.Print("Frame buffer address: ");
  g_BasicRenderer.SetColor(BR_CYAN);
  g_BasicRenderer.Print("0x");
  g_BasicRenderer.Print(to_hstring(osStats::frameBufferAddr));

  g_BasicRenderer.NewLine();
  g_BasicRenderer.SetColor(BR_WHITE);
  g_BasicRenderer.Print("Frame buffer size: ");
  g_BasicRenderer.SetColor(BR_CYAN);
  sizePrint(osStats::frameBufferSize);
}

void ShowHeap()
{
  g_BasicRenderer.SetColor(BR_WHITE);
  g_BasicRenderer.Print("Heap pages: ");
  g_BasicRenderer.SetColor(BR_CYAN);
  g_BasicRenderer.Print(to_string(osStats::heapPages));
  g_BasicRenderer.NewLine();

  g_BasicRenderer.SetColor(BR_WHITE);
  g_BasicRenderer.Print("Heap size: ");
  g_BasicRenderer.SetColor(BR_CYAN);
  sizePrint(osStats::heapPages * 4096);
}

void ShowRAM()
{
  g_BasicRenderer.SetColor(BR_WHITE);
  g_BasicRenderer.Print("Total RAM: ");
  g_BasicRenderer.SetColor(BR_CYAN);
  sizePrint(osStats::totalRam);

  g_BasicRenderer.SetColor(BR_WHITE);
  g_BasicRenderer.Print("Free RAM: ");
  g_BasicRenderer.SetColor(BR_CYAN);
  sizePrint(osStats::freeRam);

  g_BasicRenderer.SetColor(BR_WHITE);
  g_BasicRenderer.Print("Used RAM: ");
  g_BasicRenderer.SetColor(BR_CYAN);
  sizePrint(osStats::usedRam);

  g_BasicRenderer.SetColor(BR_WHITE);
  g_BasicRenderer.Print("Reserved RAM: ");
  g_BasicRenderer.SetColor(BR_CYAN);
  sizePrint(osStats::reservedRam);
}

void ShowStats(unsigned int x, unsigned int y)
{
  osStats::updateOSStats();

  g_BasicRenderer.SetCursor(x, y);
  g_BasicRenderer.SetColor(BR_WHITE);
  g_BasicRenderer.Print("OS Stats:");
  g_BasicRenderer.NewLine();
  ShowRAM();
  ShowHeap();
  ShowBasicFrameBuffer();
  g_BasicRenderer.SetColor(BR_WHITE);
}