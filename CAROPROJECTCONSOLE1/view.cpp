#include "View.h"
#include "Windows.h"
#include <conio.h>
#include "Model.h"
#include "Control.h"
#include <iostream>
#include <iomanip>   
#include <string>
#include "truecolor_utils.h"
#include <fcntl.h>
#include <io.h>
#include "GameState.h"
#include <cwchar> // THÊM VÀO: Thư viện cần thiết cho hàm SetConsoleFont
#include <sstream>
#include "GameAssets.h"
using namespace std;

const char* MENU_ITEMS[] = { "NEW GAME", "LOAD GAME", "SETTING", "GUIDE", "ABOUT", "EXIT" };
const char* PAUSE_ITEMS[] = { "RESUME", "RESTART", "SAVE", "SETTINGS", "QUIT" };
extern const char* NEW_GAME_OPTIONS[] = { "1 PLAYER", "2 PLAYERS", "BACK" };
extern const int TOTAL_NEW_GAME_OPTIONS = 3;


const int TOTAL_ITEMS = sizeof(MENU_ITEMS) / sizeof(MENU_ITEMS[0]);
const int START_Y = 15; 

const int TOTAL_PAUSE_ITEMS = sizeof(PAUSE_ITEMS) / sizeof(PAUSE_ITEMS[0]);
const int PAUSE_START_Y = 9;/* const int PAUSE_START_Y = 9;*/
const int FRAME_START_X = 99; // Tọa độ X nơi các khung menu bắt đầu
const int FRAME_WIDTH = 28;   // Chiều rộng của khung menu
// Ký tự vẽ khung (ĐÃ XÓA - SẼ SỬ DỤNG TRỰC TIẾP)

_POINT _A[BOARD_SIZE][BOARD_SIZE];
bool _TURN = true;
struct ColorTemp {
    int r, g, b;
    bool operator==(const ColorTemp& o) const { return r == o.r && g == o.g && b == o.b; }
};

