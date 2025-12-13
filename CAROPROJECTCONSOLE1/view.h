#pragma once
#include "model.h"
#include <vector>
#include <windows.h>
#include <string>
#include "GameAssets.h"
#define CONSOLE_WIDTH 165
#define CONSOLE_HEIGHT 55
#define CELL_VISUAL_WIDTH 6
#define CELL_VISUAL_HEIGHT 3
#define BOARD_VISUAL_WIDTH (BOARD_SIZE * CELL_VISUAL_WIDTH + 1)
#define BOARD_VISUAL_HEIGHT (BOARD_SIZE * CELL_VISUAL_HEIGHT + 1)
#define LEFT ((CONSOLE_WIDTH - BOARD_VISUAL_WIDTH) / 2)
#define TOP  ((CONSOLE_HEIGHT - BOARD_VISUAL_HEIGHT) / 2)
extern const char* MENU_ITEMS[];
extern const int TOTAL_ITEMS;
extern const int START_Y;
extern const char* PAUSE_ITEMS[];
extern const int TOTAL_PAUSE_ITEMS;
extern const int PAUSE_START_Y;
extern const char* NEW_GAME_OPTIONS[];
extern const int TOTAL_NEW_GAME_OPTIONS;
struct DrawInstructionTrueColor {
    int y, x, r, g, b, l;
};
bool SetConsoleFont(LPCWSTR fontName, SHORT sizeX, SHORT sizeY); 
void ResizeConsoleWindow(int width, int height);
void FixConsoleWindow(); 
void CenterConsole(); 
int CenterX(const std::string& text); 
int CenterXW(std::wstring s); 
void GotoXY(int x, int y);
void ClearScreenWithColor(int r, int g, int b); 
void DrawImageHalfBlock(int startX, int startY, const std::vector<DrawInstructionTrueColor>& data, float brightness = 1.0f);
void ShowGroupIntro(); 
void DrawLoadingScreen();
void DrawPressAnyKeyPrompt(bool showText);
void DrawAboutScreen();
void DrawWinEffectFrame(int winner, int colorIdx);
void DrawFullMenu(int selected_index); 
void DrawMenuItem(int index, bool is_selected);
void DrawFullNewGameMenu(int selected_index); 
void DrawNewGameMenuItem(int index, bool is_selected); 
void ShowGroupIntro();
void DrawFull1PlayerNameScreen();
void DrawFull2PlayerNameScreen(); 
void Update1PlayerNameScreen(int activeControl, const char* p1_buffer, const char* bot_buffer);
void Update2PlayerNameScreen(int activeControl, const char* p1_buffer, const char* p2_buffer); 
void DrawGuideScreen();
void DrawFrame(int x, int y, int width, int height); 
void DrawFrame1(int x, int y, int width, int height);
void DrawBoard(int pSize);
void RedrawBoardState();
void GotoBoard(int pX, int pY); \
void DrawGameUI_1P();
void DrawGameUI_2P();
void DrawStatic1P_UI(); 
void DrawStatic2P_UI(); 
void UpdateDynamic1P_UI();
void UpdateDynamic2P_UI(); 
void DrawBigArt_Color(int x, int y, const std::vector<std::string>& art, int r, int g, int b);

void DrawFullPauseMenu(int selected_index); 
void DrawPauseMenuItem(int index, bool is_selected);
void DrawGameOverScreen(int selectedOption, bool drawAll); 
void PrintCenteredText(std::string text, int boxWidth);
void DrawTimerOnly();
