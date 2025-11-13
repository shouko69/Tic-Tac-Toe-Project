#include "View.h"
#include "Windows.h"
#include <conio.h>
#include "Model.h"
#include "Control.h"
#include <iostream>
#include <iomanip>    
#include <string>
#include "truecolor_utils.h"
// #include <fcntl.h> // Không cần nữa
// #include <io.h> // Không cần nữa
#include "GameState.h"
#include <cwchar> // Cần cho SetConsoleFont
#include <thread> // Cần cho hiệu ứng (nếu có)
#include <chrono> // Cần cho hiệu ứng (nếu có)


using namespace std;
const char* MENU_ITEMS[] = { "NEW GAME", "LOAD GAME", "SETTING", "GUIDE", "ABOUT", "EXIT" };
const char* PAUSE_ITEMS[] = { "RESUME", "RESTART", "SAVE", "SETTINGS", "QUIT" };
const int TOTAL_ITEMS = sizeof(MENU_ITEMS) / sizeof(MENU_ITEMS[0]);
const int START_Y = 15;
const int TOTAL_PAUSE_ITEMS = sizeof(PAUSE_ITEMS) / sizeof(PAUSE_ITEMS[0]);
const int PAUSE_START_Y = 9;
const int FRAME_START_X = 99;
const int FRAME_WIDTH = 28;
const char* LOGO_LINE = "CARO LEGENDS";

_POINT _A[BOARD_SIZE][BOARD_SIZE];
bool _TURN = true;

// --- HÀM HỖ TRỢ (Giữ nguyên) ---

bool SetConsoleFont(LPCWSTR fontName, SHORT sizeX, SHORT sizeY) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hConsole == INVALID_HANDLE_VALUE) return false;
    CONSOLE_FONT_INFOEX cfi;
    cfi.cbSize = sizeof(CONSOLE_FONT_INFOEX);
    if (!GetCurrentConsoleFontEx(hConsole, FALSE, &cfi)) return false;
    cfi.dwFontSize.X = sizeX;
    cfi.dwFontSize.Y = sizeY;
    wcscpy_s(cfi.FaceName, fontName);
    if (!SetCurrentConsoleFontEx(hConsole, FALSE, &cfi)) return false;
    return true;
}

void ResizeConsoleWindow(int width, int height) {
    HWND consoleWindow = GetConsoleWindow();
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD bufferSize = { (SHORT)width, (SHORT)height };
    SetConsoleScreenBufferSize(hConsole, bufferSize);
    SMALL_RECT windowSize = { 0, 0, (SHORT)(width - 1), (SHORT)(height - 1) };
    SetConsoleWindowInfo(hConsole, TRUE, &windowSize);
}

void CenterConsole() {
    HWND consoleWindow = GetConsoleWindow();
    if (consoleWindow == NULL) return;
    RECT rectClient;
    GetWindowRect(consoleWindow, &rectClient);
    int windowWidth = rectClient.right - rectClient.left;
    int windowHeight = rectClient.bottom - rectClient.top;
    RECT rectScreen;
    SystemParametersInfo(SPI_GETWORKAREA, 0, &rectScreen, 0);
    int screenWidth = rectScreen.right - rectScreen.left;
    int screenHeight = rectScreen.bottom - rectScreen.top;
    int newX = rectScreen.left + (screenWidth - windowWidth) / 2;
    int newY = rectScreen.top + (screenHeight - windowHeight) / 2;
    MoveWindow(consoleWindow, newX, newY, windowWidth, windowHeight, TRUE);
}

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

// --- HÀM "SƠN" NỀN (Phiên bản cuối cùng, flicker-free) ---
// (ĐÃ XÓA HÀM ClearScreen() GÂY CRASH Ở TRÊN)
void ClearScreenWithColor(int r, int g, int b) {
    SetBgRGB(r, g, b); // Đặt màu nền mong muốn

    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD coordScreen = { 0, 0 }; // Góc trên bên trái
    DWORD cCharsWritten;
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    DWORD dwConSize;

    if (!GetConsoleScreenBufferInfo(hConsole, &csbi)) return;
    dwConSize = csbi.dwSize.X * csbi.dwSize.Y;

    if (!FillConsoleOutputCharacter(hConsole, (TCHAR)' ', dwConSize, coordScreen, &cCharsWritten)) return;
    if (!GetConsoleScreenBufferInfo(hConsole, &csbi)) return;
    if (!FillConsoleOutputAttribute(hConsole, csbi.wAttributes, dwConSize, coordScreen, &cCharsWritten)) return;

    SetConsoleCursorPosition(hConsole, coordScreen);
}

