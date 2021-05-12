#include "panic.h"
#include "../renderer/basic_renderer.h"

void Panic(const char *message)
{
  g_BasicRenderer.ClearScreen(BR_RED);
  g_BasicRenderer.SetCursor(400, 300);
  g_BasicRenderer.SetColor(BR_BLACK);
  g_BasicRenderer.Print("Kernel Panic");
  g_BasicRenderer.NewLine();
  g_BasicRenderer.NewLine();
  g_BasicRenderer.Print(message);
}