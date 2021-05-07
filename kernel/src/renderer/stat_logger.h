#pragma once

#include "basic_renderer.h"

void setLoggerStart(unsigned int x, unsigned int y);
void showSuccess(const char *message);
void showFailed(const char *message);
void printStats(const char *message, uint32_t color=BR_DARK_CYAN);
void statNewLine();