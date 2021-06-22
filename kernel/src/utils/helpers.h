#pragma once

#include "stddef.h"
#include "stdint.h"

void halt();
uint8_t checksum(const char *addr, size_t size);