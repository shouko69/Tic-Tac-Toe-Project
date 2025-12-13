#include "Model.h"
#include "View.h"
#include "GameState.h"
#include <iostream>
#include <vector>
#include <algorithm> 
#include <climits>   
#include <cmath>     

int _X = 0;
int _Y = 0;
int _COMMAND;
_POINT _A[BOARD_SIZE][BOARD_SIZE];
bool _TURN;
char _player1_name[MAX_NAME_LEN];
char _player2_name[MAX_NAME_LEN];
char _bot_name[MAX_NAME_LEN] = "MINIMAX AI";
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
// AI MINIMAX
const long SCORE_WIN = 100000000;
const long SCORE_BLOCK = 20000000; // Ưu tiên chặn rất cao
const long SCORE_4 = 10000000; // Live 4
const long SCORE_3 = 50000;    // Live 3
const long SCORE_2 = 1000;
std::vector<_POINT> getPossibleMoves() {
    std::vector<_POINT> moves;
    bool visited[BOARD_SIZE][BOARD_SIZE] = { false };

    bool isEmptyBoard = true;
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {

            if (_A[i][j].c != 0) { 
                isEmptyBoard = false;
                for (int di = -1; di <= 1; di++) {
                    for (int dj = -1; dj <= 1; dj++) {
                        if (di == 0 && dj == 0) continue; 

                        int r = i + di;
                        int c = j + dj; 
                        if (r >= 0 && r < BOARD_SIZE && c >= 0 && c < BOARD_SIZE) {
                            if (_A[r][c].c == 0 && !visited[r][c]) {
                                _POINT p;
                                p.x = c; 
                                p.y = r; 
                                p.c = 0; 
                                moves.push_back(p);
                                visited[r][c] = true; 
                            }
                        }
                    }
                }
            }
        }
    }
    if (isEmptyBoard) {
        _POINT p;
        p.x = BOARD_SIZE / 2; 
        p.y = BOARD_SIZE / 2; 
        p.c = 0;
        moves.push_back(p);
    }
    return moves;
}
long minimax(int depth, long alpha, long beta, bool isMaximizing) {
    long currentScore = evaluateBoard();
    if (abs(currentScore) > 70000000) {
        return currentScore;
    }
    if (depth == 0) {
        return currentScore;
    }
    std::vector<_POINT> moves = getPossibleMoves();
    if (moves.empty()) return 0;
    if (isMaximizing) {
        long maxEval = -LLONG_MAX;

        for (auto p : moves) {

            _A[p.y][p.x].c = -1; 

            long eval = minimax(depth - 1, alpha, beta, false);

            _A[p.y][p.x].c = 0; 

            maxEval = (std::max)(maxEval, eval);
            alpha = (std::max)(alpha, eval);

            if (beta <= alpha) break; // Cắt tỉa
        }
        return maxEval;
    }
    else {
        long minEval = LLONG_MAX;

        for (auto p : moves) {
            _A[p.y][p.x].c = 1; 

            long eval = minimax(depth - 1, alpha, beta, true);

            _A[p.y][p.x].c = 0; 

            minEval = (std::min)(minEval, eval);
            beta = (std::min)(beta, eval);

            if (beta <= alpha) break; // Cắt tỉa
        }
        return minEval;
    }
}

long getPoint(int count, int openEnds, bool currentTurn) {
    if (count >= 5) return 100000000; 

    if (count == 4) {
        if (openEnds == 2) return 10000000; 
        if (openEnds == 1) return 1000000; 
    }

    if (count == 3) {
        if (openEnds == 2) return 1000000; 
        if (openEnds == 1) return 10000;
    }

    if (count == 2) {
        if (openEnds == 2) return 1000;
        if (openEnds == 1) return 100;
    }

    if (count == 1) {
        if (openEnds == 2) return 10;
        if (openEnds == 1) return 1;
    }

    return 0;
}
long evaluateLine(int r, int c, int dRow, int dCol) {
    long score = 0;
    int prevR = r - dRow;
    int prevC = c - dCol;
    if (prevR >= 0 && prevR < BOARD_SIZE && prevC >= 0 && prevC < BOARD_SIZE) {
        if (_A[prevR][prevC].c == _A[r][c].c) return 0;
    }
    int count = 0;
    int currR = r;
    int currC = c;
    int myColor = _A[r][c].c;

    while (currR >= 0 && currR < BOARD_SIZE && currC >= 0 && currC < BOARD_SIZE && _A[currR][currC].c == myColor) {
        count++;
        currR += dRow;
        currC += dCol;
    }
    int openEnds = 0;
    if (prevR >= 0 && prevR < BOARD_SIZE && prevC >= 0 && prevC < BOARD_SIZE && _A[prevR][prevC].c == 0) {
        openEnds++;
    }
    if (currR >= 0 && currR < BOARD_SIZE && currC >= 0 && currC < BOARD_SIZE && _A[currR][currC].c == 0) {
        openEnds++;
    }
    double defenseWeight = 1.5; 
    if (myColor == -1) { // AI
        score += getPoint(count, openEnds, true);
    }
    else { // Người
        score -= getPoint(count, openEnds, false) * defenseWeight;
    }

    return score;
}
long evaluateBoard() {
    long totalScore = 0;
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (_A[i][j].c != 0) {
                totalScore += evaluateLine(i, j, 0, 1);
                totalScore += evaluateLine(i, j, 1, 0);
                totalScore += evaluateLine(i, j, 1, 1);
                totalScore += evaluateLine(i, j, 1, -1);
            }
        }
    }
    return totalScore;
}
AIMove FindBestMove(int depth) {
    AIMove bestMove;
    bestMove.x = -1;
    bestMove.y = -1;
    bestMove.score = -LLONG_MAX;
    long long bestVal = -LLONG_MAX; 
    std::vector<_POINT> moves = getPossibleMoves();
    for (auto p : moves) {
        _A[p.y][p.x].c = -1;
        long moveVal = minimax(depth - 1, -LLONG_MAX, LLONG_MAX, false);
        _A[p.y][p.x].c = 0;
        if (moveVal > bestVal) {
            bestMove.x = p.x; // x là cột
            bestMove.y = p.y; // y là dòng
            bestMove.score = moveVal;
            bestVal = moveVal;
        }
    }

    return bestMove;
}
