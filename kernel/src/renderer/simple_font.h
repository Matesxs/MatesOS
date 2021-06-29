#pragma once

#define PSF1_MAGIC0 0x36
#define PSF1_MAGIC1 0x04

struct PSF1_HEADER
{
	uint8_t magic[2];
	uint8_t mode;
	uint8_t charsize;
};

struct PSF1_FONT
{
	PSF1_HEADER psf1_header;
	uint8_t glyphBuffer[16 * 256];
};