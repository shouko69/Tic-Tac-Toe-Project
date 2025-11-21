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

using namespace std;

// ==========================================================================
// 1. GLOBAL VARIABLES & ASSETS (TỪ VIEW 1)
// ==========================================================================
const char* MENU_ITEMS[] = { "NEW GAME", "LOAD GAME", "SETTING", "GUIDE", "ABOUT", "EXIT" };
const char* PAUSE_ITEMS[] = { "RESUME", "RESTART", "SAVE", "SETTINGS", "QUIT" };
const char* NEW_GAME_OPTIONS[] = { "1 PLAYER", "2 PLAYERS", "BACK" };

const int TOTAL_ITEMS = sizeof(MENU_ITEMS) / sizeof(MENU_ITEMS[0]);
const int TOTAL_NEW_GAME_OPTIONS = 3;
const int TOTAL_PAUSE_ITEMS = sizeof(PAUSE_ITEMS) / sizeof(PAUSE_ITEMS[0]);

const int START_Y = 15;
const int PAUSE_START_Y = 9;
const int FRAME_START_X = 99;
const int FRAME_WIDTH = 28;
// ==========================================================================
// 2. Màu sắc Theme (Lavender)
// ==========================================================================

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
// ==========================================================================
// 3. Assets cho Game Over (Giữ nguyên từ View 1)
// ==========================================================================
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
    "██╗       ██████╗ ███████╗███████╗",
    "██║      ██╔═══██╗██╔════╝██╔════╝",
    "██║      ██║   ██║███████╗█████╗  ",
    "██║      ██║   ██║╚════██║██╔══╝  ",
    "███████╗ ╚██████╔╝███████║███████╗ ",
    "╚══════╝  ╚═════╝ ╚══════╝╚══════╝ "
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

_POINT _A[BOARD_SIZE][BOARD_SIZE];
bool _TURN = true;
struct ColorTemp {
    int r, g, b;
    bool operator==(const ColorTemp& o) const { return r == o.r && g == o.g && b == o.b; }
};

