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

using namespace std; 

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

//void StartHelp() {
//	//DrawObject("Background");
//	//DrawObject("Border");
//	//DrawObject("Text_Border", 65, 95);
//	//DrawObject("CornerEsc");
//	GotoXY(65, 1);
//	//DrawObject("Help_Logo");
//
//	COORD helpTextCoor;
//	helpTextCoor.X = 35;
//	helpTextCoor.Y = 11;
//	const char* helpText[16] = { "Player Movement: ","'W': Go up","'S': Go down","'A': Go left","'D': Go right","'Enter': Mark","","","","" ,"Caro is the game using the X and O symbols to represent players and followed by a set of rules: ", "1. Two players play against each other on 16 x 16 grid.", "2. \"X\" player gets to take the first turn followed by \"O\" player.", "3. Players take turns placing their symbol on an open intersection on the board.", "4. The player that manages to create five in a row first wins the round.", "   The row can be horizontal, vertical, or diagonal as long as it is continuous." };
//
//	GotoXY(32, 8);
//	COORD currentPos = GetConsoleCursorPosition();
//	int width = 100, height = 20; // ┏━┓ -> width = 1;
//
//	_setmode(_fileno(stdout), _O_U16TEXT);
//
//	GotoXY(currentPos.X, currentPos.Y); // top left
//	wcout << L"┏";
//	for (int i = 1; i <= width; i++) { // print column
//		GotoXY(currentPos.X + i, currentPos.Y);
//		wcout << L"━";
//		GotoXY(currentPos.X + i, currentPos.Y + 2);
//		wcout << L"━";
//		GotoXY(currentPos.X + i, currentPos.Y + 10);
//		wcout << L"━";
//		GotoXY(currentPos.X + i, currentPos.Y + 12);
//		wcout << L"━";
//		GotoXY(currentPos.X + i, currentPos.Y + height + 1);
//		wcout << L"━";
//	}
//	GotoXY(currentPos.X + width + 1, currentPos.Y); // top right
//	wcout << L"┓";
//	for (int i = 1; i <= height; i++) { // print column
//		GotoXY(currentPos.X, currentPos.Y + i);
//		wcout << L"┃";
//		GotoXY(currentPos.X + width + 1, currentPos.Y + i);
//		wcout << L"┃";
//	}
//	for (int i = 1; i <= height / 2 - 3; i++) {
//		GotoXY(currentPos.X + 28, currentPos.Y + 2 + i);
//		wcout << L"┃";
//	}
//	GotoXY(currentPos.X, currentPos.Y + height + 1); // bottom left
//	wcout << L"┗";
//	GotoXY(currentPos.X + width + 1, currentPos.Y + height + 1); // bottom left
//	wcout << L"┛";
//
//	_setmode(_fileno(stdout), _O_TEXT);
//	/*
//	Player Movement:
//	'W': Go up
//	'S': Go down
//	'A': Go left
//	'D': Go right
//	'Enter': Mark
//
//	Command Key:
//	'P': Save game
//	'Z': Undo the last move
//	'Esc': Halt the game
//
//	1. Caro is played using the Xand O symbol.
//	2. Two players play against each other on 15 x 15 or 19 x 19 grid.
//	3. The player representing the “X” symbol gets to take the first turn followed by the player representing the “O” symbol.
//	4. Players take turns placing their symbol on an open intersetion on the board.
//	5. The player that manages to create five in _POINT row first wins the round.The row can be horizontal, vertical, or diagonal as long as it is unbroken.
//	6. Once _POINT player places _POINT symbol on the board, the symbol cannot be moved or changed.*/
//	for (int i = 0; i < sizeof(helpText) / sizeof(helpText[0]); i++) {
//		GotoXY(helpTextCoor.X, helpTextCoor.Y + i);
//		cout << helpText[i];
//	}
//	GotoXY(helpTextCoor.X + 30, helpTextCoor.Y);
//	cout << "Command Key:";
//	GotoXY(helpTextCoor.X + 30, helpTextCoor.Y + 1);
//	cout << "'P': Save game";
//	GotoXY(helpTextCoor.X + 30, helpTextCoor.Y + 2);
//	cout << "'Y': Save game";
//	GotoXY(helpTextCoor.X + 30, helpTextCoor.Y + 3);
//	cout << "'Z': Undo the last move";
//	GotoXY(helpTextCoor.X + 30, helpTextCoor.Y + 4);
//	cout << "'Esc': Halt the game/ Return to the previous scene of the game";
//	GotoXY(helpTextCoor.X + 37, helpTextCoor.Y - 2);
//	cout << "Feature and Command Keys";
//	GotoXY(helpTextCoor.X + 45, helpTextCoor.Y + 8);
//	cout << "Rules";
//
//	bool _checkNotEnter = true;
//	while (_checkNotEnter)
//	{
//		if (_kbhit())
//		{
//			switch (_getch())
//			{
//			case 13: //Enter ASCII value of enter
//				_checkNotEnter = false;
//				break;
//			case 27:
//				_checkNotEnter = false;
//			default:
//				break;
//			};
//		}
//	}
//
//	SceneHandle("MAIN MENU");
//}

