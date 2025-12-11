#include "Model.h"
#include "View.h"
#include "GameState.h"

int _X = 0;
int _Y = 0;
int _COMMAND;
_POINT _A[BOARD_SIZE][BOARD_SIZE];
bool _TURN;
char _player1_name[MAX_NAME_LEN];
char _player2_name[MAX_NAME_LEN];
int _player1_score;
int _player2_score;
int _moveCount;
int _currentPlayer;
int _gameWinner;
int _turnTimer = 120;
time_t _lastTimeCheck = 0;
int _round = 1;

void ResetData() {
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            _A[i][j].x = 0;
            _A[i][j].y = 0;
            _A[i][j].c = 0;
        }
    }
    _TURN = true;
    _COMMAND = -1;
    _X = 0;
    _Y = 0;
}

void GabageCollect() {
}

int TestBoard() {
    if (IsFullBoard()) {
        return 0;
    }
    else {
        if (CheckWin()) {
            return (_TURN == true ? -1 : 1);
        }
        else {
            return 2;
        }
    }
}

int CheckBoard(int pX, int pY) {
    if (pX < 0 || pX >= BOARD_SIZE || pY < 0 || pY >= BOARD_SIZE) {
        return 0;
    }

    if (_A[pY][pX].c == 0) {
        if (_TURN == true) {
            _A[pY][pX].c = -1;
        }
        else {
            _A[pY][pX].c = 1;
        }
        return _A[pY][pX].c;
    }
    return 0;
}

bool IsFullBoard() {
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (_A[i][j].c == 0)
                return false;
        }
    }
    return true;
}

bool CheckWin() {
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            int c = _A[i][j].c;
            if (c == 0) continue;

            if (j <= BOARD_SIZE - 5 &&
                c == _A[i][j + 1].c &&
                c == _A[i][j + 2].c &&
                c == _A[i][j + 3].c &&
                c == _A[i][j + 4].c)
                return true;

            if (i <= BOARD_SIZE - 5 &&
                c == _A[i + 1][j].c &&
                c == _A[i + 2][j].c &&
                c == _A[i + 3][j].c &&
                c == _A[i + 4][j].c)
                return true;

            if (i <= BOARD_SIZE - 5 && j <= BOARD_SIZE - 5 &&
                c == _A[i + 1][j + 1].c &&
                c == _A[i + 2][j + 2].c &&
                c == _A[i + 3][j + 3].c &&
                c == _A[i + 4][j + 4].c)
                return true;

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