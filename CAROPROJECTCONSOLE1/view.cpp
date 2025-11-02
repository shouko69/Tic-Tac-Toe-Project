#include "View.h"
#include "Windows.h"
#include <conio.h>
#include "Model.h"
#include "Control.h"
#include <iostream>
#include <string>
#include "truecolor_utils.h"
#include "GameState.h" 
#include <io.h>       
#include <fcntl.h> 
#include <stdlib.h>
#include "HandleInput.h"

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

COORD GetConsoleCursorPosition()
{
    CONSOLE_SCREEN_BUFFER_INFO cbsi;
    HANDLE hConsoleOutput = GetStdHandle(STD_OUTPUT_HANDLE); // get console_output_handle
    if (GetConsoleScreenBufferInfo(hConsoleOutput, &cbsi))
    {
        return cbsi.dwCursorPosition;
    }
    else
    {
        // The function failed. Call GetLastError() for details.
        COORD invalid = { 0, 0 };
        return invalid;
    }
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

    // --- HƯỚNG DẪN DI CHUYỂN ---
    SetColor(COLOR_GUIDE);
    const char* guide = "'W': Move up, 'S': Move down,  'Enter': Select,  'ESC': Resume";
    GotoXY(CenterX(guide), PAUSE_START_Y + TOTAL_PAUSE_ITEMS * 2 + 3);
    std::cout << guide;

    SetColor(COLOR_NORMAL_TEXT);

}

//Draw gameScreen

void StartGuide() {
    SetColorRGB(51, 0, 132);
    system("cls");
    GabageCollect();
    int i = 58, offset = 1;
    //set to UTF16 text cout
    _setmode(_fileno(stdout), _O_U16TEXT);
	GotoXY(i, 2);
    std::wcout << L" ██████╗ ██╗   ██╗██╗██████╗ ███████╗\n"; GotoXY(i, 3);
    std::wcout << L"██╔════╝ ██║   ██║██║██╔══██╗██╔════╝\n"; GotoXY(i, 4);
    std::wcout << L"██║  ███╗██║   ██║██║██║  ██║█████╗  \n"; GotoXY(i, 5);
    std::wcout << L"██║   ██║██║   ██║██║██║  ██║██╔══╝  \n"; GotoXY(i, 6);
    std::wcout << L"╚██████╔╝╚██████╔╝██║██████╔╝███████╗\n"; GotoXY(i, 7);
    std::wcout << L" ╚═════╝ ╚═════╝ ╚═╝╚═════╝ ╚══════╝ \n"; 
	COORD guideCoor;
	guideCoor.X = 25;
	guideCoor.Y = 11;
	const char* guideText[16] = { 
        "Player Movement: ",
        "'W': Go up",
        "'S': Go down",
        "'A': Go left",
        "'D': Go right",
        "","","","","" ,
        "Caro is the game using the X and O symbols to represent players and followed by a set of rules: ", 
        "",
        "1. Players take turns placing their marks(X or O) on empty intersections.", 
        "2. \"X\" always plays first, followed by \"O\" player.", 
        "3. The first player to score five points in a row horizontally, vertically, or diagonally wins the game.", 
        "4. If the board is completely filled and no player has five in a row, the game ends in a draw."
        };

	GotoXY(22, 8);
	COORD currentPos = GetConsoleCursorPosition();
	int width = 110, height = 20; // ┏━┓ -> width = 1;

	_setmode(_fileno(stdout), _O_U16TEXT);

	GotoXY(currentPos.X, currentPos.Y); // top left
    std::wcout << L"┏";
    for (int i = 1; i <= width; i++) { // print column
        GotoXY(currentPos.X + i, currentPos.Y);
        std::wcout << L"━";
        GotoXY(currentPos.X + i, currentPos.Y + 2);
        std::wcout << L"━";
        GotoXY(currentPos.X + i, currentPos.Y + 10);
        std::wcout << L"━";
        GotoXY(currentPos.X + i, currentPos.Y + 12);
        std::wcout << L"━";
        GotoXY(currentPos.X + i, currentPos.Y + height + 1);
        std::wcout << L"━";
    }
    GotoXY(currentPos.X + width + 1, currentPos.Y); // top right
    std::wcout << L"┓";
    for (int i = 1; i <= height; i++) { // print column
        GotoXY(currentPos.X, currentPos.Y + i);
        std::wcout << L"┃";
        GotoXY(currentPos.X + width + 1, currentPos.Y + i);
        std::wcout << L"┃";
    }
    for (int i = 1; i <= height / 2 - 3; i++) {
        GotoXY(currentPos.X + 28, currentPos.Y + 2 + i);
        std::wcout << L"┃";
    }
    GotoXY(currentPos.X, currentPos.Y + height + 1); // bottom left
    std::wcout << L"┗";
    GotoXY(currentPos.X + width + 1, currentPos.Y + height + 1); // bottom left
    std::wcout << L"┛";

	_setmode(_fileno(stdout), _O_TEXT);

	for (int i = 0; i < sizeof(guideText) / sizeof(guideText[0]); i++) {
		GotoXY(guideCoor.X, guideCoor.Y + i);
		std::cout << guideText[i];
	}
    GotoXY(guideCoor.X + 37, guideCoor.Y - 2);
    std::cout << "Game features and in-game operations";
    GotoXY(guideCoor.X + 30, guideCoor.Y);
    std::cout << "Game Features:";
    GotoXY(guideCoor.X + 30, guideCoor.Y + 1);
    std::cout << "'L': Load game";
    GotoXY(guideCoor.X + 30, guideCoor.Y + 2);
    std::cout << "'Z': Undo the last move";
    GotoXY(guideCoor.X + 30, guideCoor.Y + 3);
    std::cout << "'Enter': Mark and Continue";
    GotoXY(guideCoor.X + 30, guideCoor.Y + 4);
    std::cout << "'Esc': Halt the game/ Return to the previous scene of the game";
    GotoXY(guideCoor.X + 37, guideCoor.Y + 8);
    std::cout << "Game Rules Summary(Gomoku / Caro)";
    ScreenHandle(MENU);
}

