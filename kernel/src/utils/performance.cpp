#include "performance.h"
#include "cstr.h"
#include "../memory/PageFrameAllocator.h"
#include "os_stats.h"

void sizePrint(uint64_t size)
{
  if (size / 1099511627776 > 1)
  {
    BasicRenderer::g_Renderer.Print(to_string((uint64_t)(size / 1099511627776)));
    BasicRenderer::g_Renderer.Print(" TB");
  }
  else if (size / 1073741824 > 1)
  {
    BasicRenderer::g_Renderer.Print(to_string((uint64_t)(size / 1073741824)));
    BasicRenderer::g_Renderer.Print(" GB");
  }
  else if (size / 1048576 > 1)
  {
    BasicRenderer::g_Renderer.Print(to_string((uint64_t)(size / 1048576)));
    BasicRenderer::g_Renderer.Print(" MB");
  }
  else if (size / 1024 > 1)
  {
    BasicRenderer::g_Renderer.Print(to_string((uint64_t)(size / 1024)));
    BasicRenderer::g_Renderer.Print(" KB");
  }
  else
  {
    BasicRenderer::g_Renderer.Print(to_string((uint64_t)size));
    BasicRenderer::g_Renderer.Print(" B");
  }

  BasicRenderer::g_Renderer.NewLine();
}

void ShowBasicFrameBuffer()
{
  BasicRenderer::g_Renderer.SetColor(BasicRenderer::BR_WHITE);
  BasicRenderer::g_Renderer.Print("Frame buffer address: ");
  BasicRenderer::g_Renderer.SetColor(BasicRenderer::BR_CYAN);
  BasicRenderer::g_Renderer.Print("0x");
  BasicRenderer::g_Renderer.Print(to_hstring(osStats::frameBufferAddr));

  BasicRenderer::g_Renderer.NewLine();
  BasicRenderer::g_Renderer.SetColor(BasicRenderer::BR_WHITE);
  BasicRenderer::g_Renderer.Print("Frame buffer size: ");
  BasicRenderer::g_Renderer.SetColor(BasicRenderer::BR_CYAN);
  sizePrint(osStats::frameBufferSize);
}

void ShowHeap()
{
  BasicRenderer::g_Renderer.SetColor(BasicRenderer::BR_WHITE);
  BasicRenderer::g_Renderer.Print("Heap pages: ");
  BasicRenderer::g_Renderer.SetColor(BasicRenderer::BR_CYAN);
  BasicRenderer::g_Renderer.Print(to_string(osStats::heapPages));
  BasicRenderer::g_Renderer.NewLine();

  BasicRenderer::g_Renderer.SetColor(BasicRenderer::BR_WHITE);
  BasicRenderer::g_Renderer.Print("Heap size: ");
  BasicRenderer::g_Renderer.SetColor(BasicRenderer::BR_CYAN);
  sizePrint(osStats::heapPages * 4096);
}

void ShowRAM()
{
  BasicRenderer::g_Renderer.SetColor(BasicRenderer::BR_WHITE);
  BasicRenderer::g_Renderer.Print("Total RAM: ");
  BasicRenderer::g_Renderer.SetColor(BasicRenderer::BR_CYAN);
  sizePrint(osStats::totalRam);

  BasicRenderer::g_Renderer.SetColor(BasicRenderer::BR_WHITE);
  BasicRenderer::g_Renderer.Print("Free RAM: ");
  BasicRenderer::g_Renderer.SetColor(BasicRenderer::BR_CYAN);
  sizePrint(osStats::freeRam);

  BasicRenderer::g_Renderer.SetColor(BasicRenderer::BR_WHITE);
  BasicRenderer::g_Renderer.Print("Used RAM: ");
  BasicRenderer::g_Renderer.SetColor(BasicRenderer::BR_CYAN);
  sizePrint(osStats::usedRam);

  BasicRenderer::g_Renderer.SetColor(BasicRenderer::BR_WHITE);
  BasicRenderer::g_Renderer.Print("Reserved RAM: ");
  BasicRenderer::g_Renderer.SetColor(BasicRenderer::BR_CYAN);
  sizePrint(osStats::reservedRam);
}

void ShowStats(unsigned int x, unsigned int y)
{
  osStats::updateOSStats();

  BasicRenderer::g_Renderer.SetCursor(x, y);
  BasicRenderer::g_Renderer.SetColor(BasicRenderer::BR_WHITE);
  BasicRenderer::g_Renderer.Print("OS Stats:");
  BasicRenderer::g_Renderer.NewLine();
  ShowRAM();
  ShowHeap();
  ShowBasicFrameBuffer();
  BasicRenderer::g_Renderer.SetColor(BasicRenderer::BR_WHITE);
}