// --- HÀM VẼ (ĐÃ SỬA LỖI) ---

// --- VẼ MÀN HÌNH BẮT ĐẦU (VỚI LOADING BAR) ---
void StartIntro() {
    // 1. "Sơn" toàn bộ màn hình bằng màu Lavender
    ClearScreenWithColor(202, 196, 248);
    SetBgRGB(202, 196, 248); // Nền Lavender
    SetColorRGB(0, 0, 0);   // Chữ Đen

    // 2. Vẽ Logo (Code vẽ logo của bạn)
    const int LOGO_PADDING = 4;
    std::string logoStr(LOGO_LINE);
    int frameWidth = logoStr.length() + LOGO_PADDING * 2;
    int logoX = CenterX(logoStr);
    int frameX = logoX - LOGO_PADDING - 1;

    SetColorRGB(100, 100, 255); // Khung Tím/Xanh
    GotoXY(frameX, 2);
    std::cout << "╔";
    for (int i = 0; i < frameWidth; i++) std::cout << "═";
    std::cout << "╗";
    GotoXY(frameX, 3);
    std::cout << "║";
    GotoXY(logoX, 3);
    SetColorRGB(0, 0, 0); // Chữ logo đen
    std::cout << logoStr;
    SetColorRGB(100, 100, 255); // Khung tím
    GotoXY(frameX + frameWidth + 1, 3);
    std::cout << "║";
    GotoXY(frameX, 4);
    std::cout << "╚";
    for (int i = 0; i < frameWidth; i++) std::cout << "═";
    std::cout << "╝";

    // (Nếu bạn vẫn dùng hàm vẽ con mèo, hãy gọi nó ở đây)
    // thecarogame(10, 10); 

    // 3. --- GIAI ĐOẠN LOADING BAR ---
    int barWidth = 50; // Chiều rộng của thanh loading (50 ký tự)
    int barY = 20;     // Vị trí Y của thanh loading
    int barX = CenterX(std::string(barWidth + 2, ' ')); // Căn giữa thanh [....]

    // 3a. Vẽ tiêu đề "LOADING..."
    SetColorRGB(80, 80, 80); // Màu xám
    GotoXY(CenterX("LOADING..."), barY - 2); // 2 dòng bên trên thanh
    std::cout << "LOADING...";

    // 3b. Vẽ khung thanh loading rỗng [....]
    GotoXY(barX, barY);
    std::cout << "[";
    std::string emptyBar(barWidth, '.'); // Dùng dấu '.' làm nền rỗng
    std::cout << emptyBar;
    std::cout << "]";

    // 3c. Vòng lặp "chạy" thanh loading
    for (int i = 0; i <= barWidth; ++i) {
        // Tính toán %
        int percent = (int)(((float)i / barWidth) * 100);

        // Vẽ khối loading (dùng nền xanh)
        SetBgRGB(100, 100, 255); // Nền xanh cho khối
        GotoXY(barX + 1 + i, barY); // Di chuyển con trỏ vào trong thanh
        std::cout << " "; // In 1 khối (khoảng trắng có nền)

        // Cập nhật số % (nền lavender, chữ xám)
        SetBgRGB(202, 196, 248);
        SetColorRGB(80, 80, 80);
        GotoXY(barX + barWidth + 3, barY); // Vị trí bên phải thanh
        std::cout << percent << "%"; // In %

        // Tạo hiệu ứng chạy từ từ
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }

    // Dừng 1 chút sau khi load xong
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    // 4. --- GIAI ĐOẠN "PRESS ANY KEY" ---

    // 4a. Xóa thanh loading và chữ "LOADING..."
    SetBgRGB(202, 196, 248); // Đặt lại nền Lavender
    std::string blank(barWidth + 10, ' ');
    GotoXY(CenterX("LOADING..."), barY - 2);
    std::cout << "          "; // Xóa chữ LOADING...
    GotoXY(barX, barY);
    std::cout << blank; // Xóa thanh loading và %

    // 4b. Vẽ dòng chữ "Press any key" nhấp nháy
    const char* prompt = "PRESS ANY KEY TO START";
    int promptX = CenterX(prompt);
    int promptY = 20; // Dùng lại vị trí cũ
    bool showText = true;

    while (true) {
        if (_kbhit()) {
            _getch();
            break;
        }
        if (showText) {
            SetColorRGB(80, 80, 80); // Màu xám
            GotoXY(promptX, promptY);
            std::cout << prompt;
        }
        else {
            std::string blankPrompt(strlen(prompt), ' ');
            SetBgRGB(202, 196, 248); // Nền Lavender
            GotoXY(promptX, promptY);
            std::cout << blankPrompt;
        }
        showText = !showText;
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    // 5. Reset màu sắc trước khi chuyển cảnh
    ResetColor();
}

void StartAbout() {
    ClearScreenWithColor(202, 196, 248); // "Sơn" nền Lavender
    SetColorRGB(0, 0, 0); // Chữ Đen

    int i = 55, offset = 1;

    // XÓA BỎ: _setmode(_fileno(stdout), _O_U16TEXT);

    // SỬA LẠI: Dùng std::cout và chuỗi hẹp "..."
    GotoXY(i, 1);
    std::cout << " █████╗ ██████╗  ██████╗ ██╗   ██╗████████╗\n"; GotoXY(i, 2);
    std::cout << "██╔══██╗██╔══██╗██╔═══██╗██║   ██║╚══██╔══╝\n"; GotoXY(i, 3);
    std::cout << "███████║██████╔╝██║   ██║██║   ██║   ██║   \n"; GotoXY(i, 4);
    std::cout << "██╔══██║██╔══██╗██║   ██║██║   ██║   ██║   \n"; GotoXY(i, 5);
    std::cout << "██║  ██║██████╔╝╚██████╔╝╚██████╔╝   ██║   \n"; GotoXY(i, 6);
    std::cout << "╚═╝  ╚═╝╚═════╝  ╚═════╝  ╚═════╝    ╚═╝   \n";

    COORD tableCoord{ 40, 10 };
    i = 0;
    GotoXY(tableCoord.X + 5, tableCoord.Y);
    std::cout << "Welcome to the Gomoku Game - made by team 6 - 25CTT3 - HCMUS\n";

    // ... (Sửa tất cả wcout thành cout) ...
    i += 2;
    GotoXY(tableCoord.X, tableCoord.Y + i);
    std::cout << "25120215 - Nguyen Minh Nhat      \t\t PvE Mode - Game Content\n";
    i += offset;
    GotoXY(tableCoord.X, tableCoord.Y + i + 2);
    std::cout << "25120215 - Nguyen Hoang Huy     \t\t Game Interface Design\n";
    i += offset;
    GotoXY(tableCoord.X, tableCoord.Y + i + 2);
    std::cout << "25120215 - Vu Thanh Phong      \t\t Game Graphic Design - Game Flow Design\n";
    i += offset;
    GotoXY(tableCoord.X, tableCoord.Y + i + 2);
    std::cout << "25120215 - Do Le Nhat Quang    \t\t Game Interface Design - Game Sound\n";
    i += offset;
    GotoXY(tableCoord.X, tableCoord.Y + i + 2);
    std::cout << "25120215 - Nguyen Phu Quang    \t\t Load & Save Game\n";
    i += offset;
    GotoXY(tableCoord.X, tableCoord.Y + i + 2);
    std::cout << "25120215 - Nguyen Vu Nhat Quang \t\t Game Graphic Design\n";

    while (true) {
        if (_kbhit()) {
            int key = _getch();
            if (key == 27 || key == 13) break; // ESC hoặc Enter
        }
        Sleep(50);
    }

    // XÓA BỎ: _setmode(_fileno(stdout), _O_TEXT);

    // Không cần system("cls") vì currentState sẽ vẽ lại
    currentState = MENU;
}

void DrawBoard(int pSize) {
    SetColorRGB(0, 0, 0); // Viền đen
    for (int j = 0; j <= pSize; j++) {
        for (int i = 0; i <= pSize; i++) {
            int x = LEFT + 4 * i;
            int y = TOP + 2 * j;
            GotoXY(x, y);

            // SỬA LẠI: Dùng std::cout
            if (i == 0 && j == 0) std::cout << "╔";
            else if (i == pSize && j == 0) std::cout << "╗";
            else if (i == 0 && j == pSize) std::cout << "╚";
            else if (i == pSize && j == pSize) std::cout << "╝";
            else if (j == 0) std::cout << "╦";
            else if (j == pSize) std::cout << "╩";
            else if (i == 0) std::cout << "╠";
            else if (i == pSize) std::cout << "╣";
            else std::cout << "╬";

            if (i < pSize) {
                std::cout << "═══";
            }
            if (j < pSize) {
                GotoXY(x, y + 1);
                std::cout << "║";
            }
        }
    }
}

void RedrawBoardState() {
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            int screenX = LEFT + j * 4 + 2;
            int screenY = TOP + i * 2 + 1;
            GotoXY(screenX, screenY);
            if (_A[i][j].c == -1) {
                SetColorRGB(255, 0, 0); // Màu đỏ
                std::cout << "X";
            }
            else if (_A[i][j].c == 1) {
                SetColorRGB(0, 0, 255); // Màu xanh
                std::cout << "O";
            }
        }
    }
    SetColorRGB(0, 0, 0); // Reset màu chữ về đen
    GotoXY(_X, _Y);
}

