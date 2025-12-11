#include "View.h"
#include "Model.h"
#include "Windows.h"
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <cwchar>
#include <sstream>
#include <fcntl.h>
#include <io.h>
#include <thread>
#include <chrono>
#include "truecolor_utils.h" 
#include <cstdio>

using namespace std;

const char* MENU_ITEMS[] = { "NEW GAME", "LOAD GAME", "SETTING", "GUIDE", "ABOUT", "EXIT" };
const char* PAUSE_ITEMS[] = { "RESUME", "RESTART", "SAVE", "GUIDE", "SETTINGS", "QUIT"};
const char* NEW_GAME_OPTIONS[] = { "1 PLAYER", "2 PLAYERS", "BACK" };
int ITEM_X[] = { 82, 82, 82, 82 };
int ITEM_Y[] = { 15, 18, 25, 28 };
const int TOTAL_ITEMS = sizeof(MENU_ITEMS) / sizeof(MENU_ITEMS[0]);
const int TOTAL_NEW_GAME_OPTIONS = 3;
const int TOTAL_PAUSE_ITEMS = sizeof(PAUSE_ITEMS) / sizeof(PAUSE_ITEMS[0]);
const int START_Y = 15;
const int PAUSE_START_Y = 9;
const int FRAME_START_X = 99;
const int FRAME_WIDTH = 28;

#define BG_PURPLE_R 202
#define BG_PURPLE_G 196
#define BG_PURPLE_B 248
#define TEXT_NORMAL_R 80
#define TEXT_NORMAL_G 60
#define TEXT_NORMAL_B 120
#define TEXT_SELECT_R 255
#define TEXT_SELECT_G 100
#define TEXT_SELECT_B 180
#define BORDER_R 100
#define BORDER_G 100
#define BORDER_B 200
#define LOGO_R 163
#define LOGO_G 87
#define LOGO_B 213

const int ART_HEIGHT = 6;
const std::vector<std::string> WIN_ART = {
    "██╗    ██╗██╗███╗   ██╗",
    "██║    ██║██║████╗  ██║",
    "██║ █╗ ██║██║██╔██╗ ██║",
    "██║███╗██║██║██║╚██╗██║",
    "╚███╔███╔╝██║██║ ╚████║",
    " ╚══╝╚══╝ ╚═╝╚═╝  ╚═══╝"
};
const std::vector<std::string> LOSE_ART = {
    "██╗      ██████╗ ███████╗███████╗",
    "██║     ██╔═══██╗██╔════╝██╔════╝",
    "██║     ██║   ██║███████╗█████╗  ",
    "██║     ██║   ██║╚════██║██╔══╝  ",
    "███████╗╚██████╔╝███████║███████╗ ",
    "╚══════╝ ╚═════╝ ╚══════╝╚══════╝ "
};
const std::vector<std::string> ART_X_BIG = {
    "██╗  ██╗",
    "╚██╗██╔╝",
    " ╚███╔╝ ",
    " ██╔██╗ ",
    "██╔╝ ██╗",
    "╚═╝  ╚═╝"
};
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
const vector<string> ASCII_O_WIN = {
    " ██████╗     ██╗    ██╗██╗███╗   ██╗",
    "██╔═══██╗    ██║    ██║██║████╗  ██║",
    "██║   ██║    ██║ █╗ ██║██║██╔██╗ ██║",
    "██║   ██║    ██║███╗██║██║██║╚██╗██║",
    "╚██████╔╝    ╚███╔███╔╝██║██║ ╚████║",
    " ╚═════╝      ╚══╝╚══╝ ╚═╝╚═╝  ╚═══╝"
};
const vector<string> ASCII_DRAW = {
    "██████╗  ██████╗  █████╗ ██╗    ██╗",
    "██╔══██╗ ██╔══██╗██╔══██╗██║    ██║",
    "██║  ██║ ██████╔╝███████║██║ █╗ ██║",
    "██║  ██║ ██╔══██╗██╔══██║██║███╗██║",
    "██████╔╝ ██║  ██║██║  ██║╚███╔███╔╝",
    "╚═════╝  ╚═╝  ╚═╝╚═╝  ╚═╝ ╚══╝╚══╝"
};

struct ColorTemp {
    int r, g, b;
    bool operator==(const ColorTemp& o) const { return r == o.r && g == o.g && b == o.b; }
};

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
    COORD bufferSize = { (SHORT)width, (SHORT)height };
    SetConsoleScreenBufferSize(hConsole, bufferSize);
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