void StartAbout() {
    SetColorRGB(51, 0, 132);
    system("cls"); 
    GabageCollect();
    int i = 55, offset = 1;
    //set to UTF16 text cout
    _setmode(_fileno(stdout), _O_U16TEXT);
    GotoXY(i, 1);
    std::wcout << L" █████╗ ██████╗  ██████╗ ██╗   ██╗████████╗\n"; GotoXY(i, 2);
    std::wcout << L"██╔══██╗██╔══██╗██╔═══██╗██║   ██║╚══██╔══╝\n"; GotoXY(i, 3);
    std::wcout << L"███████║██████╔╝██║   ██║██║   ██║   ██║   \n"; GotoXY(i, 4);
    std::wcout << L"██╔══██║██╔══██╗██║   ██║██║   ██║   ██║   \n"; GotoXY(i, 5);
    std::wcout << L"██║  ██║██████╔╝╚██████╔╝╚██████╔╝   ██║   \n"; GotoXY(i, 6);
    std::wcout << L"╚═╝  ╚═╝╚═════╝  ╚═════╝  ╚═════╝    ╚═╝   \n";

    COORD tableCoord;
    tableCoord.X = 40;
    tableCoord.Y = 10;
    i = 0;
    GotoXY(tableCoord.X + 5, tableCoord.Y);
    std::wcout << L"Welcome to the Gomoku Game - made by team 6 - 25CTT3 - HCMUS" << std::endl;
    i = i + 2;
    GotoXY(tableCoord.X, tableCoord.Y + i + 2);
    std::wcout << L"25120215 - Nguyễn Minh Nhật \t \t PvE Mode - Game Content" << std::endl;
    i += offset;
    GotoXY(tableCoord.X, tableCoord.Y + i + 2);
    std::wcout << L"25120215 - Nguyễn Hoàng Huy\t \t Game Interface Design" << std::endl;
    i += offset;
    GotoXY(tableCoord.X, tableCoord.Y + i + 2);
    std::wcout << L"25120215 - Vũ Thanh Phong \t \t Game Graphic Design - Game Flow Design" << std::endl;
    i += offset;
    GotoXY(tableCoord.X, tableCoord.Y + i + 2);
    std::wcout << L"25120215 - Đỗ Lê Nhật Quang \t \t Game Interface Design - Game sound" << std::endl;
    i += offset;
    GotoXY(tableCoord.X, tableCoord.Y + i + 2);
    std::wcout << L"25120215 - Nguyễn Phú Quang \t \t Load & Save game" << std::endl;
    i += offset;
    GotoXY(tableCoord.X, tableCoord.Y + i + 2);
    std::wcout << L"25120215 - Nguyễn Vũ Nhật Quang\t \t Game Graphic Design" << std::endl;
    i += 4;
    GotoXY(tableCoord.X + 25, tableCoord.Y + i + 2);
    std::wcout << L"RELEASED IN 2025" << std::endl;
    i += 2;
    GotoXY(tableCoord.X + 25, tableCoord.Y + i + 2);
    std::wcout << L"PRESS ESC TO BACK" << std::endl;
    _setmode(_fileno(stdout), _O_TEXT);//set to UTF16 text cout
    ScreenHandle(MENU);
}

//void StartLoadGame()

//void StartSetting()

//void DrawEsc ()