int ProcessFinish(int pWhoWin) {
    const int MESSAGE_X = LEFT + 4 * BOARD_SIZE + 5;
    const int MIDDLE_Y = TOP + BOARD_SIZE;

    SetColorRGB(0, 0, 0); // Chữ đen
    GotoXY(MESSAGE_X, MIDDLE_Y);

    switch (pWhoWin) {
    case -1:
        std::cout << "                                              ";
        GotoXY(MESSAGE_X, MIDDLE_Y);
        std::cout << "Nguoi choi X da thang, nguoi choi O da thua";
        break;
    case 1:
        std::cout << "                                              ";
        GotoXY(MESSAGE_X, MIDDLE_Y);
        std::cout << "Nguoi choi O da thang, nguoi choi X da thua";
        break;
    case 0:
        std::cout << "                                              ";
        GotoXY(MESSAGE_X, MIDDLE_Y);
        std::cout << "Hoa!";
        break;
    case 2:
        _TURN = !_TURN;
        GotoXY(MESSAGE_X, MIDDLE_Y);
        std::cout << "                                              ";
        GotoXY(MESSAGE_X, MIDDLE_Y + 2);
        std::cout << "                                              ";
        break;
    }
    GotoXY(_X, _Y);
    return pWhoWin;
}

int AskContinue() {
    const int MESSAGE_X = LEFT + 4 * BOARD_SIZE + 5;
    const int MIDDLE_Y = TOP + BOARD_SIZE;
    GotoXY(MESSAGE_X, MIDDLE_Y + 2);

    SetColorRGB(0, 0, 0); // Chữ đen
    std::cout << "Nhan 'Y/N' de tiep tuc/dung: ";
    return toupper(_getch());
}

