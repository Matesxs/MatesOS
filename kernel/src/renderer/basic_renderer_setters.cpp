#include "basic_renderer.hpp"

namespace BasicRenderer
{
  void Renderer::SetPix(uint32_t x, uint32_t y, uint32_t color)
  {
    if (x >= frameBuffer->Width || y >= frameBuffer->Height)
      return;

    ((uint32_t*)frameBuffer->BaseAddress)[x + y * frameBuffer->PixelsPerScanline] = color;
  }

  void Renderer::SetPix(long x, long y, uint32_t color)
  {
    if (x < 0 || y < 0) return;
    SetPix((uint32_t)x, (uint32_t)y, color);
  }

  void Renderer::SetPix(uint32_t x, uint32_t y, Color color)
  {
    SetPix(x, y, fromColor(color));
  }

  void Renderer::SetFont(PSF1_FONT *_font, uint32_t _fontWidth, uint32_t _fontHeight)
  {
    if (_font == NULL)
      return;

    if (_fontWidth > frameBuffer->Width || _fontHeight > frameBuffer->Height)
      return;

    font = _font;
    fontWidth = _fontWidth;
    fontHeight = _fontHeight;

    if (frameBuffer)
    {
      xGridMax = frameBuffer->Width / fontWidth;
      yGridMax = frameBuffer->Height / fontHeight;
    }
  }

  void Renderer::SetColor(Color _color)
  {
    color = _color;
  }

  void Renderer::SetBackColor(Color _color)
  {
    clearColor = _color;
  }

  void Renderer::SetFramebuffer(FrameBuffer *_frameBuffer)
  {
    if (_frameBuffer == NULL)
      return;

    frameBuffer = _frameBuffer;

    if (font)
    {
      xGridMax = frameBuffer->Width / fontWidth;
      yGridMax = frameBuffer->Height / fontHeight;
    }
  }

  void Renderer::SetCursor(unsigned int x, unsigned int y)
  {
    if (x < 0 || x >= xGridMax || y < 0 || y >= yGridMax)
      return;

    textCursorPos = {x, y};
    baseXOff = x;
  }

  void Renderer::SetCursor(Point cursor)
  {
    SetCursor(cursor.X, cursor.Y);
  }
}