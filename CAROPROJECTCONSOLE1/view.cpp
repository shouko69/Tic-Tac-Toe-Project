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
}


// --- VẼ BÀN CỜ ---
void DrawBoard(int pSize) {
    // Vòng lặp này sẽ đi qua từng "giao điểm" trên lưới caro
    SetColorRGB(0, 0, 0);
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
// Đặt hàm này trong file View.cpp hoặc file tương tự
void RedrawBoardState() {
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            // Lấy tọa độ màn hình từ chỉ số mảng
            int screenX = LEFT + j * 4 + 2; // Tọa độ X để vẽ
            int screenY = TOP + i * 2 + 1;  // Tọa độ Y để vẽ

            GotoXY(screenX, screenY); // Di chuyển đến đúng ô

            if (_A[i][j].c == -1) { // Nếu là quân X
                SetColorRGB(255, 0, 0); // Màu đỏ
                std::cout << "X";
            }
            else if (_A[i][j].c == 1) { // Nếu là quân O
                SetColorRGB(0, 0, 255); // Màu xanh
                std::cout << "O";
            }
            // Nếu là 0 (ô trống) thì không làm gì cả
        }
    }
    // Sau khi vẽ xong, đặt lại màu mặc định
    SetColorRGB(0, 0, 0);
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

// ĐẶT KHỐI CODE NÀY Ở ĐẦU FILE View.cpp HOẶC NGAY TRÊN HÀM DrawGameOverScreen
// 2 cai vẽ này tạm thời
const char* WIN_ART[] = {
    "__      __ .__   __. .__       __.",
    "|  | /\\ |  ||  \\ |  | |  |     |  |",
    "|  |/  \\|  ||   \\|  | |  |     |  |",
    "|  '    '  ||  . `  | |  |     |  |",
    "|   /    \\ ||  |\\   | |  `----.|  |",
    "|__/      \\||__| \\__| |_______||__|"
};
const int WIN_ART_HEIGHT = 6; // Chiều cao của chữ WIN

const char* LOSE_ART[] = {
    ".______       .____    .___________. _______.",
    "|   _  \\      |    |   |           ||   ____|",
    "|  |_)  |     |    |   `---|  |----`|  |__   ",
    "|      /      |    |       |  |     |   __|  ",
    "|  |\\  \\----. |    |       |  |     |  |____ ",
    "| _| `._____||____|       |__|     |_______|"
};
const int LOSE_ART_HEIGHT = 6; // Chiều cao của chữ LOSE
void DrawGameOverScreen(int selectedOption) {
    // --- KHAI BÁO CÁC THAM SỐ GIAO DIỆN ---
    const int BOX_WIDTH = 60;
    const int BOX_X = CenterX(std::string(BOX_WIDTH, ' '));
    const int BOX_Y = 15;
    const int WIN_LOSE_Y = 20;
    const int P1_X = 25;
    const int P2_X = 100;

    // --- 1. DỌN DẸP MÀN HÌNH VÀ VẼ TIÊU ĐỀ ---
    ClearScreenWithColor(89, 79, 175);
    GotoXY(CenterX("Game results"), 10);
    std::cout << "Game results";

    // --- 2. XÁC ĐỊNH THÔNG TIN THẮNG/THUA ---
    std::string resultMessage;
    int winner = 0; // 0=Hòa, 1=P1, 2=P2

    if (_gameWinner == 1) {
        winner = 1;
        resultMessage = "Player " + std::string(_player1_name) + " (X) won the game";
    }
    else if (_gameWinner == 2) {
        winner = 2;
        resultMessage = "Player " + std::string(_player2_name) + " (O) won the game";
    }
    else {
        resultMessage = "The game ended in a draw!";
    }

    if (winner != 0) {
        // Vẽ WIN cho người thắng
        for (int i = 0; i < WIN_ART_HEIGHT; ++i) {
            GotoXY((winner == 1) ? P1_X : P2_X, WIN_LOSE_Y + i);
            std::cout << WIN_ART[i]; // Vẽ dòng thứ i của chữ WIN
        }
        // Vẽ LOSE cho người thua
        for (int i = 0; i < LOSE_ART_HEIGHT; ++i) {
            GotoXY((winner == 1) ? P2_X : P1_X, WIN_LOSE_Y + i);
            std::cout << LOSE_ART[i]; // Vẽ dòng thứ i của chữ LOSE
        }
    }


    // --- 4. TẠO NỘI DUNG VĂN BẢN CHO KHUNG ---
    std::stringstream ss_score, ss_moves;
    ss_score << std::setw(2) << std::setfill('O') << _player1_score << " | " << std::setw(2) << std::setfill('O') << _player2_score;
    int p1_moves = (_moveCount + 1) / 2;
    int p2_moves = _moveCount / 2;
    ss_moves << p1_moves << " (X) | (O) " << p2_moves;

    std::string nameLine = "Name:  " + std::string(_player1_name) + " (X) | (O) " + std::string(_player2_name);
    std::string scoreLine = "Score: " + ss_score.str();
    std::string modeLine = "Mode:  2 Players";
    std::string movesLine = "Moves: " + ss_moves.str();

    // --- 5. VẼ KHUNG VÀ NỘI DUNG (DÙNG KÝ TỰ TRỰC TIẾP) ---
    // Hàm lambda để căn lề phải cho chuỗi
    auto padString = [&](const std::string& s) {
        return s + std::string(BOX_WIDTH - 4 - s.length(), ' ');
        };

    // Vẽ viền trên
    GotoXY(BOX_X, BOX_Y);
    std::cout << "┌" << std::string(BOX_WIDTH - 2, '─') << "┐";

    // Vẽ các dòng nội dung và viền hai bên
    GotoXY(BOX_X, BOX_Y + 1); std::cout << "│ " << padString(resultMessage) << " │";
    GotoXY(BOX_X, BOX_Y + 2); std::cout << "│ " << std::string(BOX_WIDTH - 4, ' ') << " │";
    GotoXY(BOX_X, BOX_Y + 3); std::cout << "│ " << padString(nameLine) << " │";
    GotoXY(BOX_X, BOX_Y + 4); std::cout << "│ " << padString(scoreLine) << " │";
    GotoXY(BOX_X, BOX_Y + 5); std::cout << "│ " << padString(modeLine) << " │";
    GotoXY(BOX_X, BOX_Y + 6); std::cout << "│ " << padString(movesLine) << " │";
    GotoXY(BOX_X, BOX_Y + 7); std::cout << "│ " << std::string(BOX_WIDTH - 4, ' ') << " │";

    // Vẽ các nút
    std::string saveText = (selectedOption == 0) ? ">> Save <<" : "   Save   ";
    std::string playAgainText = (selectedOption == 1) ? ">> Play again <<" : "   Play again   ";
    std::string backText = (selectedOption == 2) ? ">> Back <<" : "   Back   ";
    std::string buttonLine = saveText + std::string(10, ' ') + playAgainText + std::string(10, ' ') + backText;
    GotoXY(BOX_X, BOX_Y + 8); std::cout << "│ " << padString(buttonLine) << " │";

    // Vẽ viền dưới
    GotoXY(BOX_X, BOX_Y + 9);
    std::cout << "└" << std::string(BOX_WIDTH - 2, '─') << "┘";
}