int CenterXW(std::wstring s) {
    return (CONSOLE_WIDTH - s.length()) / 2;
}

void PrintCenteredText(std::string text, int boxWidth) {
    if (text.length() > boxWidth) {
        text = text.substr(0, boxWidth);
    }
    int left = (boxWidth - text.length()) / 2;
    int right = boxWidth - text.length() - left;
    for (int i = 0; i < left; i++) std::cout << " ";
    std::cout << text;
    for (int i = 0; i < right; i++) std::cout << " ";
}

void DrawImageHalfBlock(int startX, int startY, const std::vector<DrawInstructionTrueColor>& data, float brightness) {
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
    buffer += "\x1b[" + std::to_string(startY + 1) + ";" + std::to_string(startX + 1) + "H";
    ColorTemp lastTop = { -1,-1,-1 }, lastBot = { -1,-1,-1 };
    for (int y = 0; y < height; y += 2) {
        buffer += "\x1b[" + std::to_string(startY + (y / 2) + 1) + ";" + std::to_string(startX + 1) + "H";
        for (int x = 0; x < width; ++x) {
            ColorTemp top = pixelGrid[y][x];
            ColorTemp bot = pixelGrid[y + 1][x];
            if (brightness < 1.0f) {
                top.r = static_cast<int>(top.r * brightness);
                top.g = static_cast<int>(top.g * brightness);
                top.b = static_cast<int>(top.b * brightness);
                bot.r = static_cast<int>(bot.r * brightness);
                bot.g = static_cast<int>(bot.g * brightness);
                bot.b = static_cast<int>(bot.b * brightness);
            }
            if (!(bot == lastBot)) {
                buffer += "\x1b[48;2;" + std::to_string(bot.r) + ";" + std::to_string(bot.g) + ";" + std::to_string(bot.b) + "m";
                lastBot = bot;
            }
            if (!(top == lastTop)) {
                buffer += "\x1b[38;2;" + std::to_string(top.r) + ";" + std::to_string(top.g) + ";" + std::to_string(top.b) + "m";
                lastTop = top;
            }
            buffer += "▀";
        }
    }
    buffer += "\x1b[0m";
    std::cout << buffer << std::flush;
}

void DrawFrame(int x, int y, int width, int height) {
    int inner_width = width - 2;
    GotoXY(x, y);
    std::wcout << L"╔";
    for (int i = 0; i < inner_width; ++i) std::wcout << L"═";
    std::wcout << L"╗";
    for (int i = 1; i < height - 1; ++i) {
        GotoXY(x, y + i); std::wcout << L"║";
        GotoXY(x + width - 1, y + i); std::wcout << L"║";
    }
    GotoXY(x, y + height - 1);
    std::wcout << L"╚";
    for (int i = 0; i < inner_width; ++i) std::wcout << L"═";
    std::wcout << L"╝";
}

void DrawFrame1(int x, int y, int width, int height) {
    int inner_width = width - 2;
    GotoXY(x, y);
    std::wcout << L"┌";
    for (int i = 0; i < inner_width; ++i) std::wcout << L"─";
    std::wcout << L"┐";
    for (int i = 1; i < height - 1; ++i) {
        GotoXY(x, y + i); std::wcout << L"│";
        GotoXY(x + width - 1, y + i); std::wcout << L"│";
    }
    GotoXY(x, y + height - 1);
    std::wcout << L"└";
    for (int i = 0; i < inner_width; ++i) std::wcout << L"─";
    std::wcout << L"┘";
}

void horizontalLine(int X, int Y, int width, int height) {
    for (int i = 0; i < width; i++) {
        GotoXY(X + i, Y + height);
        std::wcout << L"─";
    }
}

void DrawBigArt_Color(int x, int y, const std::vector<std::string>& art, int r, int g, int b) {
    SetBgRGB(97, 100, 151);
    SetColorRGB(r, g, b);
    for (int i = 0; i < art.size(); i++) {
        GotoXY(x, y + i);
        std::cout << art[i];
    }
}

