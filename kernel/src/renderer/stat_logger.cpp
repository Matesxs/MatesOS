#include "stat_logger.h"

Point statLoggingCursor = {0, 0};
uint32_t defaultX = 0;

void setLoggerStart(unsigned int x, unsigned int y)
{
  if (x <= GlobalBasicRenderer.GetWidth() && y <= GlobalBasicRenderer.GetHeight())
  {
    statLoggingCursor.X = x;
    defaultX = x;
    statLoggingCursor.Y = y;
  }
}

void showSuccess(const char *message)
{
  uint32_t tmpColor = GlobalBasicRenderer.GetColor();
  uint32_t tmpX = GlobalBasicRenderer.GetCursorX();
  uint32_t tmpY = GlobalBasicRenderer.GetCursorY();

  GlobalBasicRenderer.SetCursor(statLoggingCursor.X, statLoggingCursor.Y);

  GlobalBasicRenderer.SetColor(BR_WHITE);
  GlobalBasicRenderer.Print("[ ");
  GlobalBasicRenderer.SetColor(BR_GREEN);
  GlobalBasicRenderer.Print("OK");
  GlobalBasicRenderer.SetColor(BR_WHITE);
  GlobalBasicRenderer.Print(" ] ");
  GlobalBasicRenderer.Print(message);
  GlobalBasicRenderer.NewLine();

  GlobalBasicRenderer.SetColor(tmpColor);
  statLoggingCursor.X = GlobalBasicRenderer.GetCursorX();
  statLoggingCursor.Y = GlobalBasicRenderer.GetCursorY();
  GlobalBasicRenderer.SetCursor(tmpX, tmpY);
}

void showFailed(const char *message)
{
  uint32_t tmpColor = GlobalBasicRenderer.GetColor();
  uint32_t tmpX = GlobalBasicRenderer.GetCursorX();
  uint32_t tmpY = GlobalBasicRenderer.GetCursorY();

  GlobalBasicRenderer.SetCursor(statLoggingCursor.X, statLoggingCursor.Y);

  GlobalBasicRenderer.SetColor(BR_WHITE);
  GlobalBasicRenderer.Print("[ ");
  GlobalBasicRenderer.SetColor(BR_RED);
  GlobalBasicRenderer.Print("NOK");
  GlobalBasicRenderer.SetColor(BR_WHITE);
  GlobalBasicRenderer.Print(" ] ");
  GlobalBasicRenderer.Print(message);
  GlobalBasicRenderer.NewLine();

  GlobalBasicRenderer.SetColor(tmpColor);
  statLoggingCursor.X = GlobalBasicRenderer.GetCursorX();
  statLoggingCursor.Y = GlobalBasicRenderer.GetCursorY();
  GlobalBasicRenderer.SetCursor(tmpX, tmpY);
}

void showInfo(const char *message)
{
  uint32_t tmpColor = GlobalBasicRenderer.GetColor();
  uint32_t tmpX = GlobalBasicRenderer.GetCursorX();
  uint32_t tmpY = GlobalBasicRenderer.GetCursorY();

  GlobalBasicRenderer.SetCursor(statLoggingCursor.X, statLoggingCursor.Y);

  GlobalBasicRenderer.SetColor(BR_WHITE);
  GlobalBasicRenderer.Print("[ * ] ");
  GlobalBasicRenderer.Print(message);
  GlobalBasicRenderer.NewLine();

  GlobalBasicRenderer.SetColor(tmpColor);
  statLoggingCursor.X = GlobalBasicRenderer.GetCursorX();
  statLoggingCursor.Y = GlobalBasicRenderer.GetCursorY();
  GlobalBasicRenderer.SetCursor(tmpX, tmpY);
}

void printStats(const char *message, uint32_t color)
{
  uint32_t tmpColor = GlobalBasicRenderer.GetColor();
  uint32_t tmpX = GlobalBasicRenderer.GetCursorX();
  uint32_t tmpY = GlobalBasicRenderer.GetCursorY();

  GlobalBasicRenderer.SetCursor(statLoggingCursor.X, statLoggingCursor.Y);

  GlobalBasicRenderer.SetColor(color);
  GlobalBasicRenderer.Print(message);

  GlobalBasicRenderer.SetColor(tmpColor);
  statLoggingCursor.X = GlobalBasicRenderer.GetCursorX();
  statLoggingCursor.Y = GlobalBasicRenderer.GetCursorY();
  GlobalBasicRenderer.SetCursor(tmpX, tmpY);
}

void statNewLine()
{
  statLoggingCursor.X = defaultX;
  statLoggingCursor.Y += 16;
}