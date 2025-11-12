#include "Control.h"
#include "View.h"
#include "Model.h"
#include <stdlib.h>
#include <Windows.h>
#include <string>
#include <conio.h>
#include <iostream>
#include "GameState.h"
#include <io.h>
#include <fcntl.h>
#include <chrono>
#include "truecolor_utils.h"

void StartGame() {
	// Bước 1: Dọn dẹp màn hình cũ và chuẩn bị nền trắng cho bàn cờ.
	// Lệnh này vừa đặt nền trắng, vừa xóa sạch mọi thứ.
	ClearScreenWithColor(255, 255, 255);
	SetColorRGB(0, 0, 0);
	ResetData(); // Khoi tao du lieu goc
	DrawBoard(BOARD_SIZE); // Ve man hinh game
	GotoXY(_X, _Y);
}

void ExitGame() {
	// Bước 1: Đưa màu sắc của console về trạng thái mặc định.
	ResetColor();
	// Bước 2: Xóa màn hình về nền đen (màu console mặc định) cho sạch sẽ.
// Lệnh này an toàn hơn system("cls") rất nhiều.
	ClearScreenWithColor(0, 0, 0);
	GabageCollect();
	// co the luu game trc khi exit
}

void MoveRight() {
	// Tọa độ X của cột cuối cùng là: LEFT + (số cột - 1) * 4
	int right_boundary = LEFT + (BOARD_SIZE - 1) * 4;
	if (_X < right_boundary) {
		_X += 4;
		GotoXY(_X, _Y);
	}
}

void MoveLeft() {
	// Tọa độ X của cột đầu tiên là LEFT
	if (_X > LEFT + 2) {
		_X -= 4;
		GotoXY(_X, _Y);
	}
}

void MoveDown() {
	// Tọa độ Y của hàng cuối cùng là: TOP + (số hàng - 1) * 2
	int bottom_boundary = TOP + (BOARD_SIZE - 1) * 2;
	if (_Y < bottom_boundary) {
		_Y += 2;
		GotoXY(_X, _Y);
	}
}

void MoveUp() {
	// Tọa độ Y của hàng đầu tiên là TOP
	if (_Y > TOP + 1) {
		_Y -= 2;
		GotoXY(_X, _Y);
	}
}

// Ẩn/hiện con trỏ
void SetCursorVisible(bool visible) {
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO cursorInfo;
	if (GetConsoleCursorInfo(hConsole, &cursorInfo)) {
		cursorInfo.bVisible = visible ? TRUE : FALSE;
		SetConsoleCursorInfo(hConsole, &cursorInfo);
	}
}

// Hàm InitConsole chuẩn
void InitConsole() {
	system("chcp 65001");
	SetCursorVisible(false);
}

// Khi vào bàn cờ hoặc chỗ cần con trỏ hiện
void ShowCursorForBoard() {
	SetCursorVisible(true);
}