#include "stat_logger.h"

Point statLoggingCursor = {0, 0};
uint32_t defaultX = 0;

void setLoggerStart(unsigned int x, unsigned int y)
{
  if (x <= g_BasicRenderer.GetWidth() && y <= g_BasicRenderer.GetHeight())
  {
    statLoggingCursor.X = x;
    defaultX = x;
    statLoggingCursor.Y = y;
  }
}

void showSuccess(const char *message)
{
  uint32_t tmpColor = g_BasicRenderer.GetColor();
  uint32_t tmpX = g_BasicRenderer.GetCursorX();
  uint32_t tmpY = g_BasicRenderer.GetCursorY();

  g_BasicRenderer.SetCursor(statLoggingCursor.X, statLoggingCursor.Y);

  g_BasicRenderer.SetColor(BR_WHITE);
  g_BasicRenderer.Print("[ ");
  g_BasicRenderer.SetColor(BR_GREEN);
  g_BasicRenderer.Print("OK");
  g_BasicRenderer.SetColor(BR_WHITE);
  g_BasicRenderer.Print(" ] ");
  g_BasicRenderer.Print(message);
  g_BasicRenderer.NewLine();

  g_BasicRenderer.SetColor(tmpColor);
  statLoggingCursor.X = g_BasicRenderer.GetCursorX();
  statLoggingCursor.Y = g_BasicRenderer.GetCursorY();
  g_BasicRenderer.SetCursor(tmpX, tmpY);
}

void showFailed(const char *message)
{
  uint32_t tmpColor = g_BasicRenderer.GetColor();
  uint32_t tmpX = g_BasicRenderer.GetCursorX();
  uint32_t tmpY = g_BasicRenderer.GetCursorY();

  g_BasicRenderer.SetCursor(statLoggingCursor.X, statLoggingCursor.Y);

  g_BasicRenderer.SetColor(BR_WHITE);
  g_BasicRenderer.Print("[ ");
  g_BasicRenderer.SetColor(BR_RED);
  g_BasicRenderer.Print("NOK");
  g_BasicRenderer.SetColor(BR_WHITE);
  g_BasicRenderer.Print(" ] ");
  g_BasicRenderer.Print(message);
  g_BasicRenderer.NewLine();

  g_BasicRenderer.SetColor(tmpColor);
  statLoggingCursor.X = g_BasicRenderer.GetCursorX();
  statLoggingCursor.Y = g_BasicRenderer.GetCursorY();
  g_BasicRenderer.SetCursor(tmpX, tmpY);
}

void showInfo(const char *message)
{
  uint32_t tmpColor = g_BasicRenderer.GetColor();
  uint32_t tmpX = g_BasicRenderer.GetCursorX();
  uint32_t tmpY = g_BasicRenderer.GetCursorY();

  g_BasicRenderer.SetCursor(statLoggingCursor.X, statLoggingCursor.Y);

  g_BasicRenderer.SetColor(BR_WHITE);
  g_BasicRenderer.Print("[ * ] ");
  g_BasicRenderer.Print(message);
  g_BasicRenderer.NewLine();

  g_BasicRenderer.SetColor(tmpColor);
  statLoggingCursor.X = g_BasicRenderer.GetCursorX();
  statLoggingCursor.Y = g_BasicRenderer.GetCursorY();
  g_BasicRenderer.SetCursor(tmpX, tmpY);
}

void printStats(const char *message, uint32_t color)
{
  uint32_t tmpColor = g_BasicRenderer.GetColor();
  uint32_t tmpX = g_BasicRenderer.GetCursorX();
  uint32_t tmpY = g_BasicRenderer.GetCursorY();

  g_BasicRenderer.SetCursor(statLoggingCursor.X, statLoggingCursor.Y);

  g_BasicRenderer.SetColor(color);
  g_BasicRenderer.Print(message);

  g_BasicRenderer.SetColor(tmpColor);
  statLoggingCursor.X = g_BasicRenderer.GetCursorX();
  statLoggingCursor.Y = g_BasicRenderer.GetCursorY();
  g_BasicRenderer.SetCursor(tmpX, tmpY);
}

void statNewLine()
{
  statLoggingCursor.X = defaultX;
  statLoggingCursor.Y += 16;
}