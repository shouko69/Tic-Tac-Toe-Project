#pragma once
#include "model.h"
#include <vector>
#include <windows.h>
#include <string>


// 1. CẤU HÌNH KÍCH THƯỚC (BÀN CỜ TO)

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


// 2. CONSTANTS & STRUCTS

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


// 3. KHAI BÁO HÀM (FUNCTION PROTOTYPES)

// --- Nhóm Hệ Thống (System & Utils) ---
bool SetConsoleFont(LPCWSTR fontName, SHORT sizeX, SHORT sizeY); /*fontName: Đặt tên font muốn dùng (ví dụ: L"Consolas", L"Terminal"...), sizeX, sizeY: 
Đặt chiều rộng và chiều cao(pixel) cho một ký tự.*/


void ResizeConsoleWindow(int width, int height);
void FixConsoleWindow(); 
void CenterConsole(); // canh giữa console
int CenterX(const std::string& text); // canh giữa chữ
int CenterXW(std::wstring s); // canh giữa chữ phiên bản khi kích hoạt chế độ unicode (k dùng cout !!!!), khi vô chế độ này thì phải xài các hàm sử dụng wcout hết !!!!!!!
void GotoXY(int x, int y);
void ClearScreenWithColor(int r, int g, int b); // Hàm này bây giờ thay thế System('cls') nó sẽ dọn dẹp màn hình nhưng sẽ chuyển màu nền về màu theo tham số đã truyền !!!!
void DrawImageHalfBlock(int startX, int startY, const std::vector<DrawInstructionTrueColor>& data); // kỹ thuật sử dụng để đưa ảnh pixel vô console 

// --- Nhóm Intro & Menu ---
void ShowGroupIntro(); // Intro Group6
void StartIntro(); // Intro có Loading bar

void DrawFullMenu(int selected_index); // vẽ tất cả những j có ở menu chính
void DrawMenuItem(int index, bool is_selected);
void DrawFullNewGameMenu(int selected_index); // vẽ tất cả những j có ở menu mode khi bấm play
void DrawNewGameMenuItem(int index, bool is_selected); // vẽ các nút chọn chức năng của mode 
void StartAbout();
void StartGuide();
void ShowGroupIntro();

// --- Nhóm Nhập Tên (Player Name) ---
void DrawFull2PlayerNameScreen(); // vẽ tiêu đề 2 PLAYERS, khung chọn tên
void Update2PlayerNameScreen(int activeControl, const char* p1_buffer, const char* p2_buffer); // hàm hiển thị khi nhập tên 

// --- Nhóm In-Game (Bàn cờ & UI chơi) ---
void DrawFrame(int x, int y, int width, int height); // Vẽ Khung tùy ý
void DrawFrame1(int x, int y, int width, int height);
void DrawBoard(int pSize);
void RedrawBoardState(); // Vẽ lại X/O trên bàn cờ
void GotoBoard(int pX, int pY); // Nhảy đến tâm ô cờ để đặt con trỏ
void DrawGameUI_2P(); // Vẽ toàn bộ giao diện trong game (Nền, Bàn cờ, UI tĩnh, UI động)
void DrawStatic2P_UI(); // Vẽ khung tên, điểm (Chạy 1 lần)
void UpdateDynamic2P_UI(); // Cập nhật điểm số, lượt đi (Chạy liên tục)
void DrawBigArt_Color(int x, int y, const std::vector<std::string>& art, int r, int g, int b);
// --- Nhóm Pause & Kết thúc ---
void DrawFullPauseMenu(int selected_index); // vẽ tất cả những j có trên màn hình pause
void DrawPauseMenuItem(int index, bool is_selected); // các lựa chọn của menu pause
void DrawGameOverScreen(int selectedOption, bool drawAll); // Màn hình hiển thị kết quả
// --- WIn effect ---
void ShowChristmasResult_ASCII(int winner);
//
void PrintCenteredText(std::string text, int boxWidth);
void DrawTimerOnly();