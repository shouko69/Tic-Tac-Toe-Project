#pragma once
#include "model.h"
#include <string>
//*****BIENTOANCUC*****
#define CONSOLE_WIDTH 150  
#define CONSOLE_HEIGHT 39 
#define LEFT ((CONSOLE_WIDTH - BOARD_SIZE) / 3) // Toa do trai man hinh ban co
#define TOP (((CONSOLE_HEIGHT - BOARD_SIZE) / 3) - 2) // Toa do tren man hinh ban co

// --- CÁC HẰNG SỐ CỦA MENU ---
extern const char* MENU_ITEMS[];
extern const int TOTAL_ITEMS;
extern const int START_Y;
extern const int TOTAL_PAUSE_ITEMS;
extern const char* PAUSE_ITEMS[];
extern const int PAUSE_START_Y;
extern const char* LOGO_LINE; 

// --- CÁC MÃ MÀU (Nền Trắng = 15) ---
#define COLOR_TITLE_TEXT (15 * 16 + 9) //Xanh Sáng
#define COLOR_NORMAL_TEXT   (15 * 16 + 0)  // Đen trên Nền Trắng (240)
#define COLOR_LOGO_FRAME    (15 * 16 + 12) // Đỏ trên Nền Trắng (252)
#define COLOR_GUIDE         (15 * 16 + 8)  // Xám trên Nền Trắng (248)
#define COLOR_SELECTED_BG   (14 * 16 + 0)  // Nền VÀNG (14), Chữ ĐEN (0)
#define COLOR_UNSELECTED_BG (15 * 16 + 0)  // Nền TRẮNG (15), Chữ ĐEN (0)
#define COLOR_DEFAULT 0xF0 


// *****HAMKHAIBAO*****
extern int CenterX(const std::string& text);
extern void GotoXY(int x, int y);
extern void SetColor(int color);
void FixConsoleWindow();
void DrawBoard(int pSize);
void RedrawBoardState();
int ProcessFinish(int pWhoWin);
int AskContinue();
void DrawMenuItem(int index, bool is_selected);
void DrawFullMenu(int selected_index);
void DrawFullPauseMenu(int selected);
void DrawPauseMenuItem(int index, bool is_selected);
void StartAbout();
void StartGuide();
