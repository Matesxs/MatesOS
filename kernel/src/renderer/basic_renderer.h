#pragma once

#include "../utils/math.h"
#include "framebuffer.h"
#include "simple_font.h"
#include "br_color.h"
#include <stdint.h>

void InitGlobalBasicRenderer(FrameBuffer *_frameBuffer, PSF1_FONT *_font, uint32_t _frontColor, uint32_t _backColor);

class BasicRenderer
{
  public:
  BasicRenderer(FrameBuffer *_frameBuffer, PSF1_FONT *_font, uint32_t _frontColor, uint32_t _backColor);
  void SetFramebuffer(FrameBuffer *_frameBuffer);
  FrameBuffer GetFramebuffer();
  void SetCursor(unsigned int x, unsigned int y);
  unsigned int GetCursorX();
  unsigned int GetCursorY();
  uint64_t GetHeight();
  uint64_t GetWidth();
  void SetFont(PSF1_FONT *_font);
  void SetColor(uint32_t _color);
  void SetBackColor(uint32_t _color);
  uint32_t GetColor();
  uint32_t GetBackColor();
  void Print(const char *str);
  void NewLine();
  void PutChar(char chr, unsigned int xOff, unsigned int yOff);
  void PutChar(char chr);
  void ClearScreen();
  void ClearScreen(uint32_t _color);
  void ClearChar();
  void PutPix(uint32_t x, uint32_t y, uint32_t color);
  uint32_t GetPix(uint32_t x, uint32_t y);
  void ClearMouseCursor(uint8_t *mouseCursor, Point position);
  void DrawOverlayMouseCursor(uint8_t *mouseCursor, Point position, uint32_t color);

  private:
  uint32_t mouseCursorBuffer[16 * 16];
  uint32_t mouseCursorBufferAfter[16 * 16];
  bool MouseDrawn;
  unsigned int baseXOff;
  Point textCursorPos;
  unsigned int color;
  unsigned int clearColor;
  FrameBuffer *frameBuffer;
  PSF1_FONT *font;
};

extern BasicRenderer g_BasicRenderer;