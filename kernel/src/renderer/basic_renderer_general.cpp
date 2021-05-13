#include "basic_renderer.h"

namespace BasicRenderer
{
	Renderer g_Renderer(NULL, NULL, BR_BLACK, BR_BLACK);

	void InitGlobalBasicRenderer(FrameBuffer *_frameBuffer, PSF1_FONT *_font, uint32_t _frontColor, uint32_t _backColor)
	{
		g_Renderer.SetFramebuffer(_frameBuffer);
		g_Renderer.SetFont(_font);
		g_Renderer.SetColor(_frontColor);
		g_Renderer.SetBackColor(_backColor);
	}

	Renderer::Renderer(FrameBuffer *_frameBuffer, PSF1_FONT *_font, uint32_t _frontColor, uint32_t _backColor) : frameBuffer(_frameBuffer),
																																																							 font(_font),
																																																							 color(_frontColor),
																																																							 clearColor(_backColor)
	{
		baseXOff = 0;
		textCursorPos = {0, 0};
	}

	void Renderer::ClearScreen()
	{
		ClearScreen(clearColor);
	}

	void Renderer::ClearScreen(uint32_t _color)
	{
		uint64_t fbBase = (uint64_t)frameBuffer->BaseAddress;
		uint64_t bytesPerScanline = frameBuffer->PixelsPerScanline * 4;
		uint64_t fbHeight = frameBuffer->Height;

		for (uint64_t vertScanLine = 0; vertScanLine < fbHeight; vertScanLine++)
		{
			uint64_t pixPtrBase = fbBase + (bytesPerScanline * vertScanLine);
			for (uint32_t *pixPtr = (uint32_t *)pixPtrBase; pixPtr < (uint32_t *)(pixPtrBase + bytesPerScanline); pixPtr++)
			{
				*pixPtr = _color;
			}
		}
	}

	void Renderer::ClearChar()
	{
		if (font == NULL || frameBuffer == NULL)
			return;

		if (textCursorPos.X <= baseXOff)
		{
			textCursorPos.X = frameBuffer->Width;
			textCursorPos.Y -= 16;
			if (textCursorPos.Y < 0)
				textCursorPos.Y = 0;
		}

		unsigned int xOff = textCursorPos.X;
		unsigned int yOff = textCursorPos.Y;

		unsigned int *pixPtr = (unsigned int *)frameBuffer->BaseAddress;
		for (unsigned long y = yOff; y < yOff + 16; y++)
		{
			for (unsigned long x = xOff - 8; x < xOff; x++)
			{
				*(unsigned int *)(pixPtr + x + (y * frameBuffer->PixelsPerScanline)) = clearColor;
			}
		}

		textCursorPos.X -= 8;
		if (textCursorPos.X < baseXOff)
		{
			textCursorPos.X = frameBuffer->Width;
			textCursorPos.Y -= 16;
			if (textCursorPos.Y < 0)
			{
				textCursorPos.Y = 0;
			}
		}
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

	void Renderer::DrawOverlayMouseCursor(uint8_t *mouseCursor, Point position, uint32_t color)
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

	void Renderer::NewLine()
	{
		textCursorPos.X = baseXOff;
		textCursorPos.Y += 16;
	}

	void Renderer::Print(const char *str)
	{
		if (font == NULL || frameBuffer == NULL)
			return;

		char *chr = (char *)str;
		while (*chr != 0)
		{
			if (*chr == '\n')
			{
				chr++;
				textCursorPos.Y += 16;
				textCursorPos.X = baseXOff;
				continue;
			}

			PutChar(*chr, textCursorPos.X, textCursorPos.Y);
			textCursorPos.X += 8;

			if (textCursorPos.X + 8 > frameBuffer->Width)
			{
				textCursorPos.X = baseXOff;
				textCursorPos.Y += 16;
			}

			chr++;
		}
	}

	void Renderer::PutChar(char chr, unsigned int xOff, unsigned int yOff)
	{
		if (font == NULL || frameBuffer == NULL)
			return;

		unsigned int *pixPtr = (unsigned int *)frameBuffer->BaseAddress;
		char *fontPtr = (char *)font->glyphBuffer + (chr * font->psf1_header->charsize);
		for (unsigned long y = yOff; y < yOff + 16; y++)
		{
			if (y > frameBuffer->Height)
				break;

			for (unsigned long x = xOff; x < xOff + 8; x++)
			{
				if (x > frameBuffer->Width)
					break;

				if ((*fontPtr & (0b10000000 >> (x - xOff))) > 0)
				{
					*(unsigned int *)(pixPtr + x + (y * frameBuffer->PixelsPerScanline)) = color;
				}
			}

			fontPtr++;
		}
	}

	void Renderer::PutChar(char chr)
	{
		if (font == NULL || frameBuffer == NULL)
			return;

		if (chr == '\n')
		{
			textCursorPos.Y += 16;
			textCursorPos.X = baseXOff;
			return;
		}

		PutChar(chr, textCursorPos.X, textCursorPos.Y);

		textCursorPos.X += 8;
		if (textCursorPos.X + 8 > frameBuffer->Width)
		{
			textCursorPos.X = baseXOff;
			textCursorPos.Y += 16;
		}
	}
}