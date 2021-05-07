#pragma once

#include <stdint.h>

void WriteBus(uint16_t port, uint8_t value);
uint8_t ReadBus(uint16_t port);
void IOWait();