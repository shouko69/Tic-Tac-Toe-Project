#pragma once
#include <string>
void ShowSaveGameScreen();
bool ShowLoadGameScreen();
void DrawCenteredCell(const char* text, int cellX, int cellY, int cellWidth, int fg_r, int fg_g, int fg_b, int bg_r, int bg_g, int bg_b);
void DrawTableFrame(int startX, int startY, int totalDataRows, int nameWidth, int dateWidth, int typeWidth);
