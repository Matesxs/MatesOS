#include "basic_renderer.h"

namespace BasicRenderer
{
  unsigned int Renderer::GetCursorX()
	{
		return textCursorPos.X;
	}

	unsigned int Renderer::GetCursorY()
	{
		return textCursorPos.Y;
	}

	uint64_t Renderer::GetHeight()
	{
		return frameBuffer->Height;
	}

	uint64_t Renderer::GetWidth()
	{
		return frameBuffer->Width;
	}

  Color Renderer::GetColor()
	{
		return color;
	}

	Color Renderer::GetBackColor()
	{
		return clearColor;
	}

  FrameBuffer Renderer::GetFramebuffer()
	{
		return *frameBuffer;
	}

  uint32_t Renderer::GetPix(uint32_t x, uint32_t y)
	{
		if (x > frameBuffer->Width || y > frameBuffer->Height)
			return fromColor(BR_BLACK);
		return ((uint32_t*)frameBuffer->BaseAddress)[y * (frameBuffer->PixPitch / sizeof(uint32_t)) + x];
	}
}