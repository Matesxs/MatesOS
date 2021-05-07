#include "panic.h"
#include "../renderer/basic_renderer.h"

void Panic(const char *message)
{
  GlobalBasicRenderer.ClearScreen(BR_RED);
  GlobalBasicRenderer.SetCursor(400, 300);
  GlobalBasicRenderer.SetColor(BR_BLACK);
  GlobalBasicRenderer.Print("Kernel Panic");
  GlobalBasicRenderer.NewLine();
  GlobalBasicRenderer.NewLine();
  GlobalBasicRenderer.Print(message);
}