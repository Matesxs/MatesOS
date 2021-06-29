#include "basic_renderer.h"

namespace BasicRenderer
{
  void Renderer::SetPix(uint32_t x, uint32_t y, uint32_t color)
	{
		if (x > frameBuffer->Width || y > frameBuffer->Height)
			return;
		((uint32_t*)frameBuffer->BaseAddress)[y * (frameBuffer->PixPitch / sizeof(uint32_t)) + x] = color;
	}

	void Renderer::SetPix(uint32_t x, uint32_t y, Color color)
	{
		SetPix(x, y, fromColor(color));
	}

  void Renderer::SetFont(PSF1_FONT *_font)
	{
		if (_font == NULL)
			return;

		font = _font;
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
	}

  void Renderer::SetCursor(unsigned int x, unsigned int y)
	{
		if (x < 0 || y < 0)
			return;

		textCursorPos = {x, y};
		baseXOff = x;
	}
}