#pragma once

#include "stddef.h"
#include "stdint.h"

struct FrameBuffer
{
	void *BaseAddress;
	size_t BufferSize;
	uint32_t Width;
	uint32_t Height;
	uint32_t PixPitch;
};