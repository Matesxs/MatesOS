#include "stat_logger.h"
#include "../memory_management/PageFrameAllocator.h"
#include "../memory_management/heap.h"

Point statLoggingCursor = {0, 0};
uint32_t defaultX = 0;

void setLoggerStart(unsigned int x, unsigned int y)
{
  if (x <= BasicRenderer::g_Renderer.GetWidth() && y <= BasicRenderer::g_Renderer.GetHeight())
  {
    statLoggingCursor.X = x;
    defaultX = x;
    statLoggingCursor.Y = y;
  }
}

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
}

void ShowRAM()
{
  BasicRenderer::g_Renderer.SetColor(BasicRenderer::BR_DARK_CYAN);
  BasicRenderer::g_Renderer.Print("   Total RAM:            ");
  BasicRenderer::g_Renderer.SetColor(BasicRenderer::BR_CYAN);
  sizePrint(memory::g_Allocator.GetTotalMemory());

  BasicRenderer::g_Renderer.SetColor(BasicRenderer::BR_DARK_CYAN);
  BasicRenderer::g_Renderer.Print("   Free RAM:             ");
  BasicRenderer::g_Renderer.SetColor(BasicRenderer::BR_CYAN);
  sizePrint(memory::g_Allocator.GetFreeMemory());

  BasicRenderer::g_Renderer.SetColor(BasicRenderer::BR_DARK_CYAN);
  BasicRenderer::g_Renderer.Print("   Used RAM:             ");
  BasicRenderer::g_Renderer.SetColor(BasicRenderer::BR_CYAN);
  sizePrint(memory::g_Allocator.GetUsedMemory());

  BasicRenderer::g_Renderer.SetColor(BasicRenderer::BR_DARK_CYAN);
  BasicRenderer::g_Renderer.Print("   Reserved RAM:         ");
  BasicRenderer::g_Renderer.SetColor(BasicRenderer::BR_CYAN);
  sizePrint(memory::g_Allocator.GetReservedMemory());
}

void ShowOSStats()
{
  uint32_t tmpColor = BasicRenderer::g_Renderer.GetColor();
  uint32_t tmpX = BasicRenderer::g_Renderer.GetCursorX();
  uint32_t tmpY = BasicRenderer::g_Renderer.GetCursorY();

  BasicRenderer::g_Renderer.SetCursor(statLoggingCursor.X, statLoggingCursor.Y);

  BasicRenderer::g_Renderer.SetColor(BasicRenderer::BR_WHITE);
  BasicRenderer::g_Renderer.Print("OS Stats:");
  BasicRenderer::g_Renderer.NewLine();
  ShowRAM();
  ShowHeap();
  ShowBasicFrameBuffer();
  BasicRenderer::g_Renderer.SetColor(BasicRenderer::BR_WHITE);

  BasicRenderer::g_Renderer.SetColor(tmpColor);
  statLoggingCursor.X = BasicRenderer::g_Renderer.GetCursorX();
  statLoggingCursor.Y = BasicRenderer::g_Renderer.GetCursorY();
  BasicRenderer::g_Renderer.SetCursor(tmpX, tmpY);
}

void ShowOSStats(unsigned int x, unsigned int y)
{
  uint32_t tmpColor = BasicRenderer::g_Renderer.GetColor();
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
  uint32_t tmpColor = BasicRenderer::g_Renderer.GetColor();
  uint32_t tmpX = BasicRenderer::g_Renderer.GetCursorX();
  uint32_t tmpY = BasicRenderer::g_Renderer.GetCursorY();

  BasicRenderer::g_Renderer.SetCursor(statLoggingCursor.X, statLoggingCursor.Y);

  BasicRenderer::g_Renderer.SetColor(BasicRenderer::BR_WHITE);
  BasicRenderer::g_Renderer.Print("[ ");
  BasicRenderer::g_Renderer.SetColor(BasicRenderer::BR_GREEN);
  BasicRenderer::g_Renderer.Print("OK");
  BasicRenderer::g_Renderer.SetColor(BasicRenderer::BR_WHITE);
  BasicRenderer::g_Renderer.Print(" ] ");
  BasicRenderer::g_Renderer.Print(message);
  BasicRenderer::g_Renderer.NewLine();

  BasicRenderer::g_Renderer.SetColor(tmpColor);
  statLoggingCursor.X = BasicRenderer::g_Renderer.GetCursorX();
  statLoggingCursor.Y = BasicRenderer::g_Renderer.GetCursorY();
  BasicRenderer::g_Renderer.SetCursor(tmpX, tmpY);
}

void showFailed(const char *message)
{
  uint32_t tmpColor = BasicRenderer::g_Renderer.GetColor();
  uint32_t tmpX = BasicRenderer::g_Renderer.GetCursorX();
  uint32_t tmpY = BasicRenderer::g_Renderer.GetCursorY();

  BasicRenderer::g_Renderer.SetCursor(statLoggingCursor.X, statLoggingCursor.Y);

  BasicRenderer::g_Renderer.SetColor(BasicRenderer::BR_WHITE);
  BasicRenderer::g_Renderer.Print("[ ");
  BasicRenderer::g_Renderer.SetColor(BasicRenderer::BR_RED);
  BasicRenderer::g_Renderer.Print("NOK");
  BasicRenderer::g_Renderer.SetColor(BasicRenderer::BR_WHITE);
  BasicRenderer::g_Renderer.Print(" ] ");
  BasicRenderer::g_Renderer.Print(message);
  BasicRenderer::g_Renderer.NewLine();

  BasicRenderer::g_Renderer.SetColor(tmpColor);
  statLoggingCursor.X = BasicRenderer::g_Renderer.GetCursorX();
  statLoggingCursor.Y = BasicRenderer::g_Renderer.GetCursorY();
  BasicRenderer::g_Renderer.SetCursor(tmpX, tmpY);
}

void showWarning(const char *message)
{
  uint32_t tmpColor = BasicRenderer::g_Renderer.GetColor();
  uint32_t tmpX = BasicRenderer::g_Renderer.GetCursorX();
  uint32_t tmpY = BasicRenderer::g_Renderer.GetCursorY();

  BasicRenderer::g_Renderer.SetCursor(statLoggingCursor.X, statLoggingCursor.Y);

  BasicRenderer::g_Renderer.SetColor(BasicRenderer::BR_WHITE);
  BasicRenderer::g_Renderer.Print("[ ");
  BasicRenderer::g_Renderer.SetColor(BasicRenderer::BR_YELLOW);
  BasicRenderer::g_Renderer.Print("WARN");
  BasicRenderer::g_Renderer.SetColor(BasicRenderer::BR_WHITE);
  BasicRenderer::g_Renderer.Print(" ] ");
  BasicRenderer::g_Renderer.Print(message);
  BasicRenderer::g_Renderer.NewLine();

  BasicRenderer::g_Renderer.SetColor(tmpColor);
  statLoggingCursor.X = BasicRenderer::g_Renderer.GetCursorX();
  statLoggingCursor.Y = BasicRenderer::g_Renderer.GetCursorY();
  BasicRenderer::g_Renderer.SetCursor(tmpX, tmpY);
}

void showInfo(const char *message)
{
  uint32_t tmpColor = BasicRenderer::g_Renderer.GetColor();
  uint32_t tmpX = BasicRenderer::g_Renderer.GetCursorX();
  uint32_t tmpY = BasicRenderer::g_Renderer.GetCursorY();

  BasicRenderer::g_Renderer.SetCursor(statLoggingCursor.X, statLoggingCursor.Y);

  BasicRenderer::g_Renderer.SetColor(BasicRenderer::BR_WHITE);
  BasicRenderer::g_Renderer.Print("[ * ] ");
  BasicRenderer::g_Renderer.Print(message);
  BasicRenderer::g_Renderer.NewLine();

  BasicRenderer::g_Renderer.SetColor(tmpColor);
  statLoggingCursor.X = BasicRenderer::g_Renderer.GetCursorX();
  statLoggingCursor.Y = BasicRenderer::g_Renderer.GetCursorY();
  BasicRenderer::g_Renderer.SetCursor(tmpX, tmpY);
}

void printStats(const char *message, uint32_t color)
{
  uint32_t tmpColor = BasicRenderer::g_Renderer.GetColor();
  uint32_t tmpX = BasicRenderer::g_Renderer.GetCursorX();
  uint32_t tmpY = BasicRenderer::g_Renderer.GetCursorY();

  BasicRenderer::g_Renderer.SetCursor(statLoggingCursor.X, statLoggingCursor.Y);

  BasicRenderer::g_Renderer.SetColor(color);
  BasicRenderer::g_Renderer.Print(message);

  BasicRenderer::g_Renderer.SetColor(tmpColor);
  statLoggingCursor.X = BasicRenderer::g_Renderer.GetCursorX();
  statLoggingCursor.Y = BasicRenderer::g_Renderer.GetCursorY();
  BasicRenderer::g_Renderer.SetCursor(tmpX, tmpY);
}

void printStatsSpacing(uint32_t color)
{
  uint32_t tmpColor = BasicRenderer::g_Renderer.GetColor();
  uint32_t tmpX = BasicRenderer::g_Renderer.GetCursorX();
  uint32_t tmpY = BasicRenderer::g_Renderer.GetCursorY();

  BasicRenderer::g_Renderer.SetCursor(statLoggingCursor.X, statLoggingCursor.Y);

  BasicRenderer::g_Renderer.SetColor(color);
  BasicRenderer::g_Renderer.Print("   - ");

  BasicRenderer::g_Renderer.SetColor(tmpColor);
  statLoggingCursor.X = BasicRenderer::g_Renderer.GetCursorX();
  statLoggingCursor.Y = BasicRenderer::g_Renderer.GetCursorY();
  BasicRenderer::g_Renderer.SetCursor(tmpX, tmpY);
}

void statNewLine()
{
  statLoggingCursor.X = defaultX;
  statLoggingCursor.Y += 16;
}