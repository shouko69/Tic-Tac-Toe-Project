#include "Model.h"
#include "View.h"
#include "GameState.h"

int _X = 0;
int _Y = 0;
int _COMMAND;
char _player1_name[MAX_NAME_LEN];
char _player2_name[MAX_NAME_LEN];
int _player1_score; // Dùng cho WINS
int _player2_score; // Dùng cho WINS
int _moveCount;     // Dùng cho MOVES
int _currentPlayer;
int _gameWinner;

void ResetData() {
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            _A[i][j].x = 0; // Không cần dùng tới nữa
            _A[i][j].y = 0; // Không cần dùng tới nữa
            _A[i][j].c = 0; // 0: Chưa đánh
        }
    }
    _TURN = true;
    _COMMAND = -1;

    // --- QUAN TRỌNG NHẤT: Đặt về 0 để bắt đầu ở góc bàn cờ ---
    _X = 0;
    _Y = 0;
}

void GabageCollect() {
	// Dọn dẹp tài nguyên nếu có khai báo con trỏ
}
//ktra ban co xem co thg nao win khong

// Kiem tra trang thai ban co
int TestBoard() {

	if (IsFullBoard()) {
		return 0; // hoa
	}
	else {
		if (CheckWin()) {
			return (_TURN == true ? -1 : 1); // -1 true thang
		}
		else {
			return 2; // 2 chua ai thang
		}
	}

}

// ham danh dau vao ma tran ban co
int CheckBoard(int pX, int pY) {
    // 1. Kiểm tra xem index có nằm trong bàn cờ không để tránh lỗi tràn mảng
    if (pX < 0 || pX >= BOARD_SIZE || pY < 0 || pY >= BOARD_SIZE) {
        return 0; // Index không hợp lệ
    }

    // 2. Kiểm tra ô đó đã có người đánh chưa
    if (_A[pY][pX].c == 0) { // Lưu ý: _A[dòng][cột] nên là _A[pY][pX]
        if (_TURN == true) {
            _A[pY][pX].c = -1; // Player 1 (X)
        }
        else {
            _A[pY][pX].c = 1;  // Player 2 (O)
        }
        return _A[pY][pX].c;
    }

    return 0; // Ô này đã bị đánh rồi
}

bool IsFullBoard() {
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (_A[i][j].c == 0)  // 0 nghĩa là ô trống
                return false;
        }
    }
    return true;
}

// Kiem tra co nguoi thang chua
bool CheckWin() {
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            int c = _A[i][j].c;
            if (c == 0) continue; // bỏ qua ô trống

            // --- kiem tra 4 huong ---
            // hàng ngang 
            if (j <= BOARD_SIZE - 5 &&
                c == _A[i][j + 1].c &&
                c == _A[i][j + 2].c &&
                c == _A[i][j + 3].c &&
                c == _A[i][j + 4].c)
                return true;

            // cột dọc 
            if (i <= BOARD_SIZE - 5 &&
                c == _A[i + 1][j].c &&
                c == _A[i + 2][j].c &&
                c == _A[i + 3][j].c &&
                c == _A[i + 4][j].c)
                return true;

            // chéo chính 
            if (i <= BOARD_SIZE - 5 && j <= BOARD_SIZE - 5 &&
                c == _A[i + 1][j + 1].c &&
                c == _A[i + 2][j + 2].c &&
                c == _A[i + 3][j + 3].c &&
                c == _A[i + 4][j + 4].c)
                return true;

            // chéo phụ 
            if (i >= 4 && j <= BOARD_SIZE - 5 &&
                c == _A[i - 1][j + 1].c &&
                c == _A[i - 2][j + 2].c &&
                c == _A[i - 3][j + 3].c &&
                c == _A[i - 4][j + 4].c)
                return true;
        }
    }
    return false;
}




