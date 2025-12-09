#pragma once
#include "GameState.h"
#include "Model.h"
#include "View.h"
#include "truecolor_utils.h"
#include "control.h"
#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <conio.h>
#include <windows.h>
#include <algorithm>
#include <ctime>
#include <iomanip>
#include <cstring>
#include <sys/stat.h>
void ShowSaveGameScreen();
bool ShowLoadGameScreen();
void DrawCenteredCell(const char* text, int cellX, int cellY, int cellWidth, int fg_r, int fg_g, int fg_b, int bg_r, int bg_g, int bg_b);
void DrawTableFrame(int startX, int startY, int totalDataRows, int nameWidth, int dateWidth, int typeWidth);
bool FileExists(const std::string& filename);
void ApplyLoadedData(const GameStateData& data);
void ShowSaveGameScreen();
bool ShowLoadGameScreen();
void ShowOverwriteScreen();
void ShowNewSaveScreen();

void DrawCenteredCell(const char* text, int cellX, int cellY, int cellWidth, int fg_r, int fg_g, int fg_b, int bg_r, int bg_g, int bg_b);
void DrawTableFrame(int startX, int startY, int totalContentRows, int nameWidth, int dateWidth, int typeWidth);
void PerformSave(const std::string& filename);
GameStateData GetCurrentGameStateData();

void DrawSaveMenuItem(int index, bool is_selected);
void ShowSaveGameScreen();