// --- HÀM VẼ CHÍNH (Copy từ Graphics.cpp cũ sang) ---
void DrawImageHalfBlock(int startX, int startY, const std::vector<DrawInstructionTrueColor>& data) {
    if (data.empty()) return;

    // 1. Tính kích thước
    int width = 0, height = 0;
    for (const auto& instr : data) {
        width = (std::max)(width, instr.x + instr.l);
        height = (std::max)(height, instr.y + 1);
    }

    // 2. Tạo lưới pixel
    std::vector<std::vector<ColorTemp>> pixelGrid(height, std::vector<ColorTemp>(width, { 0, 0, 0 }));
    for (const auto& instr : data) {
        if (instr.y < height) {
            for (int i = 0; i < instr.l; ++i) {
                if (instr.x + i < width) pixelGrid[instr.y][instr.x + i] = { instr.r, instr.g, instr.b };
            }
        }
    }
    if (height % 2 != 0) { pixelGrid.push_back(std::vector<ColorTemp>(width, { 0, 0, 0 })); height++; }

    // 3. Vẽ Half-block
    std::string buffer;
    ColorTemp lastTop = { -1,-1,-1 }, lastBot = { -1,-1,-1 };

    for (int y = 0; y < height; y += 2) {
        buffer += "\x1b[" + std::to_string(startY + (y / 2) + 1) + ";" + std::to_string(startX + 1) + "H";
        for (int x = 0; x < width; ++x) {
            ColorTemp top = pixelGrid[y][x];
            ColorTemp bot = pixelGrid[y + 1][x];

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
// ***** HÀM MỚI 3: TỰ ĐỘNG CANH GIỮA CỬA SỔ CONSOLE *****
void CenterConsole() {
    // 1. Lấy handle của cửa sổ console
    HWND consoleWindow = GetConsoleWindow();
    if (consoleWindow == NULL) {
        return; // Không tìm thấy cửa sổ
    }

    // 2. Lấy kích thước hiện tại của cửa sổ console (tính bằng pixel)
    RECT rectClient;
    GetWindowRect(consoleWindow, &rectClient);
    int windowWidth = rectClient.right - rectClient.left;
    int windowHeight = rectClient.bottom - rectClient.top;

    // 3. Lấy kích thước của khu vực làm việc trên màn hình (đã trừ Taskbar)
    RECT rectScreen;
    SystemParametersInfo(SPI_GETWORKAREA, 0, &rectScreen, 0);
    int screenWidth = rectScreen.right - rectScreen.left;
    int screenHeight = rectScreen.bottom - rectScreen.top;

    // 4. Tính toán tọa độ X, Y mới để cửa sổ nằm giữa
    int newX = rectScreen.left + (screenWidth - windowWidth) / 2;
    int newY = rectScreen.top + (screenHeight - windowHeight) / 2;

    // 5. Di chuyển cửa sổ đến vị trí mới
    // MoveWindow(handle, X, Y, width, height, repaint)
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
int CenterX(const std::string& text) {
    // Sử dụng hằng số CONSOLE_WIDTH thay vì gọi hàm GetConsoleWidth()
    int x = (CONSOLE_WIDTH - (int)text.length()) / 2;

    // Đảm bảo tọa độ không bao giờ là số âm
    return (x > 0) ? x : 0;
}
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
}// can fix


// --- VẼ BÀN CỜ ---
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
// Nó đọc Model (_Board[][]) và in ra tất cả X, O
// Đặt hàm này trong file View.cpp hoặc file tương tự
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

    // 1. Chỉ lấy text gốc, không thêm ">>" hay "  "
    std::string menuText = std::string(MENU_ITEMS[index]);

    // 2. Quyết định màu sắc
    if (is_selected) {
        SetColorRGB(255, 100, 180); // Màu chữ NỔI BẬT
    }
    else {
        SetColorRGB(80, 60, 120); // Màu chữ BÌNH THƯỜNG
    }


    SetBgRGB(203, 196, 248);
    // 2. Tính toán tọa độ X để canh giữa KHUNG chứ không phải màn hình
    int text_length = menuText.length();
    int x = FRAME_START_X + (FRAME_WIDTH - text_length) / 2;

    int y = START_Y + index * 2 + 5;

    GotoXY(x, y);
    std::cout << menuText;
}

// --- VẼ TOÀN BỘ MENU ---

// --- VẼ TOÀN BỘ MENU ---
void DrawFullMenu(int selected_index) {

    // 1. Đặt màu nền tím và tô toàn bộ màn hình
    ClearScreenWithColor(203, 196, 248);
    
    int i = 25;
    GotoXY(i, 1);
    SetColorRGB(163, 87, 213);
    _setmode(_fileno(stdout), _O_U16TEXT);
    std::wcout << L"████████╗██╗  ██╗███████╗     ██████╗ █████╗ ██████╗  ██████╗      ██████╗  █████╗ ███╗   ███╗███████╗\n"; GotoXY(i, 2);
    std::wcout << L"╚══██╔══╝██║  ██║██╔════╝    ██╔════╝██╔══██╗██╔══██╗██╔═══██╗    ██╔════╝ ██╔══██╗████╗ ████║██╔════╝\n"; GotoXY(i, 3);
    std::wcout << L"   ██║   ███████║█████╗      ██║     ███████║██████╔╝██║   ██║    ██║  ███╗███████║██╔████╔██║█████╗  \n"; GotoXY(i, 4);
    std::wcout << L"   ██║   ██╔══██║██╔══╝      ██║     ██╔══██║██╔══██╗██║   ██║    ██║   ██║██╔══██║██║╚██╔╝██║██╔══╝  \n"; GotoXY(i, 5);
    std::wcout << L"   ██║   ██║  ██║███████╗    ╚██████╗██║  ██║██║  ██║╚██████╔╝    ╚██████╔╝██║  ██║██║ ╚═╝ ██║███████╗\n"; GotoXY(i, 6);
    std::wcout << L"   ╚═╝   ╚═╝  ╚═╝╚══════╝     ╚═════╝╚═╝  ╚═╝╚═╝  ╚═╝ ╚═════╝      ╚═════╝ ╚═╝  ╚═╝╚═╝     ╚═╝╚══════╝\n";
    _setmode(_fileno(stdout), _O_TEXT); // Trả về chế độ thường


    

    // --- 3. VẼ MENU ---
    // (Giữ nguyên code vẽ menu)
    for (int i = 0; i < TOTAL_ITEMS; i++) {
        DrawMenuItem(i, i == selected_index);
    }

    
}

// --- VẼ TỪNG MỤC PAUSE MENU (PHIÊN BẢN TRUECOLOR) ---
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

// --- VẼ TOÀN BỘ PAUSE MENU (PHIÊN BẢN TRUECOLOR) ---
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
//****HAIHAMMOI****
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
// Đặt hàm này vào View.h hoặc truecolor_utils.h
void ClearScreenWithColor(int r, int g, int b) {
    SetBgRGB(r, g, b); // Đặt màu nền mong muốn

    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD coordScreen = { 0, 0 }; // Góc trên bên trái
    DWORD cCharsWritten;
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    DWORD dwConSize;

    // Lấy thông tin kích thước của màn hình console
    if (!GetConsoleScreenBufferInfo(hConsole, &csbi)) {
        return;
    }
    dwConSize = csbi.dwSize.X * csbi.dwSize.Y;

    // Lấp đầy toàn bộ màn hình bằng ký tự trống (' ')
    if (!FillConsoleOutputCharacter(hConsole, (TCHAR)' ', dwConSize, coordScreen, &cCharsWritten)) {
        return;
    }

    // Lấy thuộc tính màu hiện tại
    if (!GetConsoleScreenBufferInfo(hConsole, &csbi)) {
        return;
    }

    // Áp dụng thuộc tính màu đó cho toàn bộ màn hình
    if (!FillConsoleOutputAttribute(hConsole, csbi.wAttributes, dwConSize, coordScreen, &cCharsWritten)) {
        return;
    }

    // Đưa con trỏ về vị trí đầu
    SetConsoleCursorPosition(hConsole, coordScreen);
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

// THAY THẾ HOÀN TOÀN HÀM DrawPlayerNameScreen CŨ BẰNG HÀM NÀY
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

//new
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



// ==================================================================================
// PHẦN 1: ĐỊNH NGHĨA TÀI NGUYÊN (CHUYỂN HẾT VỀ VECTOR CHO ĐỒNG BỘ)
// ==================================================================================
// Chiều cao chung (Dùng cho vòng lặp vẽ)
const int ART_HEIGHT = 6;
// 1. WIN ART (Đã chuyển sang vector)
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
// Hàm vẽ căn giữa một dòng text vào trong khung
// --- 2. HÀM VẼ 1 DÒNG TRONG HỘP (Helper Function) ---
// ==================================================================================
// PHẦN 2: HÀM VẼ DÒNG TRONG HỘP (CĂN CHỈNH THẲNG TẮP)
// ==================================================================================
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


// ==================================================================================
// PHẦN 3: HÀM CHÍNH (DRAW GAME OVER SCREEN)
// ==================================================================================
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