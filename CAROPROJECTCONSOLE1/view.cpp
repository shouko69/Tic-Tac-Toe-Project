#include "View.h"
#include "Windows.h"
#include <conio.h>
#include "Model.h"
#include "Control.h"
#include <iostream>
#include <string>

// --- CÁC HẰNG SỐ CỦA MENU ---
const char* MENU_ITEMS[] = { "NEW GAME", "LOAD GAME", "SETTING", "GUIDE", "ABOUT", "EXIT" };
const char* PAUSE_ITEMS[] = { "RESUME", "RESTART", "SAVE", "SETTINGS", "QUIT" };
const int TOTAL_ITEMS = sizeof(MENU_ITEMS) / sizeof(MENU_ITEMS[0]);
const int START_Y = 10;
const int TOTAL_PAUSE_ITEMS = sizeof(PAUSE_ITEMS) / sizeof(PAUSE_ITEMS[0]);
const int PAUSE_START_Y = 9;
const char* LOGO_LINE = "CARO LEGENDS"; // Logo đơn giản

// Ký tự vẽ khung (ĐÃ XÓA - SẼ SỬ DỤNG TRỰC TIẾP)

_POINT _A[BOARD_SIZE][BOARD_SIZE];
bool _TURN = true;

// --- HÀM HỖ TRỢ --- //
void FixConsoleWindow() {
    HWND consoleWindow = GetConsoleWindow();
    LONG style = GetWindowLong(consoleWindow, GWL_STYLE);
    style &= ~(WS_MAXIMIZEBOX) & ~(WS_THICKFRAME);
    SetWindowLong(consoleWindow, GWL_STYLE, style);
}

void GotoXY(int x, int y) {
    COORD coord = { (SHORT)x, (SHORT)y };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void SetColor(int color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

int GetConsoleWidth() {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    int columns = 80;
    if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) {
        columns = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    }
    return columns;
}

int CenterX(const std::string& text) {
    int consoleWidth = GetConsoleWidth();
    int x = (consoleWidth - (int)text.length()) / 2;
    return x > 0 ? x : 0;
}

// --- VẼ BÀN CỜ ---
void DrawBoard(int pSize) {
    // Vòng lặp này sẽ đi qua từng "giao điểm" trên lưới caro
    for (int j = 0; j <= pSize; j++) {
        for (int i = 0; i <= pSize; i++) {

            // Tính toán tọa độ chính xác của từng giao điểm trên màn hình
            int x = LEFT + 4 * i;
            int y = TOP + 2 * j;
            GotoXY(x, y);

            // --- Xác định vị trí để vẽ ký tự phù hợp (ĐÃ THAY THẾ) ---
            if (i == 0 && j == 0) std::cout << "╔";
            else if (i == pSize && j == 0) std::cout << "╗";
            else if (i == 0 && j == pSize) std::cout << "╚";
            else if (i == pSize && j == pSize) std::cout << "╝";
            else if (j == 0) std::cout << "╦";
            else if (j == pSize) std::cout << "╩";
            else if (i == 0) std::cout << "╠";
            else if (i == pSize) std::cout << "╣";
            else std::cout << "╬";

            // 2. Nếu đây không phải cột cuối cùng, vẽ 3 đường gạch ngang "---"
            if (i < pSize) {
                std::cout << "═══"; // Thay vì 3 lần FRAME_HORIZONTAL
            }

            // 3. Nếu đây không phải hàng cuối cùng, vẽ 1 đường dọc "|" ở dưới
            if (j < pSize) {
                GotoXY(x, y + 1);
                std::cout << "║";
            }
        }
    }
}
// Nó đọc Model (_Board[][]) và in ra tất cả X, O
void RedrawBoardState() {

    // 1. Lặp qua toàn bộ Model (mảng dữ liệu _A)
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {

            // 2. Lấy dữ liệu của từng ô
            int c = _A[i][j].c;      // Lấy trạng thái ô (-1 là X, 1 là O, 0 là trống)
            int x_pos = _A[i][j].x;  // Lấy tọa độ X màn hình
            int y_pos = _A[i][j].y;  // Lấy tọa độ Y màn hình

            // 3. Thực hiện vẽ (View)
            if (c == -1) { // Nếu ô này là X
                GotoXY(x_pos, y_pos);
                std::cout << "X"; // (Hoặc ký tự người chơi 1)
            }
            else if (c == 1) { // Nếu ô này là O
                GotoXY(x_pos, y_pos);
                std::cout << "O"; // (Hoặc ký tự người chơi 2)
            }
            // Nếu c == 0 (ô trống) thì không làm gì cả
        }
    }

    // 4. Rất quan trọng: Đặt con trỏ về vị trí người dùng đang đứng
    //    Vì vòng lặp ở trên đã di chuyển con trỏ đi lung tung
    GotoXY(_X, _Y);
}
// --- XỬ LÝ KẾT THÚC TRÒ CHƠI ---
int ProcessFinish(int pWhoWin) {
    // Tọa độ X bên phải bàn cờ (giữ nguyên)
    const int MESSAGE_X = LEFT + 4 * BOARD_SIZE + 5;

    // Tọa độ Y MỚI để căn giữa theo chiều dọc
    // TOP là điểm bắt đầu, BOARD_SIZE là nửa chiều cao của bàn cờ (vì mỗi ô cao 2 dòng)
    const int MIDDLE_Y = TOP + BOARD_SIZE;

    // Đặt con trỏ vào vị trí mới
    GotoXY(MESSAGE_X, MIDDLE_Y);

    switch (pWhoWin) {
    case -1:
        std::cout << "                                                  "; // Xóa dòng cũ
        GotoXY(MESSAGE_X, MIDDLE_Y);
        std::cout << "Nguoi choi X da thang, nguoi choi O da thua";
        break;
    case 1:
        std::cout << "                                                  "; // Xóa dòng cũ
        GotoXY(MESSAGE_X, MIDDLE_Y);
        std::cout << "Nguoi choi O da thang, nguoi choi X da thua";
        break;
    case 0:
        std::cout << "                                                  "; // Xóa dòng cũ
        GotoXY(MESSAGE_X, MIDDLE_Y);
        std::cout << "Hoa!";
        break;
    case 2:
        _TURN = !_TURN;
        // Xóa thông báo cũ khi ván đấu tiếp tục
        GotoXY(MESSAGE_X, MIDDLE_Y);
        std::cout << "                                                  ";
        GotoXY(MESSAGE_X, MIDDLE_Y + 2); // Cũng xóa dòng hỏi Y/N
        std::cout << "                                                  ";
        break;
    }

    // Đưa con trỏ về lại vị trí hiện tại trên bàn cờ
    GotoXY(_X, _Y);
    return pWhoWin;
}

