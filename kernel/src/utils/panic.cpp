#include "panic.hpp"
#include "../renderer/basic_renderer.hpp"
#include "helpers.hpp"

__attribute__((noreturn))
void Panic(const char *message)
{
  asm ("cli");

  BasicRenderer::g_Renderer.SetBackColor(BasicRenderer::BR_RED);
  BasicRenderer::g_Renderer.ClearScreen();
  BasicRenderer::g_Renderer.SetCursor(20, 10);
  BasicRenderer::g_Renderer.SetColor(BasicRenderer::BR_BLACK);
  BasicRenderer::g_Renderer.Print("Kernel Panic");
  BasicRenderer::g_Renderer.NewLine();
  BasicRenderer::g_Renderer.NewLine();
  BasicRenderer::g_Renderer.Print(message);
  
  halt();
  while (true);
}

void __unreachable_code()
{
  Panic("Unreachable code hit");
}