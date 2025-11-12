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




using namespace std;
const char* MENU_ITEMS[] = { "NEW GAME", "LOAD GAME", "SETTING", "GUIDE", "ABOUT", "EXIT" };
const char* PAUSE_ITEMS[] = { "RESUME", "RESTART", "SAVE", "SETTINGS", "QUIT" };
const int TOTAL_ITEMS = sizeof(MENU_ITEMS) / sizeof(MENU_ITEMS[0]);
const int START_Y = 15; /*const int START_Y = 15;*/

const int TOTAL_PAUSE_ITEMS = sizeof(PAUSE_ITEMS) / sizeof(PAUSE_ITEMS[0]);
const int PAUSE_START_Y = 9;/* const int PAUSE_START_Y = 9;*/
const int FRAME_START_X = 99; // Tọa độ X nơi các khung menu bắt đầu
const int FRAME_WIDTH = 28;   // Chiều rộng của khung menu
// Ký tự vẽ khung (ĐÃ XÓA - SẼ SỬ DỤNG TRỰC TIẾP)

_POINT _A[BOARD_SIZE][BOARD_SIZE];
bool _TURN = true;


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
    system("cls");
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