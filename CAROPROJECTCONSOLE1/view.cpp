#include "View.h"
#include "Windows.h"
#include <conio.h>
#include "Model.h"
#include "Control.h"
#include <iostream>
#include <iomanip>   
#include <string>
#include <vector>
#include <cwchar> 
#include <sstream>
#include <fcntl.h>
#include <io.h>
#include <thread> // Cần cho Intro (View 2)
#include <chrono> // Cần cho Intro (View 2)
#include "GameState.h"
#include "GameAssets.h" 
#include "truecolor_utils.h"
#include <cstdio>

using namespace std;


// 1. GLOBAL VARIABLES & ASSETS (TỪ VIEW 1)

const char* MENU_ITEMS[] = { "NEW GAME", "LOAD GAME", "SETTING", "GUIDE", "ABOUT", "EXIT" };
const char* PAUSE_ITEMS[] = { "RESUME", "RESTART", "SAVE", "SETTINGS", "QUIT" };
const char* NEW_GAME_OPTIONS[] = { "1 PLAYER", "2 PLAYERS", "BACK" };
// canh các mục của menu chính
int ITEM_X[] = { 82, 82, 82, 82 };
int ITEM_Y[] = { 15, 18, 25, 28 };
//
const int TOTAL_ITEMS = sizeof(MENU_ITEMS) / sizeof(MENU_ITEMS[0]);
const int TOTAL_NEW_GAME_OPTIONS = 3;
const int TOTAL_PAUSE_ITEMS = sizeof(PAUSE_ITEMS) / sizeof(PAUSE_ITEMS[0]);

const int START_Y = 15;
const int PAUSE_START_Y = 9;
const int FRAME_START_X = 99;
const int FRAME_WIDTH = 28;

// 2. Màu sắc Theme (Lavender)


#define BG_PURPLE_R  202
#define BG_PURPLE_G  196
#define BG_PURPLE_B  248
// Màu chữ khi KHÔNG chọn (Tím đậm hơn)
#define TEXT_NORMAL_R 80
#define TEXT_NORMAL_G 60
#define TEXT_NORMAL_B 120
// Màu chữ khi ĐANG CHỌN (Hồng rực - nổi bật)
#define TEXT_SELECT_R 255
#define TEXT_SELECT_G 100
#define TEXT_SELECT_B 180
// Màu viền khung (Tím đen)
#define BORDER_R 100
#define BORDER_G 100
#define BORDER_B 200
// Màu logo (ví dụ: Logo CARO LEGENDS)
#define LOGO_R 163
#define LOGO_G 87
#define LOGO_B 213
const char* LOGO_LINE = "CARO LEGENDS";

// 3. Assets cho Game Over (Giữ nguyên từ View 1)

const int ART_HEIGHT = 6;
const std::vector<std::string> WIN_ART = {
    "██╗    ██╗██╗███╗   ██╗",
    "██║    ██║██║████╗  ██║",
    "██║ █╗ ██║██║██╔██╗ ██║",
    "██║███╗██║██║██║╚██╗██║",
    "╚███╔███╔╝██║██║ ╚████║",
    " ╚══╝╚══╝ ╚═╝╚═╝  ╚═══╝"
};
// 2. LOSE ART (Đã chuyển sang vector)
const std::vector<std::string> LOSE_ART = {
    "██╗      ██████╗ ███████╗███████╗",
    "██║     ██╔═══██╗██╔════╝██╔════╝",
    "██║     ██║   ██║███████╗█████╗  ",
    "██║     ██║   ██║╚════██║██╔══╝  ",
    "███████╗╚██████╔╝███████║███████╗ ",
    "╚══════╝ ╚═════╝ ╚══════╝╚══════╝ "
};
// 3. X ART (Giữ nguyên vector)
const std::vector<std::string> ART_X_BIG = {
    "██╗  ██╗",
    "╚██╗██╔╝",
    " ╚███╔╝ ",
    " ██╔██╗ ",
    "██╔╝ ██╗",
    "╚═╝  ╚═╝"
};
// 4. O ART (Giữ nguyên vector)
const std::vector<std::string> ART_O_BIG = {
    " ██████╗ ",
    "██╔═══██╗",
    "██║   ██║",
    "██║   ██║",
    "╚██████╔╝",
    " ╚═════╝ "
};

const vector<string> ASCII_X_WIN = {
    "██╗  ██╗    ██╗    ██╗██╗███╗   ██╗",
    "╚██╗██╔╝    ██║    ██║██║████╗  ██║",
    " ╚███╔╝     ██║ █╗ ██║██║██╔██╗ ██║",
    " ██╔██╗     ██║███╗██║██║██║╚██╗██║",
    "██╔╝ ██╗    ╚███╔███╔╝██║██║ ╚████║",
    "╚═╝  ╚═╝     ╚══╝╚══╝ ╚═╝╚═╝  ╚═══╝"
};

// 2. O WIN
const vector<string> ASCII_O_WIN = {
    " ██████╗     ██╗    ██╗██╗███╗   ██╗",
    "██╔═══██╗    ██║    ██║██║████╗  ██║",
    "██║   ██║    ██║ █╗ ██║██║██╔██╗ ██║",
    "██║   ██║    ██║███╗██║██║██║╚██╗██║",
    "╚██████╔╝    ╚███╔███╔╝██║██║ ╚████║",
    " ╚═════╝      ╚══╝╚══╝ ╚═╝╚═╝  ╚═══╝"
};

