#pragma once

#include "basic_renderer.h"

void setLoggerStart(unsigned int x, unsigned int y);
void showSuccess(const char *message);
void showFailed(const char *message);
void showInfo(const char *message);
void printStats(const char *message, uint32_t color=BasicRenderer::BR_DARK_CYAN);
void statNewLine();
void ShowStats();