void ShowGroupIntro() {
    _setmode(_fileno(stdout), _O_TEXT);
    SetCursorVisible(false);
    ClearScreenWithColor(0, 0, 0);
    int x = 37;
    int y = 5;
    for (float b = 0.0f; b <= 1.0f; b += 0.04f) {
        DrawGroup6(x, y, b);
        Sleep(10);
    }
    DrawGroup6(x, y, 1.0f);
    Sleep(2500);
    for (float b = 1.0f; b >= 0.0f; b -= 0.04f) {
        DrawGroup6(x, y, b);
        Sleep(0);
    }
    ClearScreenWithColor(0, 0, 0);
}

void DrawLoadingScreen() {
    ClearScreenWithColor(255, 255, 255);
    DrawLoading(0, 0);
    _setmode(_fileno(stdout), _O_U16TEXT);
    SetBgW(97, 100, 151);
    SetColorW(255, 255, 255);

    int barWidth = 60;
    int barY = 20;
    int barX = (CONSOLE_WIDTH - barWidth) / 2;

    SetColorW(255, 255, 255);
    GotoXY(barX - 1, barY - 1); std::wcout << L"╔" << std::wstring(barWidth, L'═') << L"╗";
    GotoXY(barX - 1, barY);      std::wcout << L"║";
    GotoXY(barX + barWidth, barY); std::wcout << L"║";
    GotoXY(barX - 1, barY + 1); std::wcout << L"╚" << std::wstring(barWidth, L'═') << L"╝";

    GotoXY(CenterXW(L"LOADING..."), barY - 2);
    std::wcout << L"LOADING...";

    for (int i = 0; i <= barWidth; ++i) {
        int percent = (i * 100) / barWidth;
        GotoXY(barX + i, barY);
        if (i < barWidth) {
            SetColorW(100, 100, 255);
            std::wcout << L"█";
        }
        SetBgW(97, 100, 151); SetColorW(255, 255, 255);
        std::wstring perStr = std::to_wstring(percent) + L"%";
        GotoXY(CenterXW(perStr), barY + 2);
        std::wcout << perStr;
        if (i < 20) std::this_thread::sleep_for(std::chrono::milliseconds(20));
        else if (i < 50) std::this_thread::sleep_for(std::chrono::milliseconds(5));
        else std::this_thread::sleep_for(std::chrono::milliseconds(30));
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(300));
}

void DrawPressAnyKeyPrompt(bool showText) {
    _setmode(_fileno(stdout), _O_U16TEXT);
    int barWidth = 60;
    int barY = 20;
    int barX = (CONSOLE_WIDTH - barWidth) / 2;
    SetBgW(97, 100, 151);
    GotoXY(0, barY - 2); std::wcout << std::wstring(120, L' ');
    GotoXY(0, barY + 2); std::wcout << std::wstring(120, L' ');
    std::wstring prompt = L">> PRESS ANY KEY TO START <<";
    int promptX = CenterXW(prompt);

    if (showText) {
        SetColorW(255, 50, 50);
        GotoXY(promptX, barY + 2);
        std::wcout << prompt;
    }
    else {
        GotoXY(promptX, barY + 2);
        std::wcout << std::wstring(prompt.length(), L' ');
    }
    _setmode(_fileno(stdout), _O_TEXT);
    ResetColor();
}

void DrawAboutScreen() {
    ClearScreenWithColor(202, 196, 248);
    DrawAboutbackground(0, 0);
    _setmode(_fileno(stdout), _O_U16TEXT);
    SetBgW(255, 255, 209);
    SetColorW(0, 0, 0);

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
            GotoXY(tableCoord.X + 14, tableCoord.Y + 5);
            std::wcout << inforLogo[i];
        }
        else {
            GotoXY(45, (inforLogoY + i * offset) + 2);
            std::wcout << inforLogo[i];
        }
    }

    _setmode(_fileno(stdout), _O_TEXT);
    const char* guide = "'ESC': quit";
    GotoXY(CenterX(guide) + 3, 51);
    std::cout << guide;
    ResetColor();
}

void DrawGuideScreen() {
    ClearScreenWithColor(97, 100, 151);
    DrawGuidebackground(0, 0);
    _setmode(_fileno(stdout), _O_U16TEXT);
    SetBgW(255, 255, 209);
    SetColorW(0, 0, 0);
    int X = 26, Y = 15;
    GotoXY(X + 38, Y + 1);
    std::wcout << L"Game features and in-game operations: ";
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
        GotoXY(X + 5, Y + i * 2 + 4);
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
        GotoXY(X + 65, Y + i * 2 + 4);
        std::wcout << Player2Guide[i];
    }
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
    _setmode(_fileno(stdout), _O_TEXT);
    const char* guide = "'ESC': quit";
    GotoXY(CenterX(guide) + 3, 51);
    std::cout << guide;
    ResetColor();
}

