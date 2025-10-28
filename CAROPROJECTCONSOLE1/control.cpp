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
void StartGame() {
	system("cls");
	ResetData(); // Khoi tao du lieu goc
	DrawBoard(BOARD_SIZE); // Ve man hinh game
	GotoXY(_X, _Y);
}

void ExitGame() {
	system("cls");
	GabageCollect();
	// co the luu game trc khi exit
}

void MoveRight() {
	if (_X < _A[BOARD_SIZE - 1][BOARD_SIZE - 1].x) {
		_X += 4;
		GotoXY(_X, _Y);
	}
}

void MoveLeft() {
	if (_X > _A[0][0].x) {
		_X -= 4;
		GotoXY(_X, _Y);
	}
}

void MoveDown() {
	if (_Y < _A[BOARD_SIZE - 1][BOARD_SIZE - 1].y) {
		_Y += 2;
		GotoXY(_X, _Y);
	}
}

void MoveUp() {
	if (_Y > _A[0][0].y) {
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
	
	// 2. Nền trắng, chữ đen mặc định
	system("color F0");

	// 3. Tạm thời ẩn con trỏ (menu)
	SetCursorVisible(false);

	// 4️ Mở rộng vùng buffer (nếu không, sẽ bị giới hạn khi set window)
	COORD bufferSize = { 150, 40 }; // muốn to bao nhiêu tùy
	SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), bufferSize);

	// 5️ Đặt kích thước cửa sổ console
	SMALL_RECT windowSize = { 0, 0, 149, 39 }; //  giá trị phải nhỏ hơn bufferSize
	SetConsoleWindowInfo(GetStdHandle(STD_OUTPUT_HANDLE), TRUE, &windowSize);
}

// Khi vào bàn cờ hoặc chỗ cần con trỏ hiện
void ShowCursorForBoard() {
	SetCursorVisible(true);
}


