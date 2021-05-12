#include "performance.h"
#include "cstr.h"
#include "../memory/PageFrameAllocator.h"
#include "os_stats.h"

void ShowStats(unsigned int x, unsigned int y)
{
  g_BasicRenderer.SetCursor(x, y);
  g_BasicRenderer.SetColor(BR_WHITE);
  g_BasicRenderer.Print("OS Stats:");
  g_BasicRenderer.NewLine();
  ShowRAM(g_BasicRenderer.GetCursorX(), g_BasicRenderer.GetCursorY());
  g_BasicRenderer.SetColor(BR_WHITE);
}

void ShowRAM(unsigned int x, unsigned int y)
{
  osStats::updateOSStats();

  g_BasicRenderer.SetCursor(x, y);

  g_BasicRenderer.SetColor(BR_WHITE);
  g_BasicRenderer.Print("Total RAM: ");
  g_BasicRenderer.SetColor(BR_CYAN);
  if (osStats::totalRam / 1099511627776 > 1)
  {
    g_BasicRenderer.Print(to_string((uint64_t)(osStats::totalRam / 1099511627776)));
    g_BasicRenderer.Print(" TB");
  }
  else if (osStats::totalRam / 1073741824 > 1)
  {
    g_BasicRenderer.Print(to_string((uint64_t)(osStats::totalRam / 1073741824)));
    g_BasicRenderer.Print(" GB");
  }
  else if (osStats::totalRam / 1048576 > 1)
  {
    g_BasicRenderer.Print(to_string((uint64_t)(osStats::totalRam / 1048576)));
    g_BasicRenderer.Print(" MB");
  }
  else if (osStats::totalRam / 1024 > 1)
  {
    g_BasicRenderer.Print(to_string((uint64_t)(osStats::totalRam / 1024)));
    g_BasicRenderer.Print(" KB");
  }
  else
  {
    g_BasicRenderer.Print(to_string((uint64_t)osStats::totalRam));
    g_BasicRenderer.Print(" B");
  }

  g_BasicRenderer.NewLine();
  g_BasicRenderer.SetColor(BR_WHITE);
  g_BasicRenderer.Print("Free RAM: ");
  g_BasicRenderer.SetColor(BR_CYAN);
  if (osStats::freeRam / 1099511627776 > 1)
  {
    g_BasicRenderer.Print(to_string((uint64_t)(osStats::freeRam / 1099511627776)));
    g_BasicRenderer.Print(" TB");
  }
  else if (osStats::freeRam / 1073741824 > 1)
  {
    g_BasicRenderer.Print(to_string((uint64_t)(osStats::freeRam / 1073741824)));
    g_BasicRenderer.Print(" GB");
  }
  else if (osStats::freeRam / 1048576 > 1)
  {
    g_BasicRenderer.Print(to_string((uint64_t)(osStats::freeRam / 1048576)));
    g_BasicRenderer.Print(" MB");
  }
  else if (osStats::freeRam / 1024 > 1)
  {
    g_BasicRenderer.Print(to_string((uint64_t)(osStats::freeRam / 1024)));
    g_BasicRenderer.Print(" KB");
  }
  else
  {
    g_BasicRenderer.Print(to_string((uint64_t)osStats::freeRam));
    g_BasicRenderer.Print(" B");
  }

  g_BasicRenderer.NewLine();
  g_BasicRenderer.SetColor(BR_WHITE);
  g_BasicRenderer.Print("Used RAM: ");
  g_BasicRenderer.SetColor(BR_CYAN);
  if (osStats::usedRam / 1099511627776 > 1)
  {
    g_BasicRenderer.Print(to_string((uint64_t)(osStats::usedRam / 1099511627776)));
    g_BasicRenderer.Print(" TB");
  }
  else if (osStats::usedRam / 1073741824 > 1)
  {
    g_BasicRenderer.Print(to_string((uint64_t)(osStats::usedRam / 1073741824)));
    g_BasicRenderer.Print(" GB");
  }
  else if (osStats::usedRam / 1048576 > 1)
  {
    g_BasicRenderer.Print(to_string((uint64_t)(osStats::usedRam / 1048576)));
    g_BasicRenderer.Print(" MB");
  }
  else if (osStats::usedRam / 1024 > 1)
  {
    g_BasicRenderer.Print(to_string((uint64_t)(osStats::usedRam / 1024)));
    g_BasicRenderer.Print(" KB");
  }
  else
  {
    g_BasicRenderer.Print(to_string((uint64_t)osStats::usedRam));
    g_BasicRenderer.Print(" B");
  }

  g_BasicRenderer.NewLine();
  g_BasicRenderer.SetColor(BR_WHITE);
  g_BasicRenderer.Print("Reserved RAM: ");
  g_BasicRenderer.SetColor(BR_CYAN);
  if (osStats::reservedRam / 1099511627776 > 1)
  {
    g_BasicRenderer.Print(to_string((uint64_t)(osStats::reservedRam / 1099511627776)));
    g_BasicRenderer.Print(" TB");
  }
  else if (osStats::reservedRam / 1073741824 > 1)
  {
    g_BasicRenderer.Print(to_string((uint64_t)(osStats::reservedRam / 1073741824)));
    g_BasicRenderer.Print(" GB");
  }
  else if (osStats::reservedRam / 1048576 > 1)
  {
    g_BasicRenderer.Print(to_string((uint64_t)(osStats::reservedRam / 1048576)));
    g_BasicRenderer.Print(" MB");
  }
  else if (osStats::reservedRam / 1024 > 1)
  {
    g_BasicRenderer.Print(to_string((uint64_t)(osStats::reservedRam / 1024)));
    g_BasicRenderer.Print(" KB");
  }
  else
  {
    g_BasicRenderer.Print(to_string((uint64_t)osStats::reservedRam));
    g_BasicRenderer.Print(" B");
  }

  g_BasicRenderer.NewLine();
  g_BasicRenderer.SetColor(BR_WHITE);
  g_BasicRenderer.Print("Frame buffer address: ");
  g_BasicRenderer.SetColor(BR_CYAN);
  g_BasicRenderer.Print("0x");
  g_BasicRenderer.Print(to_hstring(osStats::frameBufferAddr));

  g_BasicRenderer.NewLine();
  g_BasicRenderer.SetColor(BR_WHITE);
  g_BasicRenderer.Print("Frame buffer size: ");
  g_BasicRenderer.SetColor(BR_CYAN);
  if (osStats::frameBufferSize / 1099511627776 > 1)
  {
    g_BasicRenderer.Print(to_string((uint64_t)(osStats::frameBufferSize / 1099511627776)));
    g_BasicRenderer.Print(" TB");
  }
  else if (osStats::frameBufferSize / 1073741824 > 1)
  {
    g_BasicRenderer.Print(to_string((uint64_t)(osStats::frameBufferSize / 1073741824)));
    g_BasicRenderer.Print(" GB");
  }
  else if (osStats::frameBufferSize / 1048576 > 1)
  {
    g_BasicRenderer.Print(to_string((uint64_t)(osStats::frameBufferSize / 1048576)));
    g_BasicRenderer.Print(" MB");
  }
  else if (osStats::frameBufferSize / 1024 > 1)
  {
    g_BasicRenderer.Print(to_string((uint64_t)(osStats::frameBufferSize / 1024)));
    g_BasicRenderer.Print(" KB");
  }
  else
  {
    g_BasicRenderer.Print(to_string((uint64_t)osStats::frameBufferSize));
    g_BasicRenderer.Print(" B");
  }
}