void DrawMenuItem(int index, bool is_selected) {
    if (index < 0 || index >= TOTAL_ITEMS) return;
    int POS_X[] = { 74, 74, 74, 75, 75, 75 };
    int POS_Y[] = { 19, 22, 25, 28, 31, 34 };
    int x = POS_X[index];
    int y = POS_Y[index];
    std::string text = MENU_ITEMS[index];
    std::string displayStr;
    char icon = char(15);
    if (is_selected) {
        displayStr = std::string(" ") + icon + " " + text + " " + icon + " ";
    }
    else {
        displayStr = "   " + text + "   ";
    }
    GotoXY(x, y);
    SetBgRGB(218, 66, 76);
    if (is_selected) {
        SetColorRGB(255, 255, 255);
        std::cout << displayStr;
    }
    else {
        SetColorRGB(255, 255, 0);
        std::cout << displayStr;
    }
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
    int POS_X[] = { 75, 75, 77 };
    int POS_Y[] = { 24, 27, 30 };
    if (index < 0 || index >= sizeof(POS_X) / sizeof(POS_X[0])) return;
    int x = POS_X[index];
    int y = POS_Y[index];
    std::string text = NEW_GAME_OPTIONS[index];
    std::string displayStr;
    char icon = char(15);
    if (is_selected) {
        displayStr = std::string(" ") + icon + " " + text + " " + icon + " ";
    }
    else {
        displayStr = "   " + text + "   ";
    }
    GotoXY(x, y);
    SetBgRGB(218, 66, 76);
    if (is_selected) {
        SetColorRGB(255, 255, 255);
        std::cout << displayStr;
    }
    else {
        SetColorRGB(255, 255, 0);
        std::cout << displayStr;
    }
    SetBgRGB(0, 0, 0);
    SetColorRGB(255, 255, 255);
}

