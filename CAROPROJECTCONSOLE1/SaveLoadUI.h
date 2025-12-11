#pragma once
#include <string>
#include <vector>
#include "SaveLoad.h" 
#include "view.h"
void DrawSaveMenu(int selectedIndex, bool init);
void DrawNewSaveInputScreen(const std::string& currentName, const std::string& message, bool isSuccess, bool init);
void DrawFileSelectionScreen(const std::vector<GameStateData>& saves, int viewOffset, int selectedIndex, bool isLoadMode, bool init);
void DrawCenteredCell(const char* text, int cellX, int cellY, int cellWidth, int fg_r, int fg_g, int fg_b, int bg_r, int bg_g, int bg_b);