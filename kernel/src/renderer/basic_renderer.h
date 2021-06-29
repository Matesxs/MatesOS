#pragma once

#include "../utils/math.h"
#include "framebuffer.h"
#include "simple_font.h"
#include "br_color.h"
#include <stdint.h>

namespace BasicRenderer
{
  void InitGlobalBasicRenderer(FrameBuffer *_frameBuffer, PSF1_FONT *_font, Color _frontColor, Color _backColor);

  class Renderer
  {
  public:
    Renderer();
    Renderer(FrameBuffer *_frameBuffer, PSF1_FONT *_font, Color _frontColor, Color _backColor);

    void SetFramebuffer(FrameBuffer *_frameBuffer);
    void SetCursor(unsigned int x, unsigned int y);
    void SetFont(PSF1_FONT *_font);
    void SetColor(Color _color);
    void SetBackColor(Color _color);
    void SetPix(uint32_t x, uint32_t y, Color color);
    void SetPix(uint32_t x, uint32_t y, uint32_t color);

    FrameBuffer GetFramebuffer();
    unsigned int GetCursorX();
    unsigned int GetCursorY();
    uint64_t GetHeight();
    uint64_t GetWidth();
    Color GetColor();
    Color GetBackColor();
    uint32_t GetPix(uint32_t x, uint32_t y);

    void Print(const char *str);
    void PutChar(char chr, unsigned int xOff, unsigned int yOff);
    void PutChar(char chr);
    void ClearScreen();
    void ClearScreen(Color _color);
    void ClearChar();
    void NewLine();
    
    void ClearMouseCursor(uint8_t *mouseCursor, Point position);
    void DrawOverlayMouseCursor(uint8_t *mouseCursor, Point position, Color color);

  private:
    uint32_t mouseCursorBuffer[16 * 16];
    uint32_t mouseCursorBufferAfter[16 * 16];
    bool MouseDrawn;
    unsigned int baseXOff;
    Point textCursorPos;
    Color color;
    Color clearColor;
    FrameBuffer *frameBuffer;
    PSF1_FONT *font;
  };

  extern Renderer g_Renderer;
}