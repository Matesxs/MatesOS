#pragma once

#include "basic_renderer.hpp"
#include "../library/string.hpp"

void showSuccess(const char *message);
void showFailed(const char *message);
void showWarning(const char *message);
void showInfo(const char *message);
void printStats(const char *message, BasicRenderer::Color color=BasicRenderer::BR_DARK_CYAN);
void printStatsSpacing(BasicRenderer::Color color=BasicRenderer::BR_DARK_CYAN);
void statNewLine();
void ShowOSStats();
void ShowOSStats(unsigned int x, unsigned int y);