void StartAbout() {
	//set to UTF16 text cout

	//DrawObject("Background");//
	system("cls");
	GabageCollect();
	_setmode(_fileno(stdout), _O_U16TEXT);
	GotoXY(62, 1);


	wcout << L" █████╗ ██████╗  ██████╗ ██╗   ██╗████████╗\n"; GotoXY(62, 2);
	wcout << L"██╔══██╗██╔══██╗██╔═══██╗██║   ██║╚══██╔══╝\n"; GotoXY(62, 3);
	wcout << L"███████║██████╔╝██║   ██║██║   ██║   ██║   \n"; GotoXY(62, 4);
	wcout << L"██╔══██║██╔══██╗██║   ██║██║   ██║   ██║   \n"; GotoXY(62, 5);
	wcout << L"██║  ██║██████╔╝╚██████╔╝╚██████╔╝   ██║   \n"; GotoXY(62, 6);
	wcout << L"╚═╝  ╚═╝╚═════╝  ╚═════╝  ╚═════╝    ╚═╝   \n"; 

	//DrawObject("Border");//
	//DrawObject("Text_Border", 62, 104);
	//DrawObject("CornerEsc");
	//DrawObject("About_Logo");
	
	//const wchar_t* avtStorage[5] = { L"      ▄▀ ", L" █▀▀▀█▀█ ", L"  ▀▄░▄▀  ", L"    █    ", L"  ▄▄█▄▄  " };
	//const wchar_t* mainAvtStorage[5] = { L"▄   ▄   ▄", L"█  █▀█  █", L"█▀▀▀█▀▀▀█", L"█ ▀▄ ▄▀ █", L"▀▀▄▄▄▄▄▀▀" };
	COORD tableCoord;
	tableCoord.X = 56;
	tableCoord.Y = 10;

	int i = 0, offset = 4;
	GotoXY(tableCoord.X - 3, tableCoord.Y);
	wcout << L"Welcome to the Gomoku Game - made by G6 - 25CTT3" << endl;
	i = i + 2;

	GotoXY(tableCoord.X, tableCoord.Y + i);
	/*for (int j = 0; j < 5; j++) {
		GotoXY(tableCoord.X - 10, tableCoord.Y + i + j);
		wcout << mainAvtStorage[j];
	} */
	GotoXY(tableCoord.X, tableCoord.Y + i + 2);
	wcout << L"Nguyễn Minh Nhật \t chàng gay lực lưỡng" << endl;
	i += offset;


	GotoXY(tableCoord.X, tableCoord.Y + i);
	/*for (int j = 0; j < 5; j++) {
		GotoXY(tableCoord.X - 10, tableCoord.Y + i + j);
		wcout << avtStorage[j];
	}*/
	GotoXY(tableCoord.X, tableCoord.Y + i + 2);
	wcout << L"Nguyễn Hoàng Huy\t nàng gay may mắn" << endl;
	i += offset;


	GotoXY(tableCoord.X, tableCoord.Y + i);
	/*for (int j = 0; j < 5; j++) {
		GotoXY(tableCoord.X - 10, tableCoord.Y + i + j);
		wcout << avtStorage[j];
	}*/
	GotoXY(tableCoord.X, tableCoord.Y + i + 2);
	wcout << L"Vũ Thanh Phong \t Cu te ba que" << endl;
	i += offset;

	GotoXY(tableCoord.X, tableCoord.Y + i);
	/*for (int j = 0; j < 5; j++) {
		GotoXY(tableCoord.X - 10, tableCoord.Y + i + j);
		wcout << avtStorage[j];
	}*/
	GotoXY(tableCoord.X, tableCoord.Y + i + 2);
	wcout << L"Đỗ Lê Nhật Quang \t Nhí nhảnh ỏng ẹo" << endl;
	i += offset;
	GotoXY(tableCoord.X, tableCoord.Y + i + 2);
	wcout << L"Nguyễn Phú Quang \t thợ sửa ống nước" << endl;
	i += offset;
	GotoXY(tableCoord.X, tableCoord.Y + i + 2);
	wcout << L"Nguyễn Vũ Nhật Quang\t tk gay" << endl;
	i += offset;

	bool _checkNotEnter = true;
	while (_checkNotEnter)
	{
		if (_kbhit())
		{
			switch (_getch())
			{
			case 13: //Enter ASCII value of enter
				_checkNotEnter = false;
				break;
			case 27:
				_checkNotEnter = false;
			default:
				break;
			};
		}
	}
	_setmode(_fileno(stdout), _O_TEXT);//set to UTF16 text cout
	//SceneHandle("MAIN MENU");
}
