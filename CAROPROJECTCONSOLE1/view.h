#pragma once
#include "model.h"
#include <vector>
#include <windows.h>
#include <string>
//*****BIENTOANCUC*****
#define CONSOLE_WIDTH 165
#define CONSOLE_HEIGHT 55
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
// --- Man hinh chon Mode ---
extern const char* NEW_GAME_OPTIONS[];
extern const int TOTAL_NEW_GAME_OPTIONS;

// 1. Struct chứa dữ liệu màu (Copy vào đây)
struct DrawInstructionTrueColor {
    int y, x, r, g, b, l;
};

// 2. Khai báo hàm vẽ chung (Engine)
void DrawImageHalfBlock(int startX, int startY, const std::vector<DrawInstructionTrueColor>& data);

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

/*
 * Chức năng: Vẽ giao diện cho màn hình chọn chế độ chơi mới.
 */
void DrawFullNewGameMenu(int selected_index);
void DrawNewGameMenuItem(int index, bool is_selected);

/*
 * Chức năng: Vẽ toàn bộ màn hình nhập tên người chơi.
 * Tham số:
 *  - internalState: Trạng thái hiện tại của màn hình (ai đang nhập, ai đang xác nhận).
 *  - selectedButton: Nút nào đang được chọn (0=OK, 1=BACK).
 *  - p1_buffer: Nội dung người chơi 1 đang gõ.
 *  - p2_buffer: Nội dung người chơi 2 đang gõ.
 */
void DrawFullPlayerNameScreen();
void UpdatePlayerNameScreen(int activeControl, const char* p1_buffer, const char* p2_buffer);

// ve thong tin choi game cho 2 players
void Draw2PlayerUI();
// hien thi result
void DrawGameOverScreen(int selectedOption);

void DrawStatic2P_UI();
void UpdateDynamic2P_UI();

