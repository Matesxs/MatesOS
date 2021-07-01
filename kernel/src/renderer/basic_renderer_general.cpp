#include "basic_renderer.hpp"
#include "../library/memory.hpp"

namespace BasicRenderer
{
  Renderer g_Renderer;

  void InitGlobalBasicRenderer(FrameBuffer *_frameBuffer, Point _cursor, PSF1_FONT *_font, uint32_t _fontWidth, uint32_t _fontHeight, Color _frontColor, Color _backColor)
  {
    g_Renderer.SetFramebuffer(_frameBuffer);
    g_Renderer.SetFont(_font, _fontWidth, _fontHeight);
    g_Renderer.SetColor(_frontColor);
    g_Renderer.SetBackColor(_backColor);
    g_Renderer.SetCursor(_cursor);
  }

  Renderer::Renderer(){};

  Renderer::Renderer(FrameBuffer *_frameBuffer, Point cursor, PSF1_FONT *_font, uint32_t _fontWidth, uint32_t _fontHeight, Color _frontColor, Color _backColor)
  {
    SetFramebuffer(_frameBuffer);
    SetFont(_font, _fontWidth, _fontHeight);
    SetColor(_frontColor);
    SetBackColor(_backColor);
    SetCursor(cursor);
  }

  void Renderer::ClearScreen()
  {
    ClearScreen(clearColor);
  }

  void Renderer::ClearScreen(Color _color)
  {
    if (frameBuffer == NULL)
      return;

    for (uint32_t x = 0; x < frameBuffer->Width; x++)
    {
      for (uint32_t y = 0; y < frameBuffer->Height; y++)
      {
        SetPix(x, y, _color);
      }
    }
  }

  void Renderer::ClearChar()
  {
    if (font == NULL || frameBuffer == NULL)
      return;

    if (textCursorPos.X <= baseXOff)
    {
      textCursorPos.X = xGridMax;
      textCursorPos.Y -= 1;
      if (textCursorPos.Y < 0)
        textCursorPos.Y = 0;
    }

    unsigned int xOff = textCursorPos.X * fontWidth;
    unsigned int yOff = textCursorPos.Y * fontHeight;

    for (unsigned long y = yOff; y < yOff + fontHeight; y++)
    {
      for (unsigned long x = xOff - fontWidth; x < xOff; x++)
      {
        SetPix(x, y, clearColor);
      }
    }

    textCursorPos.X -= 1;
    if (textCursorPos.X < baseXOff)
    {
      textCursorPos.X = xGridMax;
      textCursorPos.Y -= 1;
      if (textCursorPos.Y < 0)
      {
        textCursorPos.Y = 0;
      }
    }
  }

  void Renderer::NewLine()
  {
    PutChar('\n');
  }

  void Renderer::Print(const char *str)
  {
    if (font == NULL || frameBuffer == NULL)
      return;

    char *chr = (char *)str;
    while (*chr != 0)
    {
      PutChar(*chr);
      chr++;
    }
  }

  void Renderer::Scroll()
  {
    if (frameBuffer == NULL || font == NULL) return;

    textCursorPos.Y--;

    size_t rowSize = frameBuffer->PixelsPerScanline * fontHeight;
    size_t screenSize = rowSize * yGridMax;

    size_t i = 0;
    for (; i < screenSize - rowSize; i++)
    {
      ((uint32_t*)frameBuffer->BaseAddress)[i] = ((uint32_t*)frameBuffer->BaseAddress)[i + rowSize];
    }

    for (size_t j = 0; j < rowSize; j++)
    {
      ((uint32_t*)frameBuffer->BaseAddress)[i + j] = fromColor(clearColor);
    }
  }

  void Renderer::PutChar(char chr)
  {
    if (font == NULL || frameBuffer == NULL)
      return;

    // while (textCursorPos.Y >= yGridMax) Scroll();

    long x_start_pix = textCursorPos.X * fontWidth;
    long y_start_pix = textCursorPos.Y * fontHeight;

    char *fontPtr = (char *)font->glyphBuffer + (chr * fontHeight);

    if (chr == '\n')
    {
      goto newline;
    }

    for (long y = y_start_pix; y < y_start_pix + fontHeight; y++)
    {
      for (long x = x_start_pix; x < x_start_pix + fontWidth; x++)
      {
        if ((*fontPtr & (0b10000000 >> (x - x_start_pix))) > 0)
          SetPix(x, y, color);
        else
          SetPix(x, y, clearColor);
      }

      fontPtr++;
    }

    textCursorPos.X += 1;
    if (textCursorPos.X + 1 > xGridMax)
    {
newline:
      textCursorPos.Y += 1;
      textCursorPos.X = baseXOff;
    }

    while (textCursorPos.Y >= yGridMax) Scroll();
  }

  void Renderer::ClearMouseCursor(uint8_t *mouseCursor, Point position)
  {
    if (!MouseDrawn)
      return;

    int xMax = 16;
    int yMax = 16;
    int differenceX = frameBuffer->Width - position.X;
    int differenceY = frameBuffer->Height - position.Y;

    if (differenceX < 16)
      xMax = differenceX;
    if (differenceY < 16)
      yMax = differenceY;

    for (int y = 0; y < yMax; y++)
    {
      for (int x = 0; x < xMax; x++)
      {
        int bit = y * 16 + x;
        int byte = bit / 8;

        if ((mouseCursor[byte] & (0b10000000 >> (x % 8))))
        {
          if (GetPix(position.X + x, position.Y + y) == mouseCursorBufferAfter[x + y * 16])
          {
            SetPix(position.X + x, position.Y + y, mouseCursorBuffer[x + y * 16]);
          }
        }
      }
    }
  }

  void Renderer::DrawOverlayMouseCursor(uint8_t *mouseCursor, Point position, Color color)
  {
    int xMax = 16;
    int yMax = 16;
    int differenceX = frameBuffer->Width - position.X;
    int differenceY = frameBuffer->Height - position.Y;

    if (differenceX < 16)
      xMax = differenceX;
    if (differenceY < 16)
      yMax = differenceY;

    for (int y = 0; y < yMax; y++)
    {
      for (int x = 0; x < xMax; x++)
      {
        int bit = y * 16 + x;
        int byte = bit / 8;

        if ((mouseCursor[byte] & (0b10000000 >> (x % 8))))
        {
          mouseCursorBuffer[x + y * 16] = GetPix(position.X + x, position.Y + y);
          SetPix(position.X + x, position.Y + y, color);
          mouseCursorBufferAfter[x + y * 16] = GetPix(position.X + x, position.Y + y);
        }
      }
    }

    MouseDrawn = true;
  }
}