// ==========================================================================
// 4. SYSTEM & HELPER FUNCTIONS
// ==========================================================================
void GotoXY(int x, int y) {
    COORD coord = { (SHORT)x, (SHORT)y };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

int CenterX(const std::string& text) {
    int x = (CONSOLE_WIDTH - (int)text.length()) / 2;
    return (x > 0) ? x : 0;
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

// ==========================================================================
// 5. START INTRO (LẤY TỪ VIEW 2 - THEO YÊU CẦU)
// ==========================================================================
void StartIntro() {
    // 1. "Sơn" nền Lavender
    ClearScreenWithColor(202, 196, 248);
    SetBgRGB(202, 196, 248); SetColorRGB(0, 0, 0);

    // 2. Vẽ Logo (Mô phỏng logo từ View 2)
    const char* LOGO_LINE = "CARO LEGENDS";
    int logoX = CenterX(LOGO_LINE);
    SetColorRGB(100, 100, 255); // Khung Tím/Xanh
    GotoXY(logoX - 2, 2); cout << "╔" << string(strlen(LOGO_LINE) + 2, '═') << "╗";
    GotoXY(logoX - 2, 3); cout << "║";
    GotoXY(logoX, 3); SetColorRGB(0, 0, 0); cout << LOGO_LINE;
    GotoXY(logoX + strlen(LOGO_LINE), 3); SetColorRGB(100, 100, 255); cout << " ║";
    GotoXY(logoX - 2, 4); cout << "╚" << string(strlen(LOGO_LINE) + 2, '═') << "╝";

    // 3. Loading Bar
    int barWidth = 50;
    int barY = 20;
    int barX = CenterX(string(barWidth + 2, ' '));

    SetColorRGB(80, 80, 80);
    GotoXY(CenterX("LOADING..."), barY - 2); cout << "LOADING...";
    GotoXY(barX, barY); cout << "[" << string(barWidth, '.') << "]";

    for (int i = 0; i <= barWidth; ++i) {
        int percent = (int)(((float)i / barWidth) * 100);
        SetBgRGB(100, 100, 255); // Nền xanh
        GotoXY(barX + 1 + i, barY); cout << " "; // Block
        SetBgRGB(202, 196, 248); SetColorRGB(80, 80, 80);
        GotoXY(barX + barWidth + 3, barY); cout << percent << "%";
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    // 4. Press Any Key
    SetBgRGB(202, 196, 248);
    GotoXY(CenterX("LOADING..."), barY - 2); cout << "          ";
    GotoXY(barX, barY); cout << string(barWidth + 10, ' '); // Xóa bar

    const char* prompt = "PRESS ANY KEY TO START";
    int promptX = CenterX(prompt);
    bool showText = true;

    while (true) {
        if (_kbhit()) { _getch(); break; }
        if (showText) { SetColorRGB(80, 80, 80); GotoXY(promptX, barY); cout << prompt; }
        else { SetBgRGB(202, 196, 248); GotoXY(promptX, barY); cout << string(strlen(prompt), ' '); }
        showText = !showText;
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
    ResetColor();
}

// ==========================================================================
// 6. MENU & GAME FLOW (LẤY TỪ VIEW 1)
// ==========================================================================
void StartAbout() {
    SetColorRGB(255, 255, 255);
    system("cls");
    int i = 55, offset = 1;
    GabageCollect(); // nếu đây là hàm dọn rác custom của huynh thì giữ

    // ✅ 1. Chuyển sang UTF16 để in Unicode (phải trước cout/wcout)
    _setmode(_fileno(stdout), _O_U16TEXT);

    GotoXY(i, 1);
    wcout << L" █████╗ ██████╗  ██████╗ ██╗   ██╗████████╗\n"; GotoXY(i, 2);
    wcout << L"██╔══██╗██╔══██╗██╔═══██╗██║   ██║╚══██╔══╝\n"; GotoXY(i, 3);
    wcout << L"███████║██████╔╝██║   ██║██║   ██║   ██║   \n"; GotoXY(i, 4);
    wcout << L"██╔══██║██╔══██╗██║   ██║██║   ██║   ██║   \n"; GotoXY(i, 5);
    wcout << L"██║  ██║██████╔╝╚██████╔╝╚██████╔╝   ██║   \n"; GotoXY(i, 6);
    wcout << L"╚═╝  ╚═╝╚═════╝  ╚═════╝  ╚═════╝    ╚═╝   \n";

    COORD tableCoord{ 40, 10 };
    i = 0;
    GotoXY(tableCoord.X + 5, tableCoord.Y);
    wcout << L"Welcome to the Gomoku Game - made by team 6 - 25CTT3 - HCMUS\n";

    i += 2;
    GotoXY(tableCoord.X, tableCoord.Y + i);
    wcout << L"25120215 - Nguyễn Minh Nhật \t\t PvE Mode - Game Content\n";
    i += offset;
    GotoXY(tableCoord.X, tableCoord.Y + i + 2);
    wcout << L"25120215 - Nguyễn Hoàng Huy\t\t Game Interface Design\n";
    i += offset;
    GotoXY(tableCoord.X, tableCoord.Y + i + 2);
    wcout << L"25120215 - Vũ Thanh Phong\t\t Game Graphic Design - Game Flow Design\n";
    i += offset;
    GotoXY(tableCoord.X, tableCoord.Y + i + 2);
    wcout << L"25120215 - Đỗ Lê Nhật Quang\t\t Game Interface Design - Game Sound\n";
    i += offset;
    GotoXY(tableCoord.X, tableCoord.Y + i + 2);
    wcout << L"25120215 - Nguyễn Phú Quang\t\t Load & Save Game\n";
    i += offset;
    GotoXY(tableCoord.X, tableCoord.Y + i + 2);
    wcout << L"25120215 - Nguyễn Vũ Nhật Quang\t\t Game Graphic Design\n";

    // ✅ 3. Vòng chờ phím bấm
    while (true) {
        if (_kbhit()) {
            int key = _getch();
            if (key == 27 || key == 13) break; // ESC hoặc Enter
        }
        Sleep(50);
    }

    // ✅ 4. Trả chế độ xuất về bình thường (O_TEXT)
    _setmode(_fileno(stdout), _O_TEXT);

    // ✅ 5. Không cần _getch() nữa (vì trên đã chờ phím)
    ClearScreenWithColor(0, 0, 0);
    ResetColor();
    // SceneHandle("MAIN MENU"); // nếu huynh có hàm menu chính
    currentState = MENU;
}

void DrawFancyBox(int x, int y, int width, std::string text, bool isSelected) {
    // 1. Thiết lập màu nền chung
    SetBgRGB(BG_PURPLE_R, BG_PURPLE_G, BG_PURPLE_B); // Nền hộp
    SetColorRGB(BORDER_R, BORDER_G, BORDER_B);

    // --- VẼ VIỀN TRÊN ---
    GotoXY(x, y);
    std::cout << "╔";
    for (int i = 0; i < width; i++) std::cout << "═";
    std::cout << "╗";

    // --- VẼ THÂN GIỮA & CHỮ ---
    GotoXY(x, y + 1);
    std::cout << "║"; // Viền trái

    // Tính toán khoảng trắng để căn giữa chữ trong hộp
    int textLen = text.length();
    int paddingLeft = (width - textLen) / 2;
    int paddingRight = width - textLen - paddingLeft;

    // In khoảng trắng bên trái
    for (int i = 0; i < paddingLeft; i++) std::cout << " ";

    // IN CHỮ (Đổi màu nếu được chọn)
    if (isSelected) {
        SetColorRGB(TEXT_SELECT_R, TEXT_SELECT_G, TEXT_SELECT_B); // Đỏ
    }
    else {
        SetColorRGB(TEXT_NORMAL_R, TEXT_NORMAL_G, TEXT_NORMAL_B); // Đen xanh
    }
    std::cout << text;

    // Trả lại màu viền đen để vẽ tiếp
    SetColorRGB(BORDER_R, BORDER_G, BORDER_B);

    // In khoảng trắng bên phải
    for (int i = 0; i < paddingRight; i++) std::cout << " ";

    std::cout << "║"; // Viền phải

    // --- VẼ VIỀN DƯỚI ---
    GotoXY(x, y + 2);
    std::cout << "╚";
    for (int i = 0; i < width; i++) std::cout << "═";
    std::cout << "╝";
}

void DrawMenuItem(int index, bool is_selected) {
    if (index < 0 || index >= TOTAL_ITEMS) return;

    std::string menuText = std::string(MENU_ITEMS[index]);

    // Chiều rộng cố định cho cái hộp (để tất cả nút bằng nhau)
    const int BOX_WIDTH = 30;

    // Tính tọa độ X để cái hộp nằm giữa màn hình
    int x = CenterX(std::string(BOX_WIDTH + 2, ' ')); // +2 vì tính cả viền

    // Tính tọa độ Y. Mỗi hộp cao 3 dòng, cộng thêm 1 dòng khoảng cách = 4
    int y = START_Y + (index * 3);

    // Gọi hàm vẽ hộp
    DrawFancyBox(x, y, BOX_WIDTH, menuText, is_selected);
}

void DrawFullMenu(int selected_index) {
    ClearScreenWithColor(BG_PURPLE_R, BG_PURPLE_G, BG_PURPLE_B); // "Sơn" nền Lavender

    int i = 33;
    GotoXY(i, 1);
    SetColorRGB(LOGO_R, LOGO_G, LOGO_B);

    // XÓA BỎ: _setmode(_fileno(stdout), _O_U16TEXT);

    // SỬA LẠI: Dùng std::cout
    std::cout << "████████╗██╗  ██╗███████╗       █████╗  █████╗ ██████╗  ██████╗     ██████╗  █████╗ ███╗   ███╗███████╗\n"; GotoXY(i, 2);
    std::cout << "╚══██╔══╝██║  ██║██╔════╝     ██╔════╝██╔══██╗██╔══██╗██╔═══██╗    ██╔════╝ ██╔══██╗████╗ ████║██╔════╝\n"; GotoXY(i, 3);
    std::cout << "   ██║   ███████║█████╗       ██║     ███████║██████╔╝██║   ██║    ██║  ███╗███████║██╔████╔██║█████╗  \n"; GotoXY(i, 4);
    std::cout << "   ██║   ██╔══██║██╔══╝       ██║     ██╔══██║██╔══██╗██║   ██║    ██║   ██║██╔══██║██║╚██╔╝██║██╔══╝  \n"; GotoXY(i, 5);
    std::cout << "   ██║   ██║  ██║███████╗     ╚██████╗██║  ██║██║  ██║╚██████╔╝    ╚██████╔╝██║  ██║██║ ╚═╝ ██║███████╗\n"; GotoXY(i, 6);
    std::cout << "   ╚═╝   ╚═╝  ╚═╝╚══════╝      ╚═════╝╚═╝  ╚═╝╚═╝  ╚═╝ ╚═════╝      ╚═════╝ ╚═╝  ╚═╝╚═╝     ╚═╝╚══════╝\n";

    // XÓA BỎ: _setmode(_fileno(stdout), _O_TEXT);
    SetBgRGB(BG_PURPLE_R, BG_PURPLE_G, BG_PURPLE_B); // Nền hướng dẫn
    SetColorRGB(TEXT_NORMAL_R, TEXT_NORMAL_G, TEXT_NORMAL_B);

    for (int i = 0; i < TOTAL_ITEMS; i++) {
        DrawMenuItem(i, i == selected_index);
    }
}

/*
 * Chức năng: Vẽ một mục menu riêng lẻ của màn hình New Game.
 */
void DrawNewGameMenuItem(int index, bool is_selected) {
    GotoXY(CenterX(NEW_GAME_OPTIONS[index]), 15 + index * 2);
    if (is_selected) {
        std::cout << ">> " << NEW_GAME_OPTIONS[index] << " <<";
    }
    else {
        std::cout << "   " << NEW_GAME_OPTIONS[index] << "   ";
    }
}

/*
 * Chức năng: Vẽ toàn bộ màn hình New Game lần đầu tiên.
 */
void DrawFullNewGameMenu(int selected_index) {
    // 1. Xóa màn hình và vẽ nền
    ClearScreenWithColor(89, 79, 175); // Dùng màu nền của bạn



    // 3. Vẽ tất cả các mục menu
    for (int i = 0; i < TOTAL_NEW_GAME_OPTIONS; ++i) {
        DrawNewGameMenuItem(i, (i == selected_index));
    }
}

// ==========================================================================
// 7. PLAYER NAME INPUT (LẤY TỪ VIEW 1 - FULL)
// ==========================================================================

/*
 * Chức năng: Vẽ toàn bộ màn hình nhập tên LẦN ĐẦU TIÊN.
 * Hàm này có ClearScreen và chỉ được gọi một lần.
 */
void DrawFullPlayerNameScreen() {
    ClearScreenWithColor(89, 79, 175);
    SetColorRGB(255, 100, 180);
    int i = 50;
    GotoXY(i, 1);
    _setmode(_fileno(stdout), _O_U16TEXT);
    std::wcout << L"██████╗     ██████╗ ██╗      █████╗ ██╗   ██╗███████╗██████╗ ███████╗ \n"; GotoXY(i, 2);
    std::wcout << L"╚════██╗    ██╔══██╗██║     ██╔══██╗╚██╗ ██╔╝██╔════╝██╔══██╗██╔════╝ \n"; GotoXY(i, 3);
    std::wcout << L" █████╔╝    ██████╔╝██║     ███████║ ╚████╔╝ █████╗  ██████╔╝███████╗ \n"; GotoXY(i, 4);
    std::wcout << L"██╔═══╝     ██╔═══╝ ██║     ██╔══██║  ╚██╔╝  ██╔══╝  ██╔══██╗╚════██║ \n"; GotoXY(i, 5);
    std::wcout << L"███████╗    ██║     ███████╗██║  ██║   ██║   ███████╗██║  ██║███████║ \n"; GotoXY(i, 6);
    std::wcout << L"╚══════╝    ╚═╝     ╚══════╝╚═╝  ╚═╝   ╚═╝   ╚══════╝╚═╝  ╚═╝╚══════╝ \n";
    _setmode(_fileno(stdout), _O_TEXT); // Trả về chế độ thường

    GotoXY(50, 23); std::cout << ">> Player's Name1 <<";
    GotoXY(100, 23); std::cout << ">> Player's Name2 <<";
    GotoXY(50, 24); std::cout << "Less than 20 characters";
    GotoXY(100, 24); std::cout << "Less than 20 characters";
    GotoXY(CenterX("Press ENTER to confirm and proceed, TAB to switch.") + 4, 40);
    std::cout << "Press ENTER to confirm and proceed, TAB to switch.";
    GotoXY(CenterX("Press ESC to exit without saving.") + 4, 41);
    std::cout << "Press ESC to exit without saving.";
}

/*
 * Chức năng: CHỈ CẬP NHẬT các phần động của màn hình (tên, con trỏ, nút BACK).
 * Hàm này KHÔNG có ClearScreen, không gây nhấp nháy.
 */
void UpdatePlayerNameScreen(int activeControl, const char* p1_buffer, const char* p2_buffer) {
    // activeControl: 0=P1_Typing, 1=P2_Typing, 2=Global_Back_Button

    // Xóa vùng tên cũ để cập nhật
    GotoXY(50, 26); std::cout << "                         ";
    GotoXY(100, 26); std::cout << "                         ";

    // Vẽ tên P1 và con trỏ nếu đang được chọn
    GotoXY(50, 26); std::cout << p1_buffer;
    if (activeControl == 0) {
        std::cout << "_";
    }

    // Vẽ tên P2 và con trỏ nếu đang được chọn
    GotoXY(100, 26); std::cout << p2_buffer;
    if (activeControl == 1) {
        std::cout << "_";
    }

    // 3. Vẽ nút BACK toàn cục và tô sáng nếu đang được chọn
    if (activeControl == 2) {
        GotoXY(CenterX("[ BACK ]") + 4, 33);
        std::cout << "[ BACK ]";
    }
    else {
        GotoXY(CenterX("  BACK  ") + 4, 33);
        std::cout << "  BACK  ";
    }
}

// ==========================================================================
// 8. IN-GAME LOGIC (LẤY TỪ VIEW 1 - BÀN CỜ TO)
// ==========================================================================
void DrawBoard(int pSize) {

    SetColorRGB(0, 0, 0);
    SetBgRGB(89, 79, 175);

    for (int j = 0; j <= pSize; j++) {
        for (int i = 0; i <= pSize; i++) {

            // 1. Dùng biến toàn cục từ view.h để tính tọa độ
            int x = LEFT + CELL_VISUAL_WIDTH * i;   // Thay CELL_WIDTH bằng CELL_VISUAL_WIDTH
            int y = TOP + CELL_VISUAL_HEIGHT * j;   // Thay CELL_HEIGHT bằng CELL_VISUAL_HEIGHT
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
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (_A[i][j].c != 0) {
                // Gọi hàm vẽ Pixel Art thay vì cout
                if (_A[i][j].c == -1) {
                    DrawXmini(j, i);
                }
                else {
                    DrawOmini(j, i);
                }
            }
        }
    }
}
// Hàm này chuyên dùng để nhảy đến TÂM của một ô trên bàn cờ
void GotoBoard(int pX, int pY) {
    // --- CÔNG THỨC CĂN GIỮA TỰ ĐỘNG ---

    // 1. Tính toạ độ góc trái trên của ô
    int startX = LEFT + pX * CELL_VISUAL_WIDTH;
    int startY = TOP + pY * CELL_VISUAL_HEIGHT;

    // 2. Cộng thêm một nửa kích thước để vào tâm
    // (CELL_VISUAL_WIDTH / 2) : Vào giữa chiều ngang
    // (CELL_VISUAL_HEIGHT / 2) : Vào giữa chiều dọc

    // Lưu ý: Nếu chiều rộng là số chẵn (ví dụ 6), tâm sẽ lệch phải 1 chút.
    // Ta trừ nhẹ đi 1 (nếu muốn) hoặc để nguyên công thức toán học thuần túy:

    int screenX = startX + (CELL_VISUAL_WIDTH / 2);
    // Nếu thấy nó lệch phải 1 nấc thì dùng dòng dưới:
    // int screenX = startX + (CELL_VISUAL_WIDTH / 2) - 1;

    int screenY = startY + (CELL_VISUAL_HEIGHT / 2);

    GotoXY(screenX, screenY);
}
/*
 * Chức năng: Vẽ các thành phần TĨNH của giao diện 2 người chơi MỘT LẦN.
 * Ví dụ: tên người chơi, chữ "WINS:", "MOVES:".
 */
void DrawStatic2P_UI() {
    const int p1_box_x = 10, p1_box_y = 5;  // Căn chỉnh lại tọa độ cho đẹp
    const int p2_box_x = 130, p2_box_y = 5;

    // Vẽ cho Player 1
    GotoXY(p1_box_x, p1_box_y);     std::cout << _player1_name << " (X)";
    GotoXY(p1_box_x, p1_box_y + 1); std::cout << "---------------";
    GotoXY(p1_box_x, p1_box_y + 2); std::cout << "WINS: ";
    GotoXY(p1_box_x, p1_box_y + 3); std::cout << "MOVES: ";

    // Vẽ cho Player 2
    GotoXY(p2_box_x, p2_box_y);     std::cout << _player2_name << " (O)";
    GotoXY(p2_box_x, p2_box_y + 1); std::cout << "---------------";
    GotoXY(p2_box_x, p2_box_y + 2); std::cout << "WINS: ";
    GotoXY(p2_box_x, p2_box_y + 3); std::cout << "MOVES: ";
}
/*
 * Chức năng: CHỈ CẬP NHẬT các giá trị ĐỘNG (số điểm, số lượt đi, ngôi sao).
 * Hàm này được gọi liên tục nhưng không gây nhấp nháy.
 */
 // hàm này sửa sau xài tạm ngôi sao
void UpdateDynamic2P_UI() {
    SetColorRGB(0, 0, 0);
    SetBgRGB(89, 79, 175);
    const int p1_box_x = 10, p1_box_y = 5;
    const int p2_box_x = 130, p2_box_y = 5;

    // 1. Cập nhật điểm số và lượt đi (in đè lên số cũ)
    // Dùng "  " để xóa số có 2 chữ số cũ trước khi in số mới
    GotoXY(p1_box_x + 7, p1_box_y + 2); std::cout << _player1_score << "  ";
    GotoXY(p1_box_x + 7, p1_box_y + 3); std::cout << (_moveCount + 1) / 2 << "  ";

    GotoXY(p2_box_x + 7, p2_box_y + 2); std::cout << _player2_score << "  ";
    GotoXY(p2_box_x + 7, p2_box_y + 3); std::cout << _moveCount / 2 << "  ";

    // 2. Cập nhật ngôi sao báo lượt
    // (Đây là ví dụ, bạn có thể dùng lại code ngôi sao vẽ tay của mình)
    GotoXY(p1_box_x - 3, p1_box_y); std::cout << " "; // Xóa ngôi sao cũ
    GotoXY(p2_box_x - 3, p2_box_y); std::cout << " "; // Xóa ngôi sao cũ
    if (_currentPlayer == 1) {
        GotoXY(p1_box_x - 3, p1_box_y); std::cout << "*";
    }
    else {
        GotoXY(p2_box_x - 3, p2_box_y); std::cout << "*";
    }
}
void DrawGameUI() {
    ClearScreenWithColor(89, 79, 175);

    DrawBoard(BOARD_SIZE); // Ve man hinh game
    // 2. Vẽ các thông tin TĨNH (Tên, khung, chữ WINS, MOVES...)
    // Bắt buộc phải có, nếu không Resume xong chỉ thấy mỗi số điểm bay lơ lửng
    SetColorRGB(0, 0, 0); // Chọn màu chữ (ví dụ đen)
    DrawStatic2P_UI();

    // 3. Vẽ các thông tin ĐỘNG hiện tại (Điểm số, lượt đi đang dở dang)
    // Để khi Resume, nó hiện đúng điểm số cũ chứ không phải số 0
    UpdateDynamic2P_UI();

    // 4. Vẽ trang trí (nếu có dùng ảnh gạch, mario...)
    DrawBrickp(0, 47);
}

// ==========================================================================
// 9. GAME OVER SCREEN (LẤY TỪ VIEW 1 - BẢN XỊN NHẤT)
// ==========================================================================
void DrawFixedLine(int startX, int y, std::string label, std::string value, int width) {
    GotoXY(startX, y);
    SetBgRGB(89, 79, 175); // NỀN TÍM
    SetColorRGB(0, 0, 0);  // CHỮ ĐEN

    // 1. Vẽ viền trái
    std::cout << "| ";

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
    int padding = width - 2 - printedLen; // width - 2 viền
    if (padding < 0) padding = 0;
    std::cout << std::string(padding, ' ');

    // 6. Vẽ viền phải
    std::cout << "|";
}
void DrawGameOverScreen(int selectedOption, bool drawAll) {
    // --- THÔNG SỐ KHUNG ---
    const int BOX_WIDTH = 50;
    const int BOX_HEIGHT = 17; // Cao 17 dòng để chứa nút bấm thoải mái

    // Tự động căn giữa màn hình
    const int BOX_X = (CONSOLE_WIDTH - BOX_WIDTH) / 2;
    const int BOX_Y = 14;

    // Tọa độ vẽ ASCII ART (X cách xa khung ra một chút)
    const int ART_Y = BOX_Y + 4;
    const int P1_ART_X = BOX_X - 42;      // Bên trái
    const int P2_ART_X = BOX_X + BOX_WIDTH + 2; // Bên phải

    // =============================================================
    // A. VẼ NỀN, TIÊU ĐỀ, KHUNG & ART (CHỈ CHẠY 1 LẦN)
    // =============================================================
    if (drawAll) {
        ClearScreenWithColor(89, 79, 175); // Xóa màn hình màu tím

        // Vẽ gạch nền (nếu có hàm này)
        DrawBrickp(0, 47);

        // Tiêu đề
        SetBgRGB(89, 79, 175);
        SetColorRGB(255, 255, 0); // Vàng
        GotoXY((CONSOLE_WIDTH - 12) / 2, 10); // Căn giữa chữ "Game results"
        std::cout << "Game results";

        // Hướng dẫn phím bấm
        SetColorRGB(255, 255, 255);
        std::string guide = "'Tab': Move   'Enter': Select   'ESC': Back";
        GotoXY((CONSOLE_WIDTH - guide.length()) / 2, 45);
        std::cout << guide;

        // --- XỬ LÝ LOGIC THẮNG/THUA ---
        std::string headerMsg;
        int winner = 0;
        if (_gameWinner == 1) { winner = 1; headerMsg = "Player " + std::string(_player1_name) + " (X) won!"; }
        else if (_gameWinner == 2) { winner = 2; headerMsg = "Player " + std::string(_player2_name) + " (O) won!"; }
        else { headerMsg = "It's a Draw!"; }

        // --- VẼ ASCII ART (X WIN / O LOSE) ---
        if (winner != 0) {
            SetBgRGB(89, 79, 175); // Nền tím

            const std::vector<std::string>* p1_Char;   // Hình X
            const std::vector<std::string>* p1_Result; // Hình WIN/LOSE
            const std::vector<std::string>* p2_Char;   // Hình O
            const std::vector<std::string>* p2_Result; // Hình WIN/LOSE

            int colorP1, colorP2;

            if (winner == 1) { // X THẮNG
                p1_Char = &ART_X_BIG; p1_Result = &WIN_ART;  colorP1 = 0xFFD700; // Vàng
                p2_Char = &ART_O_BIG; p2_Result = &LOSE_ART; colorP2 = 0x696969; // Xám
            }
            else { // O THẮNG
                p1_Char = &ART_X_BIG; p1_Result = &LOSE_ART; colorP1 = 0x696969; // Xám
                p2_Char = &ART_O_BIG; p2_Result = &WIN_ART;  colorP2 = 0xFFD700; // Vàng
            }

            // --- VẼ BÊN TRÁI (P1) ---
            SetColorRGB((colorP1 >> 16) & 0xFF, (colorP1 >> 8) & 0xFF, colorP1 & 0xFF);

            for (int i = 0; i < ART_HEIGHT; ++i) {
                // 1. Vẽ chữ X
                GotoXY(P1_ART_X, ART_Y + i);
                std::cout << (*p1_Char)[i];

                // 2. Vẽ WIN/LOSE (Cách lề trái P1 đúng 12 ô)
                // Dùng GotoXY để tách rời 2 chữ ra, không cho nó dính vào nhau
                GotoXY(P1_ART_X + 12, ART_Y + i);
                std::cout << (*p1_Result)[i];
            }

            // --- VẼ BÊN PHẢI (P2) ---
            SetColorRGB((colorP2 >> 16) & 0xFF, (colorP2 >> 8) & 0xFF, colorP2 & 0xFF);

            for (int i = 0; i < ART_HEIGHT; ++i) {
                // 1. Vẽ O
                GotoXY(P2_ART_X, ART_Y + i);
                std::cout << (*p2_Char)[i];

                // 2. Vẽ WIN/LOSE (Cách lề trái P2 đúng 12 ô)
                GotoXY(P2_ART_X + 12, ART_Y + i);
                std::cout << (*p2_Result)[i];
            }
        }

        // --- VẼ HỘP KẾT QUẢ (NÉT ĐỨT + NỀN TÍM) ---
        SetBgRGB(89, 79, 175);
        SetColorRGB(0, 0, 0); // Viền đen

        // Nắp hộp
        GotoXY(BOX_X, BOX_Y);
        std::cout << "+" << std::string(BOX_WIDTH - 2, '-') << "+";

        // Thân hộp
        for (int i = 1; i < BOX_HEIGHT; i++) {
            GotoXY(BOX_X, BOX_Y + i);
            std::cout << "|" << std::string(BOX_WIDTH - 2, ' ') << "|";
        }

        // Đáy hộp
        GotoXY(BOX_X, BOX_Y + BOX_HEIGHT);
        std::cout << "+" << std::string(BOX_WIDTH - 2, '-') << "+";

        // --- ĐIỀN NỘI DUNG ---
        GotoXY(BOX_X + 1, BOX_Y + 2);
        int padHead = (BOX_WIDTH - 2 - headerMsg.length()) / 2;
        if (padHead < 0) padHead = 0;
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
        DrawFixedLine(BOX_X, BOX_Y + 4, "Name:", names, BOX_WIDTH);
        DrawFixedLine(BOX_X, BOX_Y + 6, "Score:", ss_score.str(), BOX_WIDTH);
        DrawFixedLine(BOX_X, BOX_Y + 8, "Mode:", "2 Players", BOX_WIDTH);
        DrawFixedLine(BOX_X, BOX_Y + 10, "Moves:", moves, BOX_WIDTH);
    }

    // =============================================================
    // B. VẼ NÚT BẤM (LUÔN CHẠY - ANTI GHOSTING)
    // =============================================================
    SetBgRGB(89, 79, 175); // Luôn giữ nền tím

    int centerX = BOX_X + (BOX_WIDTH / 2);
    int ySave = BOX_Y + 13; // Save nằm dòng 13
    int yPlay = BOX_Y + 15; // Play/Back nằm dòng 15 (Sát đáy)

    // 1. Nút SAVE (Giữa)
    GotoXY(centerX - 4, ySave);
    if (selectedOption == 0) { SetColorRGB(255, 255, 255); std::cout << "[ Save ]"; }
    else { SetColorRGB(80, 80, 80); std::cout << "  Save  "; }

    // 2. Nút PLAY AGAIN (Trái)
    GotoXY(BOX_X + 4, yPlay);
    if (selectedOption == 1) { SetColorRGB(255, 255, 255); std::cout << "[ Play again ]"; }
    else { SetColorRGB(80, 80, 80); std::cout << "  Play again  "; }

    // 3. Nút BACK (Phải)
    GotoXY(BOX_X + BOX_WIDTH - 12, yPlay);
    if (selectedOption == 2) { SetColorRGB(255, 255, 255); std::cout << "[ Back ]"; }
    else { SetColorRGB(80, 80, 80); std::cout << "  Back  "; }

    SetColorRGB(0, 0, 0); // Reset
}
void DrawPauseMenuItem(int index, bool is_selected) {
    if (index < 0 || index >= TOTAL_PAUSE_ITEMS) return;

    // 1. Chỉ lấy text gốc
    std::string text = std::string(PAUSE_ITEMS[index]);

    // 2. Quyết định màu sắc
    if (is_selected) {
        SetColorRGB(255, 100, 180); // Màu chữ NỔI BẬT (Hồng)
    }
    else {
        SetColorRGB(80, 60, 120); // Màu chữ BÌNH THƯỜNG (Tím than)
    }

    // 3. Căn giữa và in
    int x = CenterX(text);
    int y = PAUSE_START_Y + index * 2;
    GotoXY(x, y);
    std::cout << text;
}
void DrawFullPauseMenu(int selected_index) {
    // 1. Đặt màu nền tím và xóa màn hình
    ClearScreenWithColor(255, 255, 255);

    // --- 2. VẼ TIÊU ĐỀ (Đã bỏ khung) ---
    const std::string title = "GAME PAUSED";
    SetColorRGB(0, 0, 0); // Chữ tiêu đề màu trắng
    GotoXY(CenterX(title), 4);
    std::cout << title;

    // --- 3. VẼ DANH SÁCH MENU ---
    for (int i = 0; i < TOTAL_PAUSE_ITEMS; i++) {
        DrawPauseMenuItem(i, i == selected_index); // Gọi hàm TrueColor mới
    }

    // --- 4. HƯỚNG DẪN ---
    SetColorRGB(80, 60, 120); // Chữ hướng dẫn màu tím than
    const char* guide = "'W': Move up, 'S': Move down,  'Enter': Select,  'ESC': Resume";
    GotoXY(CenterX(guide), PAUSE_START_Y + TOTAL_PAUSE_ITEMS * 2 + 3);
    std::cout << guide;
}