void DrawMenuItem(int index, bool is_selected) {
    if (index < 0 || index >= TOTAL_ITEMS) return;

    std::string menuText = std::string(MENU_ITEMS[index]);

    if (is_selected) {
        SetColorRGB(255, 100, 180); // Chữ NỔI BẬT (Hồng)
    }
    else {
        SetColorRGB(80, 60, 120); // Chữ BÌNH THƯỜNG (Tím than)
    }

    SetBgRGB(203, 196, 248); // Nền Lavender (mã màu từ code cũ)

    int text_length = menuText.length();
    int x = FRAME_START_X + (FRAME_WIDTH - text_length) / 2;
    int y = START_Y + index * 2 + 5;

    GotoXY(x, y);
    std::cout << menuText;
}

void DrawFullMenu(int selected_index) {
    ClearScreenWithColor(203, 196, 248); // "Sơn" nền Lavender

    int i = 25;
    GotoXY(i, 1);
    SetColorRGB(163, 87, 213);

    // XÓA BỎ: _setmode(_fileno(stdout), _O_U16TEXT);

    // SỬA LẠI: Dùng std::cout
    std::cout << "████████╗██╗  ██╗███████╗     ██████╗ █████╗ ██████╗  ██████╗     ██████╗  █████╗ ███╗   ███╗███████╗\n"; GotoXY(i, 2);
    std::cout << "╚══██╔══╝██║  ██║██╔════╝     ██╔════╝██╔══██╗██╔══██╗██╔═══██╗    ██╔════╝ ██╔══██╗████╗ ████║██╔════╝\n"; GotoXY(i, 3);
    std::cout << "   ██║   ███████║█████╗       ██║     ███████║██████╔╝██║   ██║    ██║  ███╗███████║██╔████╔██║█████╗  \n"; GotoXY(i, 4);
    std::cout << "   ██║   ██╔══██║██╔══╝       ██║     ██╔══██║██╔══██╗██║   ██║    ██║   ██║██╔══██║██║╚██╔╝██║██╔══╝  \n"; GotoXY(i, 5);
    std::cout << "   ██║   ██║  ██║███████╗     ╚██████╗██║  ██║██║  ██║╚██████╔╝    ╚██████╔╝██║  ██║██║ ╚═╝ ██║███████╗\n"; GotoXY(i, 6);
    std::cout << "   ╚═╝   ╚═╝  ╚═╝╚══════╝      ╚═════╝╚═╝  ╚═╝╚═╝  ╚═╝ ╚═════╝      ╚═════╝ ╚═╝  ╚═╝╚═╝     ╚═╝╚══════╝\n";

    // XÓA BỎ: _setmode(_fileno(stdout), _O_TEXT);

    for (int i = 0; i < TOTAL_ITEMS; i++) {
        DrawMenuItem(i, i == selected_index);
    }
}

// --- VẼ TỪNG MỤC PAUSE MENU (ĐÃ SỬA THEO truecolor_utils.h) ---
void DrawPauseMenuItem(int index, bool is_selected) {
    if (index < 0 || index >= TOTAL_PAUSE_ITEMS) return;

    std::string text = std::string(PAUSE_ITEMS[index]);

    if (is_selected) {
        SetColorRGB(255, 100, 180); // Màu chữ NỔI BẬT (Hồng)
    }
    else {
        SetColorRGB(80, 60, 120); // Màu chữ BÌNH THƯỜNG (Tím than)
    }

    // Đặt nền trắng (như code cũ)
    SetBgRGB(255, 255, 255);

    int x = CenterX(text);
    int y = PAUSE_START_Y + index * 2;
    GotoXY(x, y);
    std::cout << text;
}

// --- VẼ TOÀN BỘ PAUSE MENU (ĐÃ SỬA THEO truecolor_utils.h) ---
void DrawFullPauseMenu(int selected_index) {
    // 1. Đặt nền trắng và xóa màn hình
    ClearScreenWithColor(255, 255, 255);

    // --- 2. VẼ TIÊU ĐỀ ---
    const std::string title = "GAME PAUSED";
    SetColorRGB(0, 0, 0); // Chữ tiêu đề màu đen
    GotoXY(CenterX(title), 4);
    std::cout << title;

    // --- 3. VẼ DANH SÁCH MENU ---
    for (int i = 0; i < TOTAL_PAUSE_ITEMS; i++) {
        DrawPauseMenuItem(i, i == selected_index);
    }

    // --- 4. HƯỚNG DẪN ---
    SetColorRGB(80, 60, 120); // Chữ hướng dẫn màu tím than
    const char* guide = "'W': Move up, 'S': Move down,  'Enter': Select,  'ESC': Resume";
    GotoXY(CenterX(guide), PAUSE_START_Y + TOTAL_PAUSE_ITEMS * 2 + 3);
    std::cout << guide;
}

// --- HÀM VẼ KHÁC ---
void DrawTableCellRGB(const std::string& text, int x, int y, int width, int text_r, int text_g, int text_b, int bg_r, int bg_g, int bg_b) {
    SetBgRGB(bg_r, bg_g, bg_b);
    GotoXY(x, y);
    for (int i = 0; i < width; ++i) {
        std::cout << " ";
    }
    SetColorRGB(text_r, text_g, text_b);
    GotoXY(x + 1, y);
    std::cout << text;
}