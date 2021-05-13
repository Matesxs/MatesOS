#pragma once

#include "../utils/math.h"
#include "framebuffer.h"
#include "simple_font.h"
#include "br_color.h"
#include <stdint.h>

namespace BasicRenderer
{
  void InitGlobalBasicRenderer(FrameBuffer *_frameBuffer, PSF1_FONT *_font, uint32_t _frontColor, uint32_t _backColor);

  class Renderer
  {
  public:
    Renderer(FrameBuffer *_frameBuffer, PSF1_FONT *_font, uint32_t _frontColor, uint32_t _backColor);

    void SetFramebuffer(FrameBuffer *_frameBuffer);
    void SetCursor(unsigned int x, unsigned int y);
    void SetFont(PSF1_FONT *_font);
    void SetColor(uint32_t _color);
    void SetBackColor(uint32_t _color);
    void PutPix(uint32_t x, uint32_t y, uint32_t color);

    FrameBuffer GetFramebuffer();
    unsigned int GetCursorX();
    unsigned int GetCursorY();
    uint64_t GetHeight();
    uint64_t GetWidth();
    uint32_t GetColor();
    uint32_t GetBackColor();
    uint32_t GetPix(uint32_t x, uint32_t y);

    void Print(const char *str);
    void PutChar(char chr, unsigned int xOff, unsigned int yOff);
    void PutChar(char chr);
    void ClearScreen();
    void ClearScreen(uint32_t _color);
    void ClearChar();
    void NewLine();
    
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

  extern Renderer g_Renderer;
}