#include "Control.h"
#include "View.h"
#include "Model.h"
#include <stdlib.h>

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