// 3. DRAW (HÒA)
const vector<string> ASCII_DRAW = {
    "██████╗  ██████╗  █████╗ ██╗    ██╗",
    "██╔══██╗ ██╔══██╗██╔══██╗██║    ██║",
    "██║  ██║ ██████╔╝███████║██║ █╗ ██║",
    "██║  ██║ ██╔══██╗██╔══██║██║███╗██║",
    "██████╔╝ ██║  ██║██║  ██║╚███╔███╔╝",
    "╚═════╝  ╚═╝  ╚═╝╚═╝  ╚═╝ ╚══╝╚══╝"
};
_POINT _A[BOARD_SIZE][BOARD_SIZE];
bool _TURN = true;
struct ColorTemp {
    int r, g, b;
    bool operator==(const ColorTemp& o) const { return r == o.r && g == o.g && b == o.b; }
};
// 4. SYSTEM & HELPER FUNCTIONS
void GotoXY(int x, int y) {
    COORD coord = { (SHORT)x, (SHORT)y };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}
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

    // Đặt kích thước buffer
    COORD bufferSize = { (SHORT)width, (SHORT)height };
    SetConsoleScreenBufferSize(hConsole, bufferSize);

    // Đặt kích thước cửa sổ
    SMALL_RECT windowSize = { 0, 0, (SHORT)(width - 1), (SHORT)(height - 1) };
    SetConsoleWindowInfo(hConsole, TRUE, &windowSize);
}
void CenterConsole() {
    HWND consoleWindow = GetConsoleWindow();
    if (consoleWindow == NULL) return;
    RECT rectClient, rectScreen;
    GetWindowRect(consoleWindow, &rectClient);
    SystemParametersInfo(SPI_GETWORKAREA, 0, &rectScreen, 0);
    int windowWidth = rectClient.right - rectClient.left;
    int windowHeight = rectClient.bottom - rectClient.top;
    int screenWidth = rectScreen.right - rectScreen.left;
    int screenHeight = rectScreen.bottom - rectScreen.top;
    MoveWindow(consoleWindow, rectScreen.left + (screenWidth - windowWidth) / 2, rectScreen.top + (screenHeight - windowHeight) / 2, windowWidth, windowHeight, TRUE);
}
void FixConsoleWindow() {
    HWND consoleWindow = GetConsoleWindow();
    LONG style = GetWindowLong(consoleWindow, GWL_STYLE);
    style &= ~(WS_MAXIMIZEBOX) & ~(WS_THICKFRAME);
    SetWindowLong(consoleWindow, GWL_STYLE, style);
}
void ClearScreenWithColor(int r, int g, int b) {
    SetBgRGB(r, g, b);
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD coordScreen = { 0, 0 };
    DWORD cCharsWritten;
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    DWORD dwConSize;
    if (!GetConsoleScreenBufferInfo(hConsole, &csbi)) return;
    dwConSize = csbi.dwSize.X * csbi.dwSize.Y;
    FillConsoleOutputCharacter(hConsole, (TCHAR)' ', dwConSize, coordScreen, &cCharsWritten);
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    FillConsoleOutputAttribute(hConsole, csbi.wAttributes, dwConSize, coordScreen, &cCharsWritten);
    SetConsoleCursorPosition(hConsole, coordScreen);
}
void DrawImageHalfBlock(int startX, int startY, const std::vector<DrawInstructionTrueColor>& data) {
    if (data.empty()) return;
    int width = 0, height = 0;
    for (const auto& instr : data) {
        width = (std::max)(width, instr.x + instr.l);
        height = (std::max)(height, instr.y + 1);
    }
    std::vector<std::vector<ColorTemp>> pixelGrid(height, std::vector<ColorTemp>(width, { 0, 0, 0 }));
    for (const auto& instr : data) {
        if (instr.y < height) {
            for (int i = 0; i < instr.l; ++i) {
                if (instr.x + i < width) pixelGrid[instr.y][instr.x + i] = { instr.r, instr.g, instr.b };
            }
        }
    }
    if (height % 2 != 0) { pixelGrid.push_back(std::vector<ColorTemp>(width, { 0, 0, 0 })); height++; }
    std::string buffer;
    ColorTemp lastTop = { -1,-1,-1 }, lastBot = { -1,-1,-1 };
    for (int y = 0; y < height; y += 2) {
        buffer += "\x1b[" + std::to_string(startY + (y / 2) + 1) + ";" + std::to_string(startX + 1) + "H";
        for (int x = 0; x < width; ++x) {
            ColorTemp top = pixelGrid[y][x];
            ColorTemp bot = pixelGrid[y + 1][x];
            if (!(bot == lastBot)) { buffer += "\x1b[48;2;" + std::to_string(bot.r) + ";" + std::to_string(bot.g) + ";" + std::to_string(bot.b) + "m"; lastBot = bot; }
            if (!(top == lastTop)) { buffer += "\x1b[38;2;" + std::to_string(top.r) + ";" + std::to_string(top.g) + ";" + std::to_string(top.b) + "m"; lastTop = top; }
            buffer += "▀";
        }
    }
    buffer += "\x1b[0m";
    std::cout << buffer << std::flush;
}
// 5. START INTRO 
int CenterXW(std::wstring s) {
    return (CONSOLE_WIDTH - s.length()) / 2;
}
void ShowGroupIntro() {
    ClearScreenWithColor(202, 196, 248);
    SetColorRGB(100, 0, 150);

    const char* text[] = {
        " ██████╗ ██████╗  ██████╗ ██╗   ██╗██████╗         ██████╗ ",
        "██╔════╝ ██╔══██╗██╔═══██╗██║   ██║██╔══██╗       ██╔════╝ ",
        "██║  ███╗██████╔╝██║   ██║██║   ██║██████╔╝       ███████╗ ",
        "██║   ██║██╔══██╗██║   ██║██║   ██║██╔═══╝        ██╔═══██╗",
        "╚██████╔╝██║  ██║╚██████╔╝╚██████╔╝██║            ╚██████╔╝",
        " ╚═════╝ ╚═╝  ╚═╝ ╚═════╝  ╚═════╝ ╚═╝             ╚═════╝ "
    };

    int lines = 6;
    int textWidth = 67;

   

    // --- CHỈNH SỬA TẠI ĐÂY ---
    // Bạn hãy thay đổi số 20 này:
    // Tăng lên (ví dụ 30, 40) để dịch sang PHẢI
    // Giảm xuống (hoặc số âm) để dịch sang TRÁI
    int SHIFT_RIGHT = 7;

    int x = (CONSOLE_WIDTH - textWidth) / 2 + SHIFT_RIGHT;
    int y = 17;

    for (int i = 0; i < lines; i++) {
        GotoXY(x, y + i);
        std::cout << text[i];
    }

    Sleep(2000);
}
void StartIntro() {
    // Xóa màn hình ở chế độ thường trước
    ClearScreenWithColor(202, 196, 248);

    // === BẮT ĐẦU CHẾ ĐỘ UNICODE (VÙNG CẤM DÙNG COUT) ===
    _setmode(_fileno(stdout), _O_U16TEXT);

    // Set màu nền Lavender & Chữ Tím (Dùng hàm W)
    SetBgW(BG_PURPLE_R, BG_PURPLE_G, BG_PURPLE_B);
    SetColorW(LOGO_R, LOGO_G, LOGO_B);

    // Vẽ Logo
    int logoY = 5;
    std::wstring logo[] = {
        L"████████╗██╗  ██╗███████╗    ██████╗  █████╗ ██████╗  ██████╗      ██████╗  █████╗ ███╗   ███╗███████╗",
        L"╚══██╔══╝██║  ██║██╔════╝   ██╔════╝ ██╔══██╗██╔══██╗██╔═══██╗    ██╔════╝ ██╔══██╗████╗ ████║██╔════╝",
        L"   ██║   ███████║█████╗     ██║      ███████║██████╔╝██║   ██║    ██║  ███╗███████║██╔████╔██║█████╗  ",
        L"   ██║   ██╔══██║██╔══╝     ██║      ██╔══██║██╔══██╗██║   ██║    ██║   ██║██╔══██║██║╚██╔╝██║██╔══╝  ",
        L"   ██║   ██║  ██║███████╗   ╚██████╗ ██║  ██║██║  ██║╚██████╔╝    ╚██████╔╝██║  ██║██║ ╚═╝ ██║███████╗",
        L"   ╚═╝   ╚═╝  ╚═╝╚══════╝    ╚═════╝ ╚═╝  ╚═╝╚═╝  ╚═╝ ╚═════╝      ╚═════╝ ╚═╝  ╚═╝╚═╝     ╚═╝╚══════╝"
    };

    for (int i = 0; i < 6; i++) {
        GotoXY(CenterXW(logo[i]), logoY + i);
        std::wcout << logo[i];
    }

    // Loading Bar
    int barWidth = 60;
    int barY = 20;
    int barX = (CONSOLE_WIDTH - barWidth) / 2;

    SetColorW(80, 80, 80); // Dùng hàm W
    GotoXY(barX - 1, barY - 1); std::wcout << L"╔" << std::wstring(barWidth, L'═') << L"╗";
    GotoXY(barX - 1, barY);     std::wcout << L"║";
    GotoXY(barX + barWidth, barY); std::wcout << L"║";
    GotoXY(barX - 1, barY + 1); std::wcout << L"╚" << std::wstring(barWidth, L'═') << L"╝";

    GotoXY(CenterXW(L"LOADING..."), barY - 2);
    std::wcout << L"LOADING...";

    for (int i = 0; i <= barWidth; ++i) {
        int percent = (i * 100) / barWidth;

        GotoXY(barX + i, barY);
        if (i < barWidth) {
            SetColorW(100, 100, 255); // Xanh dương (Dùng hàm W)
            std::wcout << L"█";
        }

        // Nếu muốn reset màu ở đây thì phải dùng ResetColorW()
        // Nhưng ở đây ta set đè màu luôn nên k cần reset
        SetBgW(97, 100, 151); SetColorW(80, 80, 80);

        std::wstring perStr = std::to_wstring(percent) + L"%";
        GotoXY(CenterXW(perStr), barY + 2);
        std::wcout << perStr;

        if (i < 20) std::this_thread::sleep_for(std::chrono::milliseconds(20));
        else if (i < 50) std::this_thread::sleep_for(std::chrono::milliseconds(5));
        else std::this_thread::sleep_for(std::chrono::milliseconds(30));
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(300));

    while (_kbhit()) {
        _getch(); // Đọc và vứt hết các phím thừa đi
    }
    // Press Any Key
    GotoXY(0, barY - 2); std::wcout << std::wstring(120, L' ');
    GotoXY(0, barY + 2); std::wcout << std::wstring(120, L' ');

    std::wstring prompt = L">> PRESS ANY KEY TO START <<";
    int promptX = CenterXW(prompt);
    bool showText = true;

    while (true) {
        if (_kbhit()) { _getch(); break; }

        if (showText) {
            SetColorW(255, 50, 50); // Đỏ (Dùng hàm W)
            GotoXY(promptX, barY + 2);
            std::wcout << prompt;
        }
        else {
            GotoXY(promptX, barY + 2);
            std::wcout << std::wstring(prompt.length(), L' ');
        }
        showText = !showText;
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    // === KẾT THÚC CHẾ ĐỘ UNICODE ===
    // Trả về chế độ thường ngay lập tức
    _setmode(_fileno(stdout), _O_TEXT);

    // Giờ đã về Text mode -> Dùng ResetColor cũ thoải mái
    ResetColor();
    ClearScreenWithColor(0, 0, 0);
}
// 6. MENU & GAME FLOW 
void DrawFrame(int x, int y, int width, int height) {
    // Số ký tự ngang để lặp (trừ 2 ký tự góc)
    int inner_width = width - 2;
    // Vẽ viền trên
    GotoXY(x, y);
    std::wcout << L"╔";
    for (int i = 0; i < inner_width; ++i) {
        std::wcout << L"═";
    }
    std::wcout << L"╗";
    // Vẽ viền dọc
    for (int i = 1; i < height - 1; ++i) {
        // Bên trái
        GotoXY(x, y + i);
        std::wcout << L"║";
        // Bên phải
        GotoXY(x + width - 1, y + i);
        std::wcout << L"║";
    }
    // Vẽ viền dưới
    GotoXY(x, y + height - 1);
    std::wcout << L"╚";
    for (int i = 0; i < inner_width; ++i) {
        std::wcout << L"═";
    }
    std::wcout << L"╝";
}
void DrawFrame1(int x, int y, int width, int height) {
    // Số ký tự ngang để lặp (trừ 2 ký tự góc)
    int inner_width = width - 2;
    // Vẽ viền trên
    GotoXY(x, y);
    std::wcout << L"┌";
    for (int i = 0; i < inner_width; ++i) {
        std::wcout << L"─";
    }
    std::wcout << L"┐";
    // Vẽ viền dọc
    for (int i = 1; i < height - 1; ++i) {
        // Bên trái
        GotoXY(x, y + i);
        std::wcout << L"│";
        // Bên phải
        GotoXY(x + width - 1, y + i);
        std::wcout << L"│";
    }
    // Vẽ viền dưới
    GotoXY(x, y + height - 1);
    std::wcout << L"└";
    for (int i = 0; i < inner_width; ++i) {
        std::wcout << L"─";
    }
    std::wcout << L"┘";
}
// Vẽ vạch ngang
void horizontalLine(int X, int Y, int width, int height) {
    for (int i = 0; i < width; i++) {
        GotoXY(X + i, Y + height);
        std::wcout << L"─";
    }
}
void StartAbout() {
    GabageCollect(); 
    ClearScreenWithColor(202, 196, 248);
    _setmode(_fileno(stdout), _O_U16TEXT);
    // Set màu nền Lavender & Chữ Tím (Dùng hàm W)
    SetBgW(BG_PURPLE_R, BG_PURPLE_G, BG_PURPLE_B);
    SetColorW(LOGO_R, LOGO_G, LOGO_B);
    DrawFrame(2, 1, 162, 53);
    int aboutLogoY = 5;
    std::wstring aboutLogo[] = {
        L" █████╗ ██████╗  ██████╗ ██╗   ██╗████████╗",
        L"██╔══██╗██╔══██╗██╔═══██╗██║   ██║╚══██╔══╝",
        L"███████║██████╔╝██║   ██║██║   ██║   ██║   ",
        L"██╔══██║██╔══██╗██║   ██║██║   ██║   ██║   ",
        L"██║  ██║██████╔╝╚██████╔╝╚██████╔╝   ██║   ",
        L"╚═╝  ╚═╝╚═════╝  ╚═════╝  ╚═════╝    ╚═╝   "
    };

    for (int i = 0; i < 6; i++) {
        GotoXY(CenterXW(aboutLogo[i]), aboutLogoY + i);
        std::wcout << aboutLogo[i];
    }

    COORD tableCoord{ 40, 10 };
    int i = 0, offset = 2;
    GotoXY(tableCoord.X + 5, tableCoord.Y);

    int inforLogoY = 20;
    std::wstring inforLogo[] = {
        L"Welcome to the Gomoku Game - made by team 6 - 25CTT3 - HCMUS",
        L"25120215 - Nguyen Minh Nhat\t\t PvE Mode - Game Content",
        L"25120193 - Nguyen Hoang Huy\t \t Game Interface Design",
        L"25120219 - Vu Thanh Phong \t\t\t Game Graphic Design - Game Flow Design",
        L"25120223 - Do Le Nhat Quang \t \t Game Interface Design - Game Sound",
        L"25120224 - Nguyen Phu Quang\ \t \t Load & Save Game",
        L"25120225 - Nguyen Vu Nhat Quang\t \t Game Graphic Design"
    };
    for (int i = 0; i < 7; i++) {
        if (i == 0) {
            GotoXY(tableCoord.X + 10, tableCoord.Y + 3);
            std::wcout << inforLogo[i];
        }
        else {
            GotoXY(45, inforLogoY + i * offset);
            std::wcout << inforLogo[i];
        } 
    }
    
    _setmode(_fileno(stdout), _O_TEXT);

    const char* guide = "'ESC': quit";
    GotoXY(CenterX(guide), 50);
    std::cout << guide;

    // Vòng chờ phím bấm
    while (true) {
        if (_kbhit()) {
            int key = _getch();
            if (key == 27 || key == 13) break; // ESC hoặc Enter
        }
        Sleep(50);
    }

    // Trả chế độ xuất về bình thường (O_TEXT)
    _setmode(_fileno(stdout), _O_TEXT);

    // Không cần _getch() nữa (vì trên đã chờ phím)
    ClearScreenWithColor(0, 0, 0);
    ResetColor();
    // SceneHandle("MAIN MENU"); 
    currentState = MENU;
}
void StartGuide() {
    GabageCollect();
    ClearScreenWithColor(202, 196, 248);
    _setmode(_fileno(stdout), _O_U16TEXT);
    // Set màu nền Lavender & Chữ Tím (Dùng hàm W)
    SetBgW(BG_PURPLE_R, BG_PURPLE_G, BG_PURPLE_B);
    SetColorW(LOGO_R, LOGO_G, LOGO_B);
    DrawFrame(2, 1, 162, 53);
    int X = 26, Y = 15;
    DrawFrame1(X - 1, Y - 1, 117, 32);
    int guideLogoY = 5;
    std::wstring guideLogo[] = {
        L" ██████╗ ██╗   ██╗██╗██████╗ ███████╗",
        L"██╔════╝ ██║   ██║██║██╔══██╗██╔════╝",
        L"██║  ███╗██║   ██║██║██║  ██║███████╗",
        L"██║   ██║██║   ██║██║██║  ██║██╔════╝",
        L"╚██████╔╝╚██████╔╝██║██████╔╝███████╗",
        L" ╚═════╝  ╚═════╝ ╚═╝╚═════╝ ╚══════╝"
    };
    for (int i = 0; i < 6; i++) {
        GotoXY(CenterXW(guideLogo[i]), guideLogoY + i);
        std::wcout << guideLogo[i];
    }
    GotoXY(X + 38, Y + 1);
    std::wcout << L"Game features and in-game operations: ";
    // Vẽ vạch ngang
    horizontalLine(X, Y, 115, 3);
    std::wstring Player1Guide[] = {
        L"Player 1 Movement: ",
        L"\t'W': Go up",
        L"\t'S': Go down",
        L"\t'A': Go left",
        L"\t'D': Go right",
        L"\t'F': Mark"
    };
    for (int i = 0; i < 6; i++) {
        GotoXY(X + 5, Y + i*2 + 4);
        std::wcout << Player1Guide[i];
    }
    std::wstring Player2Guide[] = {
        L"Player 2 Movement: ",
        L"\t'↑': Go up",
        L"\t'↓': Go down",
        L"\t'←': Go left",
        L"\t'→': Go right",
        L"\t'Enter': Mark"
    };
    for (int i = 0; i < 6; i++) {
        GotoXY(X + 65, Y + i*2 + 4);
        std::wcout << Player2Guide[i];
    }
    // Vẽ vạch ngăn cách
    for (int i = 0; i < 13; i++) {
        GotoXY(81, Y + i + 4);
        std::wcout << L"│";
    }
    horizontalLine(X, Y, 115, 17);
    GotoXY(X + 38, Y + 19);
    std::wcout << L"Caro Game Rules Summary : ";
    horizontalLine(X, Y, 115, 21);
    std::wstring guideRule[] = {
        L"Caro is the game using the X and O symbols to represent players and followed by a set of rules: ",
        L"",
        L"1. Players take turns placing their marks(X or O) on empty intersections.",
        L"2. \"X\" always plays first, followed by \"O\" player.",
        L"3. The first player to score five points in a row horizontally, vertically, or diagonally wins the game.",
        L"4. If the board is completely filled and no player has five in a row, the game ends in a draw."
    };
    for (int i = 0; i < 6; i++) {
        GotoXY(X + 5, Y + i + 23);
        std::wcout << guideRule[i];
    }

    // Trả chế độ xuất về bình thường (O_TEXT)
    _setmode(_fileno(stdout), _O_TEXT);
    const char* guide = "'ESC': quit";
    GotoXY(CenterX(guide), 50);
    std::cout << guide;

    // Vòng chờ phím bấm
    while (true) {
        if (_kbhit()) {
            int key = _getch();
            if (key == 27 || key == 13) break; // ESC hoặc Enter
        }
        Sleep(50);
    }

    // Không cần _getch() nữa (vì trên đã chờ phím)
    ClearScreenWithColor(0, 0, 0);
    ResetColor();
    // SceneHandle("MAIN MENU"); 
    currentState = MENU;
}
void DrawMenuItem(int index, bool is_selected) {
    if (index < 0 || index >= TOTAL_ITEMS) return;

    // --- CẤU HÌNH TỌA ĐỘ ---
    int POS_X[] = { 74, 74, 74, 75, 75, 75 };
    int POS_Y[] = { 19, 22, 25, 28, 31, 34 };

    int x = POS_X[index];
    int y = POS_Y[index];

    std::string text = MENU_ITEMS[index];
    std::string displayStr;

    // --- CHỌN KÝ TỰ TRANG TRÍ Ở ĐÂY ---
    // char(175) là dấu » 
    // char(16)  là dấu ►
    // char(4)   là dấu ♦ (Hình thoi - hợp với game bài/caro)
    // char(15)  là dấu ☼ (Mặt trời - giống bông tuyết nhất trong font này)

    char icon = char(15); // Đang để là dấu »

    if (is_selected) {
        // Cộng chuỗi kiểu này để chèn ký tự đặc biệt vào
        displayStr = std::string(" ") + icon + " " + text + " " + icon + " ";
    }
    else {
        displayStr = "   " + text + "   ";
    }

    // --- VẼ (Dùng cout bình thường) ---
    GotoXY(x, y);
    SetBgRGB(218, 66, 76); // Nền đỏ

    if (is_selected) {
        SetColorRGB(255, 255, 255); // Trắng
        std::cout << displayStr;
    }
    else {
        SetColorRGB(255, 255, 0); // Vàngs
        std::cout << displayStr;
    }

    // Reset màu
    SetBgRGB(0, 0, 0);
    SetColorRGB(255, 255, 255);
}
void DrawFullMenu(int selected_index) {
    ClearScreenWithColor(97, 100, 151);

 
    SetBgRGB(218, 66, 76);
    DrawBgm(0, 0);
    DrawFramemainmenu(66, 16);
    for (int i = 0; i < TOTAL_ITEMS; i++) {
        DrawMenuItem(i, i == selected_index);
    }
}
void DrawNewGameMenuItem(int index, bool is_selected) {
    // --- 1. CẤU HÌNH TỌA ĐỘ (CHỈNH Ở ĐÂY) ---
    // Bạn điền toạ độ X, Y cho từng mục menu con tại đây
    // Giả sử có 4 mục (PvP, PvE, Hard, Back...)
    // Mình để tạm X = 75, Y cách nhau 3 dòng cho thoáng
    int POS_X[] = { 75, 75, 77};
    int POS_Y[] = { 24, 27, 30};

    // Kiểm tra index để tránh lỗi
    // Thay số 4 bằng số lượng mục thực tế trong NEW_GAME_OPTIONS của bạn
    if (index < 0 || index >= sizeof(POS_X) / sizeof(POS_X[0])) return;

    int x = POS_X[index];
    int y = POS_Y[index];

    std::string text = NEW_GAME_OPTIONS[index];
    std::string displayStr;

    // --- 2. STYLE ICON & CHUỖI (Giống Menu chính) ---
    char icon = char(15); // Dấu ☼

    if (is_selected) {
        displayStr = std::string(" ") + icon + " " + text + " " + icon + " ";
    }
    else {
        displayStr = "   " + text + "   ";
    }

    // --- 3. VẼ ---
    GotoXY(x, y);
    SetBgRGB(218, 66, 76); // Nền Đỏ

    if (is_selected) {
        SetColorRGB(255, 255, 255); // Chọn: Trắng
        std::cout << displayStr;
    }
    else {
        SetColorRGB(255, 255, 0);   // Thường: Vàng
        std::cout << displayStr;
    }

    // --- 4. RESET MÀU ---
    SetBgRGB(0, 0, 0);
    SetColorRGB(255, 255, 255);
}
void DrawFullNewGameMenu(int selected_index) {
    // 1. Xóa màn hình và vẽ nền
    ClearScreenWithColor(97, 100, 151); // Dùng màu nền của bạn
    DrawLargebackground1(0, 0);
    DrawGamemodeframe(67, 21);
    DrawGamemode(23, 2);
    DrawTutorial(37, 40);
    SetBgRGB(176, 140, 88);
    SetColorRGB(0, 0, 0);
    GotoXY(40, 42); std::cout << "'W, S': MOVE";
    GotoXY(40, 44); std::cout << "BETWEEN MENU";
    GotoXY(40, 46); std::cout << "'ESC': QUIT";
    // 3. Vẽ tất cả các mục menu
    SetColorRGB(255, 100, 180);
    for (int i = 0; i < TOTAL_NEW_GAME_OPTIONS; ++i) {
        DrawNewGameMenuItem(i, (i == selected_index));
    }
}
void DrawFull2PlayerNameScreen() {
    ClearScreenWithColor(97, 100, 151);
    SetColorRGB(255, 255, 255);
    DrawLargebackground1(0, 0);
    DrawPlayerframe(42, 22);
    DrawPlayerframe(95, 22);
    DrawBackframe(78, 32);
    Draw2Players(24, 2);
    SetColorRGB(255, 255, 255);
    SetBgRGB(218, 66, 76);
    GotoXY(44, 23); std::cout << ">> Player's Name1 <<";
    GotoXY(97, 23); std::cout << ">> Player's Name2 <<";
    GotoXY(44, 24); std::cout << "*Less than 20 characters";
    GotoXY(97, 24); std::cout << "*Less than 20 characters";
    GotoXY(44, 26); std::cout << "Enter: ";
    GotoXY(97, 26); std::cout << "Enter: ";
    DrawTutorial(37, 40);
    SetBgRGB(176, 140, 88);
    SetColorRGB(0, 0, 0);
    GotoXY(40, 42); std::cout << "'ENTER': CONFIRM";
    GotoXY(40, 44); std::cout << "'TAB': SWITCH";
    GotoXY(40, 46); std::cout << "'ESC': QUIT";
    
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
void Update2PlayerNameScreen(int activeControl, const char* p1_buffer, const char* p2_buffer) {
    // activeControl: 0=P1, 1=P2, 2=Back
    SetBgRGB(218, 66, 76);
    // --- 1. VẼ TÊN PLAYER 1 ---
    GotoXY(51, 26);
    if (activeControl == 0) {
        SetColorRGB(255, 255, 255); // Đang nhập: Chữ Trắng sáng
        SetBgRGB(218, 66, 76);       
    }
    else {
        SetColorRGB(255, 255, 255);
        SetBgRGB(218, 66, 76);          
    }
    // In tên + khoảng trắng thừa để xóa ký tự cũ nếu bấm Backspace
    // Giả sử tên tối đa 15 ký tự, ta in đè khoảng trắng lên
    std::cout << p1_buffer;
    for (int i = strlen(p1_buffer); i < 20; i++) std::cout << " ";

    // Reset màu sau khi vẽ xong P1


    // --- 2. VẼ TÊN PLAYER 2 ---
    GotoXY(104, 26);
    if (activeControl == 1) {
        SetColorRGB(255, 255, 255);
        SetBgRGB(218, 66, 76);
    }
    else {
        SetColorRGB(255, 255, 255);
        SetBgRGB(218, 66, 76);
    }
   
    std::cout << p2_buffer;
    for (int i = strlen(p2_buffer); i < 20; i++) std::cout << " ";

    SetBgRGB(218, 66, 76); // Reset màu


    // --- 3. VẼ NÚT BACK ---
    // Nút Back nằm ở dưới
    int backX = CenterX("[ BACK ]") + 1;
    int backY = 33;
    GotoXY(backX, backY);

    if (activeControl == 2) {
        SetColorRGB(255, 255, 0);
        std::cout << " ☼ BACK ☼";
    }
    else {
        SetColorRGB(255, 255, 255); 
        std::cout << "   BACK   ";
    }

    // Reset về mặc định cuối hàm để không ảnh hưởng chỗ khác
    ResetColor();
}
void DrawBoard(int pSize) {

    SetColorRGB(0, 0, 0);
    SetBgRGB(97, 100, 151);

    for (int j = 0; j <= pSize; j++) {
        for (int i = 0; i <= pSize; i++) {

            // 1. Dùng biến toàn cục từ view.h để tính tọa độ
            int x = LEFT + CELL_VISUAL_WIDTH * i;   
            int y = TOP + CELL_VISUAL_HEIGHT * j;  
            GotoXY(x, y);

            // 2. Vẽ các giao điểm (Giữ nguyên logic)
            if (i == 0 && j == 0) std::cout << "╔";
            else if (i == pSize && j == 0) std::cout << "╗";
            else if (i == 0 && j == pSize) std::cout << "╚";
            else if (i == pSize && j == pSize) std::cout << "╝";
            else if (j == 0) std::cout << "╦";
            else if (j == pSize) std::cout << "╩";
            else if (i == 0) std::cout << "╠";
            else if (i == pSize) std::cout << "╣";
            else std::cout << "╬";

            // 3. Vẽ đường ngang (Dùng vòng lặp để tự động thích nghi độ dài)
            if (i < pSize) {
                // Chạy vòng lặp dựa trên độ rộng đã cài trong view.h
                // Trừ 1 là trừ đi cái ký tự giao điểm vừa vẽ ở trên
                for (int k = 0; k < CELL_VISUAL_WIDTH - 1; k++) std::cout << "═";
            }

            // 4. Vẽ đường dọc (Dùng vòng lặp để tự động thích nghi độ cao)
            if (j < pSize) {
                // Chạy vòng lặp dựa trên độ cao đã cài trong view.h
                for (int k = 1; k < CELL_VISUAL_HEIGHT; k++) {
                    GotoXY(x, y + k);
                    std::cout << "║";
                }
            }
        }
    }
}
void RedrawBoardState() {
    for (int i = 0; i < BOARD_SIZE; i++) {      // i là dòng (Row - Y)
        for (int j = 0; j < BOARD_SIZE; j++) {  // j là cột (Col - X)

            if (_A[i][j].c != 0) {
                // --- BƯỚC 1: TÍNH TOÁN TỌA ĐỘ PIXEL TRÊN MÀN HÌNH ---
                // (Phải dùng các hằng số LEFT, TOP, CELL_VISUAL_WIDTH giống hàm GotoBoard)

                int visualX = LEFT + (j * CELL_VISUAL_WIDTH);  // j là cột -> nhân với chiều rộng
                int visualY = TOP + (i * CELL_VISUAL_HEIGHT);  // i là dòng -> nhân với chiều cao
                int padX = 2;
                int padY = 1;
                

                // --- BƯỚC 2: TRUYỀN TỌA ĐỘ PIXEL VÀO HÀM VẼ ---
                if (_A[i][j].c == -1) {
                    DrawXmini(visualX + padX, visualY + padY); // Đã sửa
                }
                else {
                    DrawOmini(visualX + padX, visualY + padY); // Đã sửa
                }
            }
        }
    }
}
void GotoBoard(int pX, int pY) {
    int startX = LEFT + pX * CELL_VISUAL_WIDTH;
    int startY = TOP + pY * CELL_VISUAL_HEIGHT;
    int screenX = startX + (CELL_VISUAL_WIDTH / 2);
    int screenY = startY + (CELL_VISUAL_HEIGHT / 2);
    GotoXY(screenX, screenY + 1);
}
/*
 * Chức năng: Vẽ các thành phần TĨNH của giao diện 2 người chơi MỘT LẦN.
 * Ví dụ: tên người chơi, chữ "WINS:", "MOVES:".
 */
void DrawStatic2P_UI() {
    const int p1_box_x = 14, p1_box_y = 5;  // Căn chỉnh lại tọa độ cho đẹp
    const int p2_box_x = 133, p2_box_y = 5;

    // Vẽ cho Player 1
    SetBgRGB(135, 236, 255);
    GotoXY(p1_box_x - 3, p1_box_y + 5);     PrintCenteredText(std::string(_player1_name) + " (X) ", 23); // Tự động canh giữa trong 29 ô
    GotoXY(p1_box_x - 2, p1_box_y + 7); std::cout << "WINS: ";
    GotoXY(p1_box_x - 2, p1_box_y + 9); std::cout << "TIME LEFT: ";
    GotoXY(p1_box_x - 2, p1_box_y + 11); std::cout << "MOVES: ";

    // Vẽ cho Player 2
    GotoXY(p2_box_x - 2, p2_box_y + 5);     PrintCenteredText(std::string(_player2_name) + " (O) ", 23); // Tự động canh giữa trong 29 ô
    GotoXY(p2_box_x - 1, p2_box_y + 7); std::cout << "WINS: ";
    GotoXY(p2_box_x - 1, p2_box_y + 9); std::cout << "TIME LEFT: ";
    GotoXY(p2_box_x - 1, p2_box_y + 11); std::cout << "MOVES: ";
    GotoXY(78, 5); std::cout << "ROUND: " << _round;
}
/*
 * Chức năng: CHỈ CẬP NHẬT các giá trị ĐỘNG (số điểm, số lượt đi, ngôi sao).
 * Hàm này được gọi liên tục nhưng không gây nhấp nháy.
 */
// thêm hàm DrawBigArt_Color, showchristmasResult và sửa lại hàn updatedynamic2p_UI, handle2PlayerGame

void UpdateDynamic2P_UI() {
    int X_BIG_POS_X = 34;  
    int X_BIG_POS_Y = 21;   
    int O_BIG_POS_X = 123; 
    int O_BIG_POS_Y = 21;  
    int bgR = 135, bgG = 236, bgB = 255;    
    int xRedR = 255, xRedG = 82, xRedB = 82;   
    int oGrnR = 105, oGrnG = 240, oGrnB = 174;
    if (_currentPlayer == 1) {
        DrawBigArt_Color(X_BIG_POS_X, X_BIG_POS_Y, ART_X_BIG, 255, 234, 0);
        DrawBigArt_Color(O_BIG_POS_X, O_BIG_POS_Y, ART_O_BIG, oGrnR, oGrnG, oGrnB);
    }
    else {
        DrawBigArt_Color(X_BIG_POS_X, X_BIG_POS_Y, ART_X_BIG, xRedR, xRedG, xRedB);
        DrawBigArt_Color(O_BIG_POS_X, O_BIG_POS_Y, ART_O_BIG, 255, 234, 0);
    }
    
    // Tọa độ khung điểm 
    const int p1_box_x = 14, p1_box_y = 5;
    const int p2_box_x = 133, p2_box_y = 5;
    SetColorRGB(0, 0, 0); // Màu chữ đen
    SetBgRGB(bgR, bgG, bgB);
    // Điểm P1
    GotoXY(p1_box_x + 4, p1_box_y + 7);
    std::cout << _player1_score << "  ";
    GotoXY(p1_box_x + 5, p1_box_y + 11);
    std::cout << (_moveCount + 1) / 2 << "  ";
    // Điểm P2
    GotoXY(p2_box_x + 5, p2_box_y + 7);
    std::cout << _player2_score << "  ";
    GotoXY(p2_box_x + 6, p2_box_y + 11);
    std::cout << _moveCount / 2 << "  ";
    // round
    GotoXY(85, 5);
    std::cout << _round;
    // Tính phút và giây
    int min = _turnTimer / 60;
    int sec = _turnTimer % 60;
    char runningTime[30];
    sprintf_s(runningTime, "%02d:%02d", min, sec);

    // 2. Tạo chuỗi thời gian ĐẦY (cho người đang chờ)
    char fullTime[] = "02:00";

    // 3. Hiển thị dựa trên lượt của ai
    if (_currentPlayer == 1) {
        // Player 1 đang đánh -> Hiện giờ chạy
        GotoXY(p1_box_x + 9, p1_box_y + 9);
        SetColorRGB(0, 0, 0);
        std::cout << runningTime;

        // Player 2 đang chờ -> Hiện 02:00 tĩnh
        GotoXY(p2_box_x + 10, p2_box_y + 9);
        SetColorRGB(0, 0, 0);   // Màu đen bình thường
        std::cout << fullTime;
    }
    else {
        // Player 2 đang đánh -> Hiện giờ chạy
        GotoXY(p2_box_x + 10, p2_box_y + 9);
        SetColorRGB(0, 0, 0);
        std::cout << runningTime;

        // Player 1 đang chờ -> Hiện 02:00 tĩnh
        GotoXY(p1_box_x + 9, p1_box_y + 9);
        SetColorRGB(0, 0, 0);
        std::cout << fullTime;
    }
}
void DrawBigArt_Color(int x, int y, const std::vector<std::string>& art, int r, int g, int b) {
    SetBgRGB(97, 100, 151); // Luôn set nền Tím (cho trùng background game)
    SetColorRGB(r, g, b);  // Set màu chữ (Vàng hoặc Đỏ/Xanh)

    for (int i = 0; i < art.size(); i++) {
        GotoXY(x, y + i);
        std::cout << art[i];
    }
}
void DrawGameUI_2P() {
    ClearScreenWithColor(97, 100, 151);
    
    DrawSnowbrick(0, 47);
    DrawChimcanhcut(0, 24);
    DrawSnowman(120, 24);
    DrawSnowlength(46, 46);
    DrawFrameingame(7, 8);
    DrawFrameingame(127, 8);
    DrawRound(75, 0);
    DrawBoard(BOARD_SIZE); // Ve man hinh game
    // 2. Vẽ các thông tin TĨNH (Tên, khung, chữ WINS, MOVES...)
    // Bắt buộc phải có, nếu không Resume xong chỉ thấy mỗi số điểm bay lơ lửng
    SetColorRGB(0, 0, 0); // Chọn màu chữ (ví dụ đen)
    DrawStatic2P_UI();

    // 3. Vẽ các thông tin ĐỘNG hiện tại (Điểm số, lượt đi đang dở dang)
    // Để khi Resume, nó hiện đúng điểm số cũ chứ không phải số 0
    UpdateDynamic2P_UI();

    
}
// 9. GAME OVER SCREEN
void DrawFixedLine(int startX, int y, std::string label, std::string value, int width) {
    GotoXY(startX, y);
    SetBgRGB(135, 236, 255); 
    SetColorRGB(0, 0, 0);  

    

    // 2. Vẽ Nhãn
    std::cout << label;

    // 3. Căn chỉnh khoảng cách để cột giá trị thẳng hàng (Bắt đầu tại ký tự thứ 12)
    int currentPos = 2 + label.length();
    int valueStartPos = 12;
    if (currentPos < valueStartPos) std::cout << std::string(valueStartPos - currentPos, ' ');

    // 4. Vẽ Giá trị
    std::cout << value;

    // 5. Lấp đầy khoảng trắng còn lại
    int printedLen = (currentPos > valueStartPos ? currentPos : valueStartPos) + value.length();
    int padding = width - 7 - printedLen; // width - 2 viền
    if (padding < 0) padding = 0;
    std::cout << std::string(padding, ' ');

}
void DrawGameOverScreen(int selectedOption, bool drawAll) {
    // --- THÔNG SỐ KHUNG ---
    const int BOX_WIDTH = 50;
    const int BOX_HEIGHT = 17; // Cao 17 dòng để chứa nút bấm thoải mái

    // Tự động căn giữa màn hình
    const int BOX_X = (CONSOLE_WIDTH - BOX_WIDTH) / 2;
    const int BOX_Y = 16;

    int p1_Art_X = 0, p1_Art_Y = 0;
    int p2_Art_X = 0, p2_Art_Y = 0;

   
    // A. VẼ NỀN, TIÊU ĐỀ, KHUNG & ART 
    if (drawAll) {
        ClearScreenWithColor(97, 100, 151); // Xóa màn hình màu tím
        DrawLargebackground(0, 0);

      

        // Tiêu đề
        DrawGameresults(10, 2);
        DrawNewtutorial(22, 30);
        // Hướng dẫn phím bấm
        SetColorRGB(0, 0, 0);
        SetBgRGB(176, 140, 88);
        GotoXY(25, 32); std::cout << "'TAB': SWITCH";
        GotoXY(25, 34); std::cout << "'ENTER': CONFIRM";
        GotoXY(25, 36); std::cout << "'ESC': QUIT";

        // --- XỬ LÝ LOGIC THẮNG/THUA ---
        std::string headerMsg;
        int winner = 0;
        if (_gameWinner == 1) { winner = 1; headerMsg = "Player " + std::string(_player1_name) + " (X) won!"; }
        else if (_gameWinner == 2) { winner = 2; headerMsg = "Player " + std::string(_player2_name) + " (O) won!"; }
        else { headerMsg = "It's a Draw!"; }

        // --- VẼ ASCII ART (X WIN / O LOSE) ---
        if (winner != 0) {
            SetBgRGB(97, 100, 151);

            const std::vector<std::string>* p1_Char;
            const std::vector<std::string>* p1_Result;
            const std::vector<std::string>* p2_Char;
            const std::vector<std::string>* p2_Result;
            int colorP1, colorP2;

            if (winner == 1) { // === TRƯỜNG HỢP X THẮNG ===
                // 1. Cấu hình nội dung & màu
                p1_Char = &ART_X_BIG; p1_Result = &WIN_ART; colorP1 = 0xFFD700;
                p2_Char = &ART_O_BIG; p2_Result = &LOSE_ART; colorP2 = 0x87CEFA;

                // 2. Cấu hình VỊ TRÍ (Custom cho X thắng)
                // Ví dụ: Khi X thắng, bạn muốn X đứng cao hơn một chút và lệch trái nhiều hơn
                p1_Art_X = 11;
                p1_Art_Y = BOX_Y + 5;   // Cao hơn mặc định

                p2_Art_X = 113; // O thua bị đẩy xa hơn
                p2_Art_Y = BOX_Y + 5;   // Thấp hơn (kiểu bị đánh bại)
            }
            else { // === TRƯỜNG HỢP O THẮNG ===
                // 1. Cấu hình nội dung & màu
                p1_Char = &ART_X_BIG; p1_Result = &LOSE_ART; colorP1 = 0x87CEFA;
                p2_Char = &ART_O_BIG; p2_Result = &WIN_ART; colorP2 = 0xFFD700;

                // 2. Cấu hình VỊ TRÍ (Custom cho O thắng)
                // Ví dụ: Khi O thắng, bạn muốn O đứng chính giữa phía trên, X bị ẩn hoặc dời đi chỗ khác
                p1_Art_X = 7;  // X thua bị đẩy xa tít mù
                p1_Art_Y = BOX_Y + 5;

                p2_Art_X = 119; // O thắng đứng gần khung hơn
                p2_Art_Y = BOX_Y + 5;   // Cao lên ăn mừng
            }

            // --- VẼ THỰC TẾ (Dùng biến tọa độ đã set ở trên) ---

            // Vẽ P1 (X)
            SetColorRGB((colorP1 >> 16) & 0xFF, (colorP1 >> 8) & 0xFF, colorP1 & 0xFF);
            for (int i = 0; i < ART_HEIGHT; ++i) {
                GotoXY(p1_Art_X, p1_Art_Y + i); // Dùng biến p1_Art_Y
                std::cout << (*p1_Char)[i];
                GotoXY(p1_Art_X + 11, p1_Art_Y + i);
                std::cout << (*p1_Result)[i];
            }

            // Vẽ P2 (O)
            SetColorRGB((colorP2 >> 16) & 0xFF, (colorP2 >> 8) & 0xFF, colorP2 & 0xFF);
            for (int i = 0; i < ART_HEIGHT; ++i) {
                GotoXY(p2_Art_X, p2_Art_Y + i); // Dùng biến p2_Art_Y
                std::cout << (*p2_Char)[i];
                GotoXY(p2_Art_X + 12, p2_Art_Y + i);
                std::cout << (*p2_Result)[i];
            }
        }

        // --- VẼ HỘP KẾT QUẢ (NÉT ĐỨT + NỀN TÍM) ---
        SetBgRGB(97, 100, 151);
        SetColorRGB(0, 0, 0); // Viền đen

        DrawGameresultframe(BOX_X, BOX_Y);

        // --- ĐIỀN NỘI DUNG ---
        GotoXY(BOX_X + 3, BOX_Y + 2);
        int padHead = (BOX_WIDTH - 2 - headerMsg.length()) / 2;
        if (padHead < 0) padHead = 0;
        SetBgRGB(135, 236, 255);
        SetColorRGB(0, 0, 0);
        std::cout << std::string(padHead, ' ') << headerMsg;

        // Dữ liệu
        std::stringstream ss_score;
        ss_score << std::setw(2) << std::setfill('0') << _player1_score << "  :  "
            << std::setw(2) << std::setfill('0') << _player2_score;

        std::string names = std::string(_player1_name) + "(X) | (O)" + std::string(_player2_name);

        // Moves: 5 (X) | (O) 4
        int xMoves = (_moveCount + 1) / 2;
        int oMoves = _moveCount / 2;
        std::string moves = std::to_string(xMoves) + " (X) | (O) " + std::to_string(oMoves);

        // Vẽ dòng (Thẳng tắp)
        SetBgRGB(97, 100, 151);
        DrawFixedLine(BOX_X + 4, BOX_Y + 4, "Name:", names, BOX_WIDTH);
        DrawFixedLine(BOX_X + 4, BOX_Y + 6, "Score:", ss_score.str(), BOX_WIDTH);
        DrawFixedLine(BOX_X + 4, BOX_Y + 8, "Mode:", "2 Players", BOX_WIDTH);
        DrawFixedLine(BOX_X + 4, BOX_Y + 10, "Moves:", moves, BOX_WIDTH);
    }

   
    // B. VẼ NÚT BẤM 
    
    SetBgRGB(135, 236, 255); // Luôn giữ nền tím

    int centerX = BOX_X + (BOX_WIDTH / 2);
    int ySave = BOX_Y + 13; // Save nằm dòng 13
    int yPlay = BOX_Y + 15; // Play/Back nằm dòng 15 (Sát đáy)

    // 1. Nút SAVE (Giữa)
    GotoXY(centerX - 4, ySave);
    if (selectedOption == 0) { SetColorRGB(255, 255, 0); std::cout << "☼ Save ☼"; }
    else { SetColorRGB(0, 0, 0); std::cout << "  Save  "; }

    // 2. Nút PLAY AGAIN (Trái)
    GotoXY(BOX_X + 4, yPlay);
    if (selectedOption == 1) { SetColorRGB(255, 255, 0); std::cout << "☼ Play again ☼"; }
    else { SetColorRGB(0, 0, 0); std::cout << "  Play again  "; }

    // 3. Nút BACK (Phải)
    GotoXY(BOX_X + BOX_WIDTH - 16, yPlay);
    if (selectedOption == 2) { SetColorRGB(255, 255, 0); std::cout << "☼ Back ☼"; }
    else { SetColorRGB(0, 0, 0); std::cout << "  Back  "; }

    SetColorRGB(0, 0, 0); // Reset
}
void DrawPauseMenuItem(int index, bool is_selected) {
    if (index < 0 || index >= TOTAL_PAUSE_ITEMS) return;

    // --- CẤU HÌNH TỌA ĐỘ (CHỈNH SỬA Ở ĐÂY) ---
    // Giả sử Pause Menu nằm giữa màn hình.
    // Bạn điền tọa độ X sao cho chữ nằm giữa (VD: màn hình 165 thì X khoảng 76-78)
    int POS_X[] = { 76, 76, 76, 76, 76 };

    // Tọa độ Y: Cách nhau 3 dòng cho thoáng
    int POS_Y[] = { 20, 23, 26, 29, 32 };

    int x = POS_X[index];
    int y = POS_Y[index];

    std::string text = std::string(PAUSE_ITEMS[index]);
    std::string displayStr;

    // --- COPY ICON VÀ LOGIC TỪ MENU CHÍNH ---
    char icon = char(15); // Dấu ☼ 

    if (is_selected) {
        // Thêm icon và khoảng trắng khi chọn
        displayStr = std::string(" ") + icon + " " + text + " " + icon + " ";
    }
    else {
        // Thêm khoảng trắng hai bên để xóa icon cũ khi bỏ chọn
        displayStr = "   " + text + "   ";
    }

    // --- TÍNH TOÁN CĂN GIỮA (Optional) ---
    // Nếu bạn muốn X là tâm điểm, hãy dùng dòng này. 
    // Còn nếu muốn X là điểm bắt đầu in thì xóa dòng này đi.
    // int printX = x - (displayStr.length() / 2); 
    // GotoXY(printX, y);

    // Ở đây mình giữ nguyên cách dùng của bạn ở Menu chính (X là điểm bắt đầu)
    GotoXY(x, y);

    // Set màu nền ĐỎ (Giống Menu chính)
    SetBgRGB(218, 66, 76);

    if (is_selected) {
        SetColorRGB(255, 255, 255); // Trắng
        std::cout << displayStr;
    }
    else {
        SetColorRGB(255, 255, 0); // Vàng
        std::cout << displayStr;
    }

    // Reset màu về mặc định
    SetBgRGB(0, 0, 0);
    SetColorRGB(255, 255, 255);
}
void DrawFullPauseMenu(int selected_index) {
    
    ClearScreenWithColor(97, 100, 151);
    DrawLargebackground(0, 0);
    DrawGamepaused(13, 2);
    DrawFramepausemenu(66, 16);
    // --- 3. VẼ DANH SÁCH MENU ---
    for (int i = 0; i < TOTAL_PAUSE_ITEMS; i++) {
        DrawPauseMenuItem(i, i == selected_index); 
    }

    
}
// 10. WIN effect
void ShowChristmasResult_ASCII(int winner) {
    // 1. Xóa bộ đệm phím
    while (_kbhit()) _getch();

    // 2. Cấu hình màu Giáng Sinh
    int xmasColors[3][3] = {
        {255, 82, 82},   // ĐỎ
        {0, 230, 118},   // XANH
        {255, 215, 0}    // VÀNG
    };
    // Màu cho trận Hòa
    int drawColors[3][3] = { {255, 255, 255}, {176, 224, 230}, {192, 192, 192} };

    // 3. Chọn hình để vẽ
    const vector<string>* textToDraw;
    if (winner == 1) textToDraw = &ASCII_X_WIN;
    else if (winner == 2) textToDraw = &ASCII_O_WIN;
    else textToDraw = &ASCII_DRAW;

    // 4. Tính tọa độ giữa màn hình
    // X WIN / O WIN rộng khoảng 35 ký tự, DRAW rộng 40
    int textWidth = (winner == 0) ? 40 : 35;
    int startX = (165 - textWidth) / 2; // Giả sử màn hình rộng 120
    int startY = 25;

    int colorIdx = 0;

    // 5. VÒNG LẶP HIỆU ỨNG
    while (true) {
        // Kiểm tra phím SKIP
        if (_kbhit()) {
            int k = _getch();
            break;
        }

        // Chọn màu
        int r, g, b;
        if (winner == 0) {
            r = drawColors[colorIdx][0]; g = drawColors[colorIdx][1]; b = drawColors[colorIdx][2];
        }
        else {
            r = xmasColors[colorIdx][0]; g = xmasColors[colorIdx][1]; b = xmasColors[colorIdx][2];
        }

        SetColorRGB(r, g, b); // Set màu chữ
        SetBgRGB(97, 100, 151);

        // VẼ HÌNH
        for (int i = 0; i < textToDraw->size(); i++) {
            GotoXY(startX, startY + i);
            cout << (*textToDraw)[i]; // In trực tiếp chuỗi chứa ký tự khối
        }

        // Dòng Press Any Key
        GotoXY(startX - 2, startY + 8);
        cout << "   - PRESS ANY KEY TO CONTINUE -   ";

        // Đổi màu
        colorIdx = (colorIdx + 1) % 3;
        Sleep(200);
    }

    // Xóa bộ đệm lần cuối
    while (_kbhit()) _getch();
}
//
void PrintCenteredText(std::string text, int boxWidth) {
    if (text.length() > boxWidth) {
        text = text.substr(0, boxWidth); 
    }

    int left = (boxWidth - text.length()) / 2;
    int right = boxWidth - text.length() - left;

    // In khoảng trắng trái
    for (int i = 0; i < left; i++) std::cout << " ";

    // In chữ
    std::cout << text;

    // In khoảng trắng phải
    for (int i = 0; i < right; i++) std::cout << " ";
}
void DrawTimerOnly() {
    // 1. Tính toán giờ
    int min = _turnTimer / 60;
    int sec = _turnTimer % 60;
    char runningTime[30];
    sprintf_s(runningTime, "%02d:%02d", min, sec);

    // 2. Tọa độ (Copy từ hàm cũ của bạn ra)
    const int p1_box_x = 14, p1_box_y = 5;
    const int p2_box_x = 133, p2_box_y = 5;

    // 3. Chỉ nhảy đến đúng chỗ cần vẽ số để in đè lên số cũ
    if (_currentPlayer == 1) {
        GotoXY(p1_box_x + 9, p1_box_y + 9);
        SetColorRGB(0, 0, 0); // Đảm bảo màu đen (hoặc màu chữ bạn muốn)
        SetBgRGB(135, 236, 255); // Đảm bảo trùng màu nền background
        std::cout << runningTime;
    }
    else {
        GotoXY(p2_box_x + 10, p2_box_y + 9);
        SetColorRGB(0, 0, 0);
        SetBgRGB(135, 236, 255);
        std::cout << runningTime;
    }

    // Reset màu để không ảnh hưởng cái khác
    SetColorRGB(0, 0, 0);
}