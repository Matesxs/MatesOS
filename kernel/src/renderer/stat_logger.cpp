#include "stat_logger.h"
#include "../memory_management/PageFrameAllocator.h"
#include "../memory_management/heap.h"

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
}

void ShowBasicFrameBuffer()
{
  FrameBuffer fbuff = BasicRenderer::g_Renderer.GetFramebuffer();

  BasicRenderer::g_Renderer.SetColor(BasicRenderer::BR_DARK_CYAN);
  BasicRenderer::g_Renderer.Print("   Frame resolution:     ");
  BasicRenderer::g_Renderer.SetColor(BasicRenderer::BR_CYAN);
  BasicRenderer::g_Renderer.Print(to_string((uint64_t)fbuff.Width));
  BasicRenderer::g_Renderer.Print("x");
  BasicRenderer::g_Renderer.Print(to_string((uint64_t)fbuff.Height));

  BasicRenderer::g_Renderer.NewLine();
  BasicRenderer::g_Renderer.SetColor(BasicRenderer::BR_DARK_CYAN);
  BasicRenderer::g_Renderer.Print("   Frame buffer address: ");
  BasicRenderer::g_Renderer.SetColor(BasicRenderer::BR_CYAN);
  BasicRenderer::g_Renderer.Print("0x");
  BasicRenderer::g_Renderer.Print(to_hstring((uint64_t)fbuff.BaseAddress));

  BasicRenderer::g_Renderer.NewLine();
  BasicRenderer::g_Renderer.SetColor(BasicRenderer::BR_DARK_CYAN);
  BasicRenderer::g_Renderer.Print("   Frame buffer size:    ");
  BasicRenderer::g_Renderer.SetColor(BasicRenderer::BR_CYAN);
  sizePrint(fbuff.BufferSize);

  BasicRenderer::g_Renderer.NewLine();
  Point grid = BasicRenderer::g_Renderer.GetGrid();
  BasicRenderer::g_Renderer.SetColor(BasicRenderer::BR_DARK_CYAN);
  BasicRenderer::g_Renderer.Print("   Frame grid:           ");
  BasicRenderer::g_Renderer.SetColor(BasicRenderer::BR_CYAN);
  BasicRenderer::g_Renderer.Print(to_string(grid.X));
  BasicRenderer::g_Renderer.Print("x");
  BasicRenderer::g_Renderer.Print(to_string(grid.Y));
}

void ShowHeap()
{
  BasicRenderer::g_Renderer.SetColor(BasicRenderer::BR_DARK_CYAN);
  BasicRenderer::g_Renderer.Print("   Heap pages:           ");
  BasicRenderer::g_Renderer.SetColor(BasicRenderer::BR_CYAN);
  BasicRenderer::g_Renderer.Print(to_string(memory::GetHeapPages()));
  BasicRenderer::g_Renderer.NewLine();

  BasicRenderer::g_Renderer.SetColor(BasicRenderer::BR_DARK_CYAN);
  BasicRenderer::g_Renderer.Print("   Heap size:            ");
  BasicRenderer::g_Renderer.SetColor(BasicRenderer::BR_CYAN);
  sizePrint(memory::GetHeapPages() * 4096);
  BasicRenderer::g_Renderer.NewLine();
}

void ShowRAM()
{
  BasicRenderer::g_Renderer.SetColor(BasicRenderer::BR_DARK_CYAN);
  BasicRenderer::g_Renderer.Print("   Total RAM:            ");
  BasicRenderer::g_Renderer.SetColor(BasicRenderer::BR_CYAN);
  sizePrint(memory::g_Allocator.GetTotalMemory());

  BasicRenderer::g_Renderer.NewLine();
  BasicRenderer::g_Renderer.SetColor(BasicRenderer::BR_DARK_CYAN);
  BasicRenderer::g_Renderer.Print("   Free RAM:             ");
  BasicRenderer::g_Renderer.SetColor(BasicRenderer::BR_CYAN);
  sizePrint(memory::g_Allocator.GetFreeMemory());

  BasicRenderer::g_Renderer.NewLine();
  BasicRenderer::g_Renderer.SetColor(BasicRenderer::BR_DARK_CYAN);
  BasicRenderer::g_Renderer.Print("   Used RAM:             ");
  BasicRenderer::g_Renderer.SetColor(BasicRenderer::BR_CYAN);
  sizePrint(memory::g_Allocator.GetUsedMemory());

  BasicRenderer::g_Renderer.NewLine();
  BasicRenderer::g_Renderer.SetColor(BasicRenderer::BR_DARK_CYAN);
  BasicRenderer::g_Renderer.Print("   Reserved RAM:         ");
  BasicRenderer::g_Renderer.SetColor(BasicRenderer::BR_CYAN);
  sizePrint(memory::g_Allocator.GetReservedMemory());

  BasicRenderer::g_Renderer.NewLine();
}

void ShowOSStats()
{
  BasicRenderer::g_Renderer.SetColor(BasicRenderer::BR_WHITE);
  BasicRenderer::g_Renderer.Print("OS Stats:");
  BasicRenderer::g_Renderer.NewLine();
  ShowRAM();
  ShowHeap();
  ShowBasicFrameBuffer();
  BasicRenderer::g_Renderer.SetColor(BasicRenderer::BR_WHITE);
}

void ShowOSStats(unsigned int x, unsigned int y)
{
  BasicRenderer::Color tmpColor = BasicRenderer::g_Renderer.GetColor();
  uint32_t tmpX = BasicRenderer::g_Renderer.GetCursorX();
  uint32_t tmpY = BasicRenderer::g_Renderer.GetCursorY();

  BasicRenderer::g_Renderer.SetCursor(x, y);

  BasicRenderer::g_Renderer.SetColor(BasicRenderer::BR_WHITE);
  BasicRenderer::g_Renderer.Print("OS Stats:");
  BasicRenderer::g_Renderer.NewLine();
  ShowRAM();
  ShowHeap();
  ShowBasicFrameBuffer();
  BasicRenderer::g_Renderer.SetColor(BasicRenderer::BR_WHITE);

  BasicRenderer::g_Renderer.SetColor(tmpColor);
  BasicRenderer::g_Renderer.SetCursor(tmpX, tmpY);
}

void showSuccess(const char *message)
{
  BasicRenderer::g_Renderer.SetColor(BasicRenderer::BR_WHITE);
  BasicRenderer::g_Renderer.Print("[ ");
  BasicRenderer::g_Renderer.SetColor(BasicRenderer::BR_GREEN);
  BasicRenderer::g_Renderer.Print("OK");
  BasicRenderer::g_Renderer.SetColor(BasicRenderer::BR_WHITE);
  BasicRenderer::g_Renderer.Print(" ] ");
  BasicRenderer::g_Renderer.Print(message);
  BasicRenderer::g_Renderer.NewLine();
}

void showFailed(const char *message)
{
  BasicRenderer::g_Renderer.SetColor(BasicRenderer::BR_WHITE);
  BasicRenderer::g_Renderer.Print("[ ");
  BasicRenderer::g_Renderer.SetColor(BasicRenderer::BR_RED);
  BasicRenderer::g_Renderer.Print("NOK");
  BasicRenderer::g_Renderer.SetColor(BasicRenderer::BR_WHITE);
  BasicRenderer::g_Renderer.Print(" ] ");
  BasicRenderer::g_Renderer.Print(message);
  BasicRenderer::g_Renderer.NewLine();
}

void showWarning(const char *message)
{
  BasicRenderer::g_Renderer.SetColor(BasicRenderer::BR_WHITE);
  BasicRenderer::g_Renderer.Print("[ ");
  BasicRenderer::g_Renderer.SetColor(BasicRenderer::BR_YELLOW);
  BasicRenderer::g_Renderer.Print("WARN");
  BasicRenderer::g_Renderer.SetColor(BasicRenderer::BR_WHITE);
  BasicRenderer::g_Renderer.Print(" ] ");
  BasicRenderer::g_Renderer.Print(message);
  BasicRenderer::g_Renderer.NewLine();
}

void showInfo(const char *message)
{
  BasicRenderer::g_Renderer.SetColor(BasicRenderer::BR_WHITE);
  BasicRenderer::g_Renderer.Print("[ * ] ");
  BasicRenderer::g_Renderer.Print(message);
  BasicRenderer::g_Renderer.NewLine();
}

void printStats(const char *message, BasicRenderer::Color color)
{
  BasicRenderer::g_Renderer.SetColor(color);
  BasicRenderer::g_Renderer.Print(message);
}

void printStatsSpacing(BasicRenderer::Color color)
{
  BasicRenderer::g_Renderer.SetColor(color);
  BasicRenderer::g_Renderer.Print("   - ");
}

void statNewLine()
{
  BasicRenderer::g_Renderer.NewLine();
}