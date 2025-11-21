#pragma once
#include "model.h"
#include <vector>
#include <windows.h>
#include <string>

// ==========================================================================
// 1. CẤU HÌNH KÍCH THƯỚC (BÀN CỜ TO)
// ==========================================================================
#define CONSOLE_WIDTH 165
#define CONSOLE_HEIGHT 55

// Kích thước vẽ của 1 ô (Bản to)
#define CELL_VISUAL_WIDTH 6
#define CELL_VISUAL_HEIGHT 3

// Tự động tính toán kích thước bàn cờ
#define BOARD_VISUAL_WIDTH (BOARD_SIZE * CELL_VISUAL_WIDTH + 1)
#define BOARD_VISUAL_HEIGHT (BOARD_SIZE * CELL_VISUAL_HEIGHT + 1)

// Căn giữa bàn cờ
#define LEFT ((CONSOLE_WIDTH - BOARD_VISUAL_WIDTH) / 2)
#define TOP  ((CONSOLE_HEIGHT - BOARD_VISUAL_HEIGHT) / 2)

// ==========================================================================
// 2. CONSTANTS & STRUCTS
// ==========================================================================
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

// ==========================================================================
// 3. KHAI BÁO HÀM (FUNCTION PROTOTYPES)
// ==========================================================================

// --- Nhóm Hệ Thống (System & Utils) ---
bool SetConsoleFont(LPCWSTR fontName, SHORT sizeX, SHORT sizeY);
void ResizeConsoleWindow(int width, int height);
void FixConsoleWindow();
void CenterConsole();
int CenterX(const std::string& text);
void GotoXY(int x, int y);
void ClearScreenWithColor(int r, int g, int b);
void DrawTableCellRGB(const std::string& text, int x, int y, int width, int text_r, int text_g, int text_b, int bg_r, int bg_g, int bg_b);
void DrawImageHalfBlock(int startX, int startY, const std::vector<DrawInstructionTrueColor>& data);

// --- Nhóm Intro & Menu ---
void StartIntro(); // Intro có Loading bar
void DrawFancyBox(int x, int y, int width, std::string text, bool isSelected);
void DrawFullMenu(int selected_index);
void DrawMenuItem(int index, bool is_selected);
void DrawFullNewGameMenu(int selected_index);
void DrawNewGameMenuItem(int index, bool is_selected);
void StartAbout();

// --- Nhóm Nhập Tên (Player Name) ---
void DrawFullPlayerNameScreen();
void UpdatePlayerNameScreen(int activeControl, const char* p1_buffer, const char* p2_buffer);

// --- Nhóm In-Game (Bàn cờ & UI chơi) ---
void DrawBoard(int pSize);
void RedrawBoardState(); // Vẽ lại X/O trên bàn cờ
void GotoBoard(int pX, int pY); // Nhảy đến tâm ô cờ
void DrawGameUI(); // Vẽ toàn bộ giao diện game (Nền, Bàn cờ, UI tĩnh, UI động)
void DrawStatic2P_UI(); // Vẽ khung tên, điểm (Chạy 1 lần)
void UpdateDynamic2P_UI(); // Cập nhật điểm số, lượt đi (Chạy liên tục)

// --- Nhóm Pause & Kết thúc ---
void DrawFullPauseMenu(int selected_index);
void DrawPauseMenuItem(int index, bool is_selected);
void DrawGameOverScreen(int selectedOption, bool drawAll); // Màn hình kết quả xịn
