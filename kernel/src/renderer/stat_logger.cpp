#include "stat_logger.h"
#include "../library/cstr.h"
#include "../memory/PageFrameAllocator.h"
#include "../utils/os_stats.h"

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

void ShowStats()
{
  uint32_t tmpColor = BasicRenderer::g_Renderer.GetColor();
  uint32_t tmpX = BasicRenderer::g_Renderer.GetCursorX();
  uint32_t tmpY = BasicRenderer::g_Renderer.GetCursorY();

  BasicRenderer::g_Renderer.SetCursor(statLoggingCursor.X, statLoggingCursor.Y);

  osStats::updateOSStats();

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

void statNewLine()
{
  statLoggingCursor.X = defaultX;
  statLoggingCursor.Y += 16;
}