#include "basic_renderer.h"

namespace BasicRenderer
{
  void Renderer::SetPix(uint32_t x, uint32_t y, uint32_t color)
	{
		if (x > frameBuffer->Width || y > frameBuffer->Height)
			return;
		*(uint32_t *)((uint64_t)frameBuffer->BaseAddress + (x * 4) + (y * frameBuffer->PixelsPerScanline * 4)) = color;
	}

  void Renderer::SetFont(PSF1_FONT *_font)
	{
		if (_font == NULL)
			return;

		font = _font;
	}

	void Renderer::SetColor(uint32_t _color)
	{
		color = _color;
	}

	void Renderer::SetBackColor(uint32_t _color)
	{
		clearColor = _color;
	}

	void Renderer::SetFramebuffer(FrameBuffer *_frameBuffer)
	{
		if (_frameBuffer == NULL)
			return;

		frameBuffer = _frameBuffer;
	}

  void Renderer::SetCursor(unsigned int x, unsigned int y)
	{
		if (x < 0 || y < 0)
			return;

		textCursorPos = {x, y};
		baseXOff = x;
	}
}