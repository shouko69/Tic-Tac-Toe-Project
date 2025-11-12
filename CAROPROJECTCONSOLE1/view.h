#pragma once
#include "model.h"
#include <windows.h>
#include <string>
//*****BIENTOANCUC*****
#define CONSOLE_WIDTH 190
#define CONSOLE_HEIGHT 45
// --- BƯỚC 2: KÍCH THƯỚC VẼ CỦA 1 Ô ---
#define CELL_VISUAL_WIDTH 4
#define CELL_VISUAL_HEIGHT 2

// --- BƯỚC 3: TỰ ĐỘNG TÍNH TOÁN KÍCH THƯỚC THỰC TẾ (VISUAL) ---
#define BOARD_VISUAL_WIDTH (BOARD_SIZE * CELL_VISUAL_WIDTH + 1)
#define BOARD_VISUAL_HEIGHT (BOARD_SIZE * CELL_VISUAL_HEIGHT + 1)

// --- BƯỚC 4: CÔNG THỨC CĂN GIỮA CHÍNH XÁC ---
#define LEFT ((CONSOLE_WIDTH - BOARD_VISUAL_WIDTH) / 2)
#define TOP  ((CONSOLE_HEIGHT - BOARD_VISUAL_HEIGHT) / 2)

// --- CÁC HẰNG SỐ CỦA MENU ---
extern const char* MENU_ITEMS[];
extern const int TOTAL_ITEMS;
extern const int START_Y;
extern const int TOTAL_PAUSE_ITEMS;
extern const char* PAUSE_ITEMS[];
extern const int PAUSE_START_Y;
extern const char* LOGO_LINE; 


// (Ví dụ: Tím than R:80, G:60, B:120)
// (Ví dụ: Hồng Neon R:255, G:100, B:180)
// (Ví dụ: Trắng R:255, G:255, B:255)

// --- Các hàm thiết lập console ---
bool SetConsoleFont(LPCWSTR fontName, SHORT sizeX, SHORT sizeY);
void ResizeConsoleWindow(int width, int height);
void FixConsoleWindow(); 
void CenterConsole();
// *****HAMKHAIBAO*****
extern int CenterX(const std::string& text);
extern void GotoXY(int x, int y);

void DrawBoard(int pSize);
void RedrawBoardState();
int ProcessFinish(int pWhoWin);
int AskContinue();
void DrawMenuItem(int index, bool is_selected);
void DrawFullMenu(int selected_index);
void DrawFullPauseMenu(int selected);
void DrawPauseMenuItem(int index, bool is_selected);
void StartAbout();
// ****CÁC HÀM VẼ UI MỚI****
void DrawTableCellRGB(const std::string& text, int x, int y, int width, int text_r, int text_g, int text_b, int bg_r, int bg_g, int bg_b);
void ClearScreenWithColor(int r, int g, int b);