void DrawFullNewGameMenu(int selected_index) {
    ClearScreenWithColor(97, 100, 151);
    DrawLargebackground1(0, 0);
    DrawGamemodeframe(67, 21);
    DrawGamemode(23, 2);
    DrawTutorial(37, 40);
    SetBgRGB(176, 140, 88);
    SetColorRGB(0, 0, 0);
    GotoXY(40, 42); std::cout << "'W, S': MOVE";
    GotoXY(40, 44); std::cout << "BETWEEN MENU";
    GotoXY(40, 46); std::cout << "'ESC': QUIT";
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

void Update2PlayerNameScreen(int activeControl, const char* p1_buffer, const char* p2_buffer) {
    SetBgRGB(218, 66, 76);
    GotoXY(51, 26);
    SetColorRGB(255, 255, 255);
    std::cout << p1_buffer;
    for (int i = strlen(p1_buffer); i < 20; i++) std::cout << " ";

    GotoXY(104, 26);
    SetColorRGB(255, 255, 255);
    std::cout << p2_buffer;
    for (int i = strlen(p2_buffer); i < 20; i++) std::cout << " ";

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
    ResetColor();
}

void DrawBoard(int pSize) {
    SetColorRGB(0, 0, 0);
    SetBgRGB(97, 100, 151);
    for (int j = 0; j <= pSize; j++) {
        for (int i = 0; i <= pSize; i++) {
            int x = LEFT + CELL_VISUAL_WIDTH * i;
            int y = TOP + CELL_VISUAL_HEIGHT * j;
            GotoXY(x, y);
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
                for (int k = 0; k < CELL_VISUAL_WIDTH - 1; k++) std::cout << "═";
            }
            if (j < pSize) {
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
                int visualX = LEFT + (j * CELL_VISUAL_WIDTH);
                int visualY = TOP + (i * CELL_VISUAL_HEIGHT);
                int padX = 2;
                int padY = 1;
                if (_A[i][j].c == -1) {
                    DrawXmini(visualX + padX, visualY + padY);
                }
                else {
                    DrawOmini(visualX + padX, visualY + padY);
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

void DrawStatic2P_UI() {
    const int p1_box_x = 14, p1_box_y = 5;
    const int p2_box_x = 133, p2_box_y = 5;
    SetBgRGB(135, 236, 255);
    GotoXY(p1_box_x - 3, p1_box_y + 5);     PrintCenteredText(std::string(_player1_name) + " (X) ", 23);
    GotoXY(p1_box_x - 2, p1_box_y + 7); std::cout << "WINS: ";
    GotoXY(p1_box_x - 2, p1_box_y + 9); std::cout << "TIME LEFT: ";
    GotoXY(p1_box_x - 2, p1_box_y + 11); std::cout << "MOVES: ";
    GotoXY(p2_box_x - 2, p2_box_y + 5);     PrintCenteredText(std::string(_player2_name) + " (O) ", 23);
    GotoXY(p2_box_x - 1, p2_box_y + 7); std::cout << "WINS: ";
    GotoXY(p2_box_x - 1, p2_box_y + 9); std::cout << "TIME LEFT: ";
    GotoXY(p2_box_x - 1, p2_box_y + 11); std::cout << "MOVES: ";
    GotoXY(78, 5); std::cout << "ROUND: " << _round;
}

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
    const int p1_box_x = 14, p1_box_y = 5;
    const int p2_box_x = 133, p2_box_y = 5;
    SetColorRGB(0, 0, 0);
    SetBgRGB(bgR, bgG, bgB);
    GotoXY(p1_box_x + 4, p1_box_y + 7);
    std::cout << _player1_score << "  ";
    GotoXY(p1_box_x + 5, p1_box_y + 11);
    std::cout << (_moveCount + 1) / 2 << "  ";
    GotoXY(p2_box_x + 5, p2_box_y + 7);
    std::cout << _player2_score << "  ";
    GotoXY(p2_box_x + 6, p2_box_y + 11);
    std::cout << _moveCount / 2 << "  ";
    GotoXY(85, 5);
    std::cout << _round;
    int min = _turnTimer / 60;
    int sec = _turnTimer % 60;
    char runningTime[30];
    sprintf_s(runningTime, "%02d:%02d", min, sec);
    char fullTime[] = "02:00";
    if (_currentPlayer == 1) {
        GotoXY(p1_box_x + 9, p1_box_y + 9);
        SetColorRGB(0, 0, 0);
        std::cout << runningTime;
        GotoXY(p2_box_x + 10, p2_box_y + 9);
        SetColorRGB(0, 0, 0);
        std::cout << fullTime;
    }
    else {
        GotoXY(p2_box_x + 10, p2_box_y + 9);
        SetColorRGB(0, 0, 0);
        std::cout << runningTime;
        GotoXY(p1_box_x + 9, p1_box_y + 9);
        SetColorRGB(0, 0, 0);
        std::cout << fullTime;
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
    DrawBoard(BOARD_SIZE);
    SetColorRGB(0, 0, 0);
    DrawStatic2P_UI();
    UpdateDynamic2P_UI();
}

void DrawTimerOnly() {
    int min = _turnTimer / 60;
    int sec = _turnTimer % 60;
    char runningTime[30];
    sprintf_s(runningTime, "%02d:%02d", min, sec);
    const int p1_box_x = 14, p1_box_y = 5;
    const int p2_box_x = 133, p2_box_y = 5;
    if (_currentPlayer == 1) {
        GotoXY(p1_box_x + 9, p1_box_y + 9);
        SetColorRGB(0, 0, 0);
        SetBgRGB(135, 236, 255);
        std::cout << runningTime;
    }
    else {
        GotoXY(p2_box_x + 10, p2_box_y + 9);
        SetColorRGB(0, 0, 0);
        SetBgRGB(135, 236, 255);
        std::cout << runningTime;
    }
    SetColorRGB(0, 0, 0);
}

void DrawFixedLine(int startX, int y, std::string label, std::string value, int width) {
    GotoXY(startX, y);
    SetBgRGB(135, 236, 255);
    SetColorRGB(0, 0, 0);
    std::cout << label;
    int currentPos = 2 + label.length();
    int valueStartPos = 12;
    if (currentPos < valueStartPos) std::cout << std::string(valueStartPos - currentPos, ' ');
    std::cout << value;
    int printedLen = (currentPos > valueStartPos ? currentPos : valueStartPos) + value.length();
    int padding = width - 7 - printedLen;
    if (padding < 0) padding = 0;
    std::cout << std::string(padding, ' ');
}

void DrawGameOverScreen(int selectedOption, bool drawAll) {
    const int BOX_WIDTH = 50;
    const int BOX_HEIGHT = 17;
    const int BOX_X = (CONSOLE_WIDTH - BOX_WIDTH) / 2;
    const int BOX_Y = 16;
    int p1_Art_X = 0, p1_Art_Y = 0;
    int p2_Art_X = 0, p2_Art_Y = 0;

    if (drawAll) {
        ClearScreenWithColor(97, 100, 151);
        DrawLargebackground(0, 0);
        DrawGameresults(10, 2);
        DrawNewtutorial(22, 30);
        SetColorRGB(0, 0, 0);
        SetBgRGB(176, 140, 88);
        GotoXY(25, 32); std::cout << "'TAB': SWITCH";
        GotoXY(25, 34); std::cout << "'ENTER': CONFIRM";
        GotoXY(25, 36); std::cout << "'ESC': QUIT";

        std::string headerMsg;
        int winner = 0;
        if (_gameWinner == 1) { winner = 1; headerMsg = "PLAYER " + std::string(_player1_name) + " (X) WON!"; }
        else if (_gameWinner == 2) { winner = 2; headerMsg = "PLAYER " + std::string(_player2_name) + " (O) WON!"; }
        else { headerMsg = "IT'S A DRAW!"; }

        if (winner != 0) {
            SetBgRGB(97, 100, 151);
            const std::vector<std::string>* p1_Char;
            const std::vector<std::string>* p1_Result;
            const std::vector<std::string>* p2_Char;
            const std::vector<std::string>* p2_Result;
            int colorP1, colorP2;

            if (winner == 1) {
                p1_Char = &ART_X_BIG; p1_Result = &WIN_ART; colorP1 = 0xFFD700;
                p2_Char = &ART_O_BIG; p2_Result = &LOSE_ART; colorP2 = 0x87CEFA;
                p1_Art_X = 11;
                p1_Art_Y = BOX_Y + 5;
                p2_Art_X = 113;
                p2_Art_Y = BOX_Y + 5;
            }
            else {
                p1_Char = &ART_X_BIG; p1_Result = &LOSE_ART; colorP1 = 0x87CEFA;
                p2_Char = &ART_O_BIG; p2_Result = &WIN_ART; colorP2 = 0xFFD700;
                p1_Art_X = 7;
                p1_Art_Y = BOX_Y + 5;
                p2_Art_X = 119;
                p2_Art_Y = BOX_Y + 5;
            }

            SetColorRGB((colorP1 >> 16) & 0xFF, (colorP1 >> 8) & 0xFF, colorP1 & 0xFF);
            for (int i = 0; i < ART_HEIGHT; ++i) {
                GotoXY(p1_Art_X, p1_Art_Y + i);
                std::cout << (*p1_Char)[i];
                GotoXY(p1_Art_X + 11, p1_Art_Y + i);
                std::cout << (*p1_Result)[i];
            }
            SetColorRGB((colorP2 >> 16) & 0xFF, (colorP2 >> 8) & 0xFF, colorP2 & 0xFF);
            for (int i = 0; i < ART_HEIGHT; ++i) {
                GotoXY(p2_Art_X, p2_Art_Y + i);
                std::cout << (*p2_Char)[i];
                GotoXY(p2_Art_X + 12, p2_Art_Y + i);
                std::cout << (*p2_Result)[i];
            }
        }
        SetBgRGB(97, 100, 151);
        SetColorRGB(0, 0, 0);
        DrawGameresultframe(BOX_X, BOX_Y);
        GotoXY(BOX_X + 3, BOX_Y + 2);
        int padHead = (BOX_WIDTH - 2 - headerMsg.length()) / 2;
        if (padHead < 0) padHead = 0;
        SetBgRGB(135, 236, 255);
        SetColorRGB(0, 0, 0);
        std::cout << std::string(padHead, ' ') << headerMsg;
        std::stringstream ss_score;
        ss_score << std::setw(2) << std::setfill('0') << _player1_score << "  :  "
            << std::setw(2) << std::setfill('0') << _player2_score;
        std::string names = std::string(_player1_name) + "(X) | (O)" + std::string(_player2_name);
        int xMoves = (_moveCount + 1) / 2;
        int oMoves = _moveCount / 2;
        std::string moves = std::to_string(xMoves) + " (X) | (O) " + std::to_string(oMoves);
        SetBgRGB(97, 100, 151);
        DrawFixedLine(BOX_X + 4, BOX_Y + 4, "NAME:", names, BOX_WIDTH);
        DrawFixedLine(BOX_X + 4, BOX_Y + 6, "SCORE:", ss_score.str(), BOX_WIDTH);
        DrawFixedLine(BOX_X + 4, BOX_Y + 8, "MODE:", "2 PLAYERS", BOX_WIDTH);
        DrawFixedLine(BOX_X + 4, BOX_Y + 10, "MOVES:", moves, BOX_WIDTH);
    }
    SetBgRGB(135, 236, 255);
    int centerX = BOX_X + (BOX_WIDTH / 2);
    int ySave = BOX_Y + 13;
    int yPlay = BOX_Y + 15;
    GotoXY(centerX - 4, ySave);
    if (selectedOption == 0) { SetColorRGB(255, 255, 0); std::cout << "☼ SAVE ☼"; }
    else { SetColorRGB(0, 0, 0); std::cout << "  SAVE  "; }
    GotoXY(BOX_X + 4, yPlay);
    if (selectedOption == 1) { SetColorRGB(255, 255, 0); std::cout << "☼ PLAY AGAIN ☼"; }
    else { SetColorRGB(0, 0, 0); std::cout << "  PLAY AGAIN  "; }
    GotoXY(BOX_X + BOX_WIDTH - 16, yPlay);
    if (selectedOption == 2) { SetColorRGB(255, 255, 0); std::cout << "☼ BACK ☼"; }
    else { SetColorRGB(0, 0, 0); std::cout << "  BACK  "; }
    SetColorRGB(0, 0, 0);
}

void DrawPauseMenuItem(int index, bool is_selected) {
    if (index < 0 || index >= TOTAL_PAUSE_ITEMS) return;
    int POS_X[] = { 76, 76, 77, 77, 76, 77 };
    int POS_Y[] = { 16, 19, 22, 25, 28, 31 };
    int x = POS_X[index];
    int y = POS_Y[index];
    std::string text = std::string(PAUSE_ITEMS[index]);
    std::string displayStr;
    char icon = char(15);
    if (is_selected) {
        displayStr = std::string(" ") + icon + " " + text + " " + icon + " ";
    }
    else {
        displayStr = "   " + text + "   ";
    }
    GotoXY(x, y);
    SetBgRGB(218, 66, 76);
    if (is_selected) {
        SetColorRGB(255, 255, 255);
        std::cout << displayStr;
    }
    else {
        SetColorRGB(255, 255, 0);
        std::cout << displayStr;
    }
    SetBgRGB(0, 0, 0);
    SetColorRGB(255, 255, 255);
}

void DrawFullPauseMenu(int selected_index) {
    ClearScreenWithColor(97, 100, 151);
    DrawLargebackground(0, 0);
    DrawGamepaused(13, 2);
    DrawFramepausemenu(66, 12);
    for (int i = 0; i < TOTAL_PAUSE_ITEMS; i++) {
        DrawPauseMenuItem(i, i == selected_index);
    }
}

void DrawWinEffectFrame(int winner, int colorIdx) {
    int xmasColors[3][3] = {
        {255, 82, 82},
        {0, 230, 118},
        {255, 215, 0}
    };
    int drawColors[3][3] = { {255, 255, 255}, {176, 224, 230}, {192, 192, 192} };
    const vector<string>* textToDraw;
    if (winner == 1) textToDraw = &ASCII_X_WIN;
    else if (winner == 2) textToDraw = &ASCII_O_WIN;
    else textToDraw = &ASCII_DRAW;

    int textWidth = (winner == 0) ? 40 : 35;
    int startX = (165 - textWidth) / 2;
    int startY = 25;

    int r, g, b;
    if (winner == 0) {
        r = drawColors[colorIdx % 3][0]; g = drawColors[colorIdx % 3][1]; b = drawColors[colorIdx % 3][2];
    }
    else {
        r = xmasColors[colorIdx % 3][0]; g = xmasColors[colorIdx % 3][1]; b = xmasColors[colorIdx % 3][2];
    }

    SetColorRGB(r, g, b);
    SetBgRGB(97, 100, 151);

    for (int i = 0; i < textToDraw->size(); i++) {
        GotoXY(startX, startY + i);
        cout << (*textToDraw)[i];
    }

    GotoXY(startX - 2, startY + 8);
    cout << "   - PRESS ANY KEY TO CONTINUE -   ";
}