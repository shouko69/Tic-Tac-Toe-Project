#include "Control.h"
#include "View.h"
#include "Model.h"
#include <stdlib.h>
#include <Windows.h>
#include <string>
#include <conio.h>
#include <iostream>
#include "GameState.h"
// #include <io.h> // Không cần nữa
// #include <fcntl.h> // Không cần nữa
#include <chrono>
#include "truecolor_utils.h"

void StartGame() {
    // 1. Dọn dẹp màn hình và "sơn" nền lavender
    ClearScreenWithColor(202, 196, 248); // <-- Đã sửa về nền Lavender
    SetColorRGB(0, 0, 0); // Đặt chữ mặc định là đen

    // 2. Chuẩn bị dữ liệu và vẽ
    ResetData();
    DrawBoard(BOARD_SIZE);
    GotoXY(_X, _Y);
}

void ExitGame() {
    // 1. Đưa màu sắc của console về trạng thái mặc định.
    ResetColor();

    // 2. Xóa màn hình về nền đen
    ClearScreenWithColor(0, 0, 0);
    GabageCollect();
}

void MoveRight() {
    int right_boundary = LEFT + (BOARD_SIZE - 1) * 4;
    if (_X < right_boundary) {
        _X += 4;
        GotoXY(_X, _Y);
    }
}

void MoveLeft() {
    if (_X > LEFT + 2) {
        _X -= 4;
        GotoXY(_X, _Y);
    }
}

void MoveDown() {
    int bottom_boundary = TOP + (BOARD_SIZE - 1) * 2;
    if (_Y < bottom_boundary) {
        _Y += 2;
        GotoXY(_X, _Y);
    }
}

void MoveUp() {
    if (_Y > TOP + 1) {
        _Y -= 2;
        GotoXY(_X, _Y);
    }
}

void SetCursorVisible(bool visible) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;
    if (GetConsoleCursorInfo(hConsole, &cursorInfo)) {
        cursorInfo.bVisible = visible ? TRUE : FALSE;
        SetConsoleCursorInfo(hConsole, &cursorInfo);
    }
}

// Hàm InitConsole chuẩn (ĐÃ SỬA LỖI)
void InitConsole() {
    // 1. Báo cho console biết chúng ta dùng UTF-8
    // (Quan trọng để "╔" và "═" hiển thị đúng)
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);

    // 2. Ẩn con trỏ
    SetCursorVisible(false);

    // 3. XÓA BỎ: system("chcp 65001"); // Gây xung đột
}

void ShowCursorForBoard() {
    SetCursorVisible(true);
}