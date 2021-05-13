#include "stat_logger.h"

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