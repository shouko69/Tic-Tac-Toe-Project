#include "Model.h"
#include "View.h"

int _X = 0;
int _Y = 0;
int _COMMAND;

void ResetData() {
	for (int i = 0; i < BOARD_SIZE; i++) {
		for (int j = 0; j < BOARD_SIZE; j++) {
			_A[i][j].x = 4 * j + LEFT + 2; // Trùng với hoành độ màn hình bàn cờ
			_A[i][j].y = 2 * i + TOP + 1; // Trùng với tung độ màn hình bàn cờ
			_A[i][j].c = 0; // 0 nghĩa là chưa ai đánh dấu, nếu đánh dấu phải theo quy
			//định như sau: -1 là lượt true đánh, 1 là lượt false đ
		}
	}
	_TURN = true; _COMMAND = -1; // Gán lượt và phím mặc định
	_X = _A[0][0].x; _Y = _A[0][0].y; // Thiết lập lại tọa độ hiện hành ban đầu
}

void GabageCollect() {
	// Dọn dẹp tài nguyên nếu có khai báo con trỏ
}


//ktra ban co xem co thg nao win khong
int TestBoard() {
	if (/*Ma tran day*/) {
		return 0; // hoa
	}
	else {
		if (/*Ton tai dieu kien thang theo luat caro*/) {
			return (_TURN == true ? -1 : 1); // -1 true thang
		}
		else {
			return 2; // 2 chua ai thang
		}
	}
}

// ham danh dau vao ma tran ban co
int CheckBoard(int pX, int pY) {
	for (int i = 0; i < BOARD_SIZE; i++) {
		for (int j = 0; j < BOARD_SIZE; j++) {
			if (_A[i][j].x == pX && _A[i][j].y == pY && _A[i][j].c == 0) {
				if (_TURN == true) {
					_A[i][j].c = -1; // neu luot hien hanh la true thi c = -1 
				}
				else {
					_A[i][j].c = 1; // Nếu lượt hiện hành là false thì c = 1
				}
				return _A[i][j].c;
			}
		}
	}
	return 0;
}