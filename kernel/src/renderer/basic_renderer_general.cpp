#include "basic_renderer.h"
#include "../library/memory.h"

namespace BasicRenderer
{
	Renderer g_Renderer;

	void InitGlobalBasicRenderer(FrameBuffer *_frameBuffer, PSF1_FONT *_font, Color _frontColor, Color _backColor)
	{
		g_Renderer.SetFramebuffer(_frameBuffer);
		g_Renderer.SetFont(_font);
		g_Renderer.SetColor(_frontColor);
		g_Renderer.SetBackColor(_backColor);
	}

	Renderer::Renderer(){};

	Renderer::Renderer(FrameBuffer *_frameBuffer, PSF1_FONT *_font, Color _frontColor, Color _backColor)
	{
		SetFramebuffer(_frameBuffer);
		SetFont(_font);
		SetColor(_frontColor);
		SetBackColor(_backColor);
		baseXOff = 0;
		textCursorPos = {0, 0};
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
				SetPix(x, y, clearColor);
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

		for (unsigned long y = yOff; y < yOff + 16; y++)
		{
			for (unsigned long x = xOff - 8; x < xOff; x++)
			{
				SetPix(x, y, clearColor);
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
				NewLine();
				continue;
			}

			if (textCursorPos.X + 8 > frameBuffer->Width)
			{
				NewLine();
			}

			PutChar(*chr, textCursorPos.X, textCursorPos.Y);
			textCursorPos.X += 8;

			chr++;
		}
	}

	void Renderer::PutChar(char chr, unsigned int xOff, unsigned int yOff)
	{
		if (font == NULL || frameBuffer == NULL)
			return;

		char *fontPtr = (char *)font->glyphBuffer + (chr * font->psf1_header.charsize);
		for (unsigned long y = yOff; y < yOff + font->psf1_header.charsize; y++)
		{
			if (y > frameBuffer->Height)
				break;

			for (unsigned long x = xOff; x < xOff + 8; x++)
			{
				if (x > frameBuffer->Width)
					break;

				if ((*fontPtr & (0b10000000 >> (x - xOff))) > 0)
					SetPix(x, y, color);
				else
					SetPix(x, y, clearColor);
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
			NewLine();
		}
	}
}