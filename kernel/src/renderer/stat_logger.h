#pragma once

#include "basic_renderer.h"
#include "../library/string.h"

void showSuccess(const char *message);
void showFailed(const char *message);
void showWarning(const char *message);
void showInfo(const char *message);
void printStats(const char *message, uint32_t color=BasicRenderer::BR_DARK_CYAN);
void printStatsSpacing(uint32_t color=BasicRenderer::BR_DARK_CYAN);
void statNewLine();
void ShowOSStats();
void ShowOSStats(unsigned int x, unsigned int y);