// --- HỎI TIẾP TỤC ---
int AskContinue() {
    // Tọa độ X bên phải (giữ nguyên)
    const int MESSAGE_X = LEFT + 4 * BOARD_SIZE + 5;

    // Tọa độ Y MỚI, nằm ngay dưới dòng thông báo ở giữa
    const int MIDDLE_Y = TOP + BOARD_SIZE;

    // Đặt vị trí con trỏ ở dòng dưới (MIDDLE_Y + 2)
    GotoXY(MESSAGE_X, MIDDLE_Y + 2);

    std::cout << "Nhan 'Y/N' de tiep tuc/dung: ";
    return toupper(_getch());
}

// --- VẼ MỘT MỤC MENU ---
// VẼ MỘT MỤC MENU VỚI HIỆU ỨNG “PHỒNG”
void DrawMenuItem(int index, bool is_selected) {
    if (index < 0 || index >= TOTAL_ITEMS) return;

    const int PADDING = 4;
    std::string leftPad(PADDING, ' ');
    std::string rightPad(PADDING, ' ');

    std::string menuText;
    if (is_selected) {
        menuText = ">> " + std::string(MENU_ITEMS[index]) + " <<";
    }
    else {
        menuText = "  " + std::string(MENU_ITEMS[index]) + "  ";
    }

    int x = CenterX(leftPad + menuText + rightPad);
    int y = START_Y + index * 2;

    // Vẽ padding trái bình thường
    SetColor(COLOR_NORMAL_TEXT);
    GotoXY(x, y);
    std::cout << leftPad;

    // Vẽ menuText có màu
    if (is_selected) SetColor(COLOR_SELECTED_BG);
    else SetColor(COLOR_UNSELECTED_BG);

    std::cout << menuText;

    // Vẽ padding phải bình thường
    SetColor(COLOR_NORMAL_TEXT);
    std::cout << rightPad;
}

// --- VẼ TOÀN BỘ MENU ---
// VẼ TOÀN BỘ MENU
void DrawFullMenu(int selected_index) {
    system("cls");
    SetColor(COLOR_DEFAULT); // Nền trắng, chữ đen

    // --- KHUNG LOGO ---
    const int LOGO_PADDING = 4;
    std::string logoStr(LOGO_LINE);
    int frameWidth = logoStr.length() + LOGO_PADDING * 2;
    int logoX = CenterX(logoStr);
    int frameX = logoX - LOGO_PADDING - 1;

    SetColor(COLOR_LOGO_FRAME);

    // Dòng trên (ĐÃ THAY THẾ)
    GotoXY(frameX, 2);
    std::cout << "╔";
    for (int i = 0; i < frameWidth; i++) std::cout << "═";
    std::cout << "╗";

    // Dòng logo (ĐÃ THAY THẾ)
    GotoXY(frameX, 3);
    std::cout << "║";
    GotoXY(logoX, 3);
    std::cout << logoStr;
    GotoXY(frameX + frameWidth + 1, 3);
    std::cout << "║";

    // Dòng dưới (ĐÃ THAY THẾ)
    GotoXY(frameX, 4);
    std::cout << "╚";
    for (int i = 0; i < frameWidth; i++) std::cout << "═";
    std::cout << "╝";

    // --- VẼ MENU ---
    for (int i = 0; i < TOTAL_ITEMS; i++) {
        DrawMenuItem(i, i == selected_index);
    }

    // --- HƯỚNG DẪN ---
    SetColor(COLOR_GUIDE);
    const char* guide = "'W': Move up, 'S': Move down, 'Enter': Select, 'ESC': Exit";
    GotoXY(CenterX(guide), START_Y + TOTAL_ITEMS * 2 + 5);
    std::cout << guide;

    SetColor(COLOR_NORMAL_TEXT);
}
// --- VẼ TỪNG MỤC PAUSE MENU ---
void DrawPauseMenuItem(int index, bool is_selected) {
    if (index < 0 || index >= TOTAL_PAUSE_ITEMS) return;

    const int PADDING = 6;
    std::string leftPad(PADDING, ' ');
    std::string rightPad(PADDING, ' ');

    std::string text;
    if (is_selected)
        text = ">> " + std::string(PAUSE_ITEMS[index]) + " <<";
    else
        text = "  " + std::string(PAUSE_ITEMS[index]) + "  ";

    int x = CenterX(leftPad + text + rightPad);
    int y = PAUSE_START_Y + index * 2;

    // PADDING TRÁI
    SetColor(COLOR_NORMAL_TEXT);
    GotoXY(x, y);
    std::cout << leftPad;

    // TEXT CHÍNH
    if (is_selected) SetColor(COLOR_SELECTED_BG);
    else SetColor(COLOR_UNSELECTED_BG);

    std::cout << text;

    // PADDING PHẢI
    SetColor(COLOR_NORMAL_TEXT);
    std::cout << rightPad;
}

// --- VẼ TOÀN BỘ PAUSE MENU ---
void DrawFullPauseMenu(int selected_index) {
    system("cls");

    // Màu nền trắng, chữ đen
    SetColor(COLOR_DEFAULT);

    // --- VẼ KHUNG TIÊU ĐỀ ---
    const std::string title = "GAME PAUSED";
    int titleX = CenterX(title);
    int frameWidth = title.length() + 8;
    int frameX = titleX - 4;

    SetColor(COLOR_LOGO_FRAME);

    // Dòng trên (ĐÃ THAY THẾ + SỬA LỖI)
    GotoXY(frameX, 3);
    std::cout << "╔";
    for (int i = 0; i < frameWidth; i++) std::cout << "═"; // Sửa từ ║ thành ═
    std::cout << "╗";

    // Dòng giữa (ĐÃ THAY THẾ)
    GotoXY(frameX, 4);
    std::cout << "║";
    GotoXY(CenterX(title), 4);
    SetColor(COLOR_TITLE_TEXT);
    std::cout << title;
    SetColor(COLOR_LOGO_FRAME);
    GotoXY(frameX + frameWidth + 1, 4);
    std::cout << "║";

    // Dòng dưới (ĐÃ THAY THẾ)
    GotoXY(frameX, 5);
    std::cout << "╚";
    for (int i = 0; i < frameWidth; i++) std::cout << "═";
    std::cout << "╝";

    // --- VẼ DANH SÁCH MENU ---
    for (int i = 0; i < TOTAL_PAUSE_ITEMS; i++) {
        DrawPauseMenuItem(i, i == selected_index);
    }

    // --- HƯỚNG DẪN ---
    SetColor(COLOR_GUIDE);
    const char* guide = "'W': Move up, 'S': Move down,  'Enter': Select,  'ESC': Resume";
    GotoXY(CenterX(guide), PAUSE_START_Y + TOTAL_PAUSE_ITEMS * 2 + 3);
    std::cout << guide;

    SetColor(COLOR_NORMAL_TEXT);
}