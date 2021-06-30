#pragma once

#include "stddef.h"
#include "stdint.h"

#define BITMAP_SCALE	sizeof(size_t)

struct Bitmap {
	uint8_t*	buffer;	
	size_t		size;	// Real size
};

// *** Class functions  *** //

// Initialize [bitmap] with real [size] of [buffer]. Will also clear buffer.
void bitmap_initialize(Bitmap* bitmap, void* buffer, size_t size);

// Gets the adjusted [bitmap] size, or 8 times [bitmap] size
size_t bitmap_adjusted_size(Bitmap* bitmap);

// Get value based on [index] for the [bitmap]
bool bitmap_get(Bitmap* bitmap, size_t index);

// Set [value] based on [index] for the [bitmap] and returns true on success
bool bitmap_set(Bitmap* bitmap, size_t index, bool value);