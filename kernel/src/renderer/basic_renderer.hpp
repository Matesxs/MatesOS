#pragma once

#include "../utils/math.hpp"
#include "framebuffer.hpp"
#include "simple_font.hpp"
#include "br_color.hpp"
#include <stdint.h>

namespace BasicRenderer
{
  void InitGlobalBasicRenderer(FrameBuffer *_frameBuffer, Point _cursor, PSF1_FONT *_font, uint32_t _fontWidth, uint32_t _fontHeight, Color _frontColor, Color _backColor);

  class Renderer
  {
  public:
    Renderer();
    Renderer(FrameBuffer *_frameBuffer, Point cursor, PSF1_FONT *_font, uint32_t _fontWidth, uint32_t _fontHeight, Color _frontColor, Color _backColor);

    void SetFramebuffer(FrameBuffer *_frameBuffer);
    void SetCursor(unsigned int x, unsigned int y);
    void SetCursor(Point cursor);
    void SetFont(PSF1_FONT *_font, uint32_t _fontWidth, uint32_t _fontHeight);
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
    Point GetGrid();

    void Print(const char *str);
    void PutChar(char chr);
    void ClearScreen();
    void ClearScreen(Color _color);
    void ClearChar();
    void NewLine();
    void Scroll();

    void ClearMouseCursor(uint8_t *mouseCursor, Point position);
    void DrawOverlayMouseCursor(uint8_t *mouseCursor, Point position, Color color);

  private:
    void SetPix(long x, long y, uint32_t color);

    uint32_t fontWidth;
    uint32_t fontHeight;
    PSF1_FONT *font;

    FrameBuffer *frameBuffer;
    uint32_t xGridMax;
    uint32_t yGridMax;

    uint32_t mouseCursorBuffer[16 * 16];
    uint32_t mouseCursorBufferAfter[16 * 16];
    bool MouseDrawn;
    unsigned int baseXOff;

    Point textCursorPos;
    Color color;
    Color clearColor;
  };

  extern Renderer g_Renderer;
}