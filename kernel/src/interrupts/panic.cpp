#include "panic.h"
#include "../renderer/basic_renderer.h"

void Panic(const char *message)
{
  BasicRenderer::g_Renderer.ClearScreen(BasicRenderer::BR_RED);
  BasicRenderer::g_Renderer.SetCursor(400, 300);
  BasicRenderer::g_Renderer.SetColor(BasicRenderer::BR_BLACK);
  BasicRenderer::g_Renderer.Print("Kernel Panic");
  BasicRenderer::g_Renderer.NewLine();
  BasicRenderer::g_Renderer.NewLine();
  BasicRenderer::g_Renderer.Print(message);
}