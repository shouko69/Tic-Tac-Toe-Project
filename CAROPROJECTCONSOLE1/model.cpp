#include "Model.h"
#include "View.h"
#include "GameState.h"
#include <iostream>
#include <vector>
#include <algorithm> // Để dùng max, min
#include <climits>   // Để dùng LLONG_MAX, LLONG_MIN
#include <cmath>     // Để dùng abs()

int _X = 0;
int _Y = 0;
int _COMMAND;
char _player1_name[MAX_NAME_LEN];
char _player2_name[MAX_NAME_LEN];
char _bot_name[MAX_NAME_LEN] = "MINIMAX AI";
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

// AI MINIMAX
// --- PHẦN KHAI BÁO CONST VÀ HÀM PHỤ TRỢ ---

// ==========================================
// 1. KHAI BÁO HẰNG SỐ CẤU HÌNH (CONSTANTS)
// ==========================================
// Nếu BOARD_SIZE đã có ở file .h thì không cần dòng này
// const int BOARD_SIZE = 12; 

// Điểm số trọng số cho AI (Càng cao càng ưu tiên)
const long SCORE_WIN = 100000000;
const long SCORE_BLOCK = 20000000; // Ưu tiên chặn rất cao
const long SCORE_4 = 10000000; // Live 4
const long SCORE_3 = 50000;    // Live 3
const long SCORE_2 = 1000;

// ==========================================
// 2. KHAI BÁO CẤU TRÚC DỮ LIỆU (STRUCTS)
// ==========================================

// Struct dùng nội bộ cho AI để lưu tọa độ


// Struct kết quả trả về của hàm FindBestMove
// (Nếu struct này đã có bên file view/main thì bỏ qua hoặc di chuyển vào file .h)
// Hàm Minimax: Trả về điểm số tốt nhất có thể đạt được từ trạng thái hiện tại
// depth: Độ sâu muốn tính tiếp
// alpha, beta: Giá trị để cắt tỉa nhánh
// isMaximizing: true nếu là lượt AI (cần điểm cao), false nếu là lượt Người (cần điểm thấp)
// Hàm lấy danh sách các nước đi khả thi
// Chiến thuật: Chỉ lấy các ô trống nằm xung quanh (phạm vi 1 ô) các quân cờ đã có trên bàn

std::vector<_POINT> getPossibleMoves() {
    std::vector<_POINT> moves;

    // Mảng đánh dấu để tránh thêm 1 ô nhiều lần vào danh sách
    bool visited[BOARD_SIZE][BOARD_SIZE] = { false };

    bool isEmptyBoard = true;

    // i là Dòng (Row), j là Cột (Col)
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {

            if (_A[i][j].c != 0) { // Nếu ô này đã có quân
                isEmptyBoard = false;

                // Quét 8 ô xung quanh nó (phạm vi 3x3)
                for (int di = -1; di <= 1; di++) {
                    for (int dj = -1; dj <= 1; dj++) {
                        if (di == 0 && dj == 0) continue; // Bỏ qua chính nó

                        int r = i + di; // Row (Dòng)
                        int c = j + dj; // Col (Cột)

                        // Kiểm tra biên bàn cờ
                        if (r >= 0 && r < BOARD_SIZE && c >= 0 && c < BOARD_SIZE) {

                            // Nếu là ô trống và CHƯA được thêm vào danh sách
                            if (_A[r][c].c == 0 && !visited[r][c]) {
                                _POINT p;
                                p.x = c; // <--- QUAN TRỌNG: x là Cột (col)
                                p.y = r; // <--- QUAN TRỌNG: y là Dòng (row)
                                p.c = 0; // 0 là chưa có quân

                                moves.push_back(p);
                                visited[r][c] = true; // Đánh dấu đã lấy
                            }
                        }
                    }
                }
            }
        }
    }

    // Trường hợp đặc biệt: Nếu bàn cờ chưa có ai đánh
    if (isEmptyBoard) {
        _POINT p;
        p.x = BOARD_SIZE / 2; // Giữa cột
        p.y = BOARD_SIZE / 2; // Giữa dòng
        p.c = 0;
        moves.push_back(p);
    }

    return moves;
}
long minimax(int depth, long alpha, long beta, bool isMaximizing) {

    // 1. KIỂM TRA ĐIỀU KIỆN DỪNG
    long currentScore = evaluateBoard();

    // Nếu điểm số quá lớn (AI thắng) hoặc quá nhỏ (Người thắng) -> Dừng ngay
    if (abs(currentScore) > 70000000) {
        return currentScore;
    }

    // Nếu hết độ sâu tính toán
    if (depth == 0) {
        return currentScore;
    }

    // 2. LẤY CÁC NƯỚC ĐI KHẢ THI
    std::vector<_POINT> moves = getPossibleMoves();

    if (moves.empty()) return 0;

    if (isMaximizing) {
        // --- LƯỢT CỦA AI (Tìm MAX) ---
        long maxEval = -LLONG_MAX;

        for (auto p : moves) {
            // SỬA: p.y là dòng, p.x là cột => _A[p.y][p.x]
            _A[p.y][p.x].c = -1; // AI đánh thử

            long eval = minimax(depth - 1, alpha, beta, false);

            _A[p.y][p.x].c = 0; // HOÀN TÁC (Backtrack)

            maxEval = (std::max)(maxEval, eval);
            alpha = (std::max)(alpha, eval);

            if (beta <= alpha) break; // Cắt tỉa
        }
        return maxEval;
    }
    else {
        // --- LƯỢT CỦA NGƯỜI (Tìm MIN) ---
        long minEval = LLONG_MAX;

        for (auto p : moves) {
            // SỬA: p.y là dòng, p.x là cột => _A[p.y][p.x]
            _A[p.y][p.x].c = 1; // Người đánh thử

            long eval = minimax(depth - 1, alpha, beta, true);

            _A[p.y][p.x].c = 0; // HOÀN TÁC (Backtrack)

            minEval = (std::min)(minEval, eval);
            beta = (std::min)(beta, eval);

            if (beta <= alpha) break; // Cắt tỉa
        }
        return minEval;
    }
}

// Hàm tính điểm cho một chuỗi quân cờ cụ thể
// count: số lượng quân liên tiếp
// openEnds: số lượng đầu trống (0, 1 hoặc 2)
// turn: true nếu là lượt AI, false nếu là lượt Người
long getPoint(int count, int openEnds, bool currentTurn) {
    if (count >= 5) return 100000000; // Thắng ngay lập tức

    if (count == 4) {
        if (openEnds == 2) return 10000000; // Live 4: Không thể đỡ
        if (openEnds == 1) return 1000000;  // Dead 4: Phải đỡ ngay
    }

    if (count == 3) {
        if (openEnds == 2) return 1000000; // Live 3: Nguy hiểm ngang Dead 4
        if (openEnds == 1) return 10000;   // Dead 3
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

// Hàm đánh giá một hướng cụ thể (dx, dy) từ vị trí (r, c)
// Trả về điểm số của AI trừ đi điểm số của Người
// Hàm đánh giá một chuỗi quân cờ
// dRow: Thay đổi về dòng (0, 1 hoặc -1)
// dCol: Thay đổi về cột (0, 1 hoặc -1)
long evaluateLine(int r, int c, int dRow, int dCol) {
    long score = 0;

    // 1. Kiểm tra vị trí KHỞI ĐẦU để tránh tính trùng
    int prevR = r - dRow;
    int prevC = c - dCol;

    // Nếu ô trước đó cùng màu -> Bỏ qua (vì đã tính chuỗi này rồi)
    if (prevR >= 0 && prevR < BOARD_SIZE && prevC >= 0 && prevC < BOARD_SIZE) {
        if (_A[prevR][prevC].c == _A[r][c].c) return 0;
    }

    // 2. Đếm số lượng quân
    int count = 0;
    int currR = r;
    int currC = c;
    int myColor = _A[r][c].c;

    while (currR >= 0 && currR < BOARD_SIZE && currC >= 0 && currC < BOARD_SIZE && _A[currR][currC].c == myColor) {
        count++;
        currR += dRow;
        currC += dCol;
    }

    // 3. Kiểm tra 2 đầu (Open Ends)
    int openEnds = 0;

    // Đầu trước
    if (prevR >= 0 && prevR < BOARD_SIZE && prevC >= 0 && prevC < BOARD_SIZE && _A[prevR][prevC].c == 0) {
        openEnds++;
    }

    // Đầu sau (Nơi vòng while dừng lại)
    if (currR >= 0 && currR < BOARD_SIZE && currC >= 0 && currC < BOARD_SIZE && _A[currR][currC].c == 0) {
        openEnds++;
    }

    // 4. Tính điểm
    double defenseWeight = 1.5; // Hệ số phòng thủ
    if (myColor == -1) { // AI
        score += getPoint(count, openEnds, true);
    }
    else { // Người
        score -= getPoint(count, openEnds, false) * defenseWeight;
    }

    return score;
}
// --- HÀM CHÍNH: EVALUATE BOARD ---
long evaluateBoard() {
    long totalScore = 0;

    // i là Dòng (Row), j là Cột (Col)
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (_A[i][j].c != 0) {
                // Thứ tự tham số: (row, col, dRow, dCol)

                // 1. Ngang: Dòng không đổi (0), Cột tăng (1)
                totalScore += evaluateLine(i, j, 0, 1);

                // 2. Dọc: Dòng tăng (1), Cột không đổi (0)
                totalScore += evaluateLine(i, j, 1, 0);

                // 3. Chéo chính: Dòng tăng (1), Cột tăng (1)
                totalScore += evaluateLine(i, j, 1, 1);

                // 4. Chéo phụ: Dòng tăng (1), Cột giảm (-1)
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
    long long bestVal = -LLONG_MAX; // Dùng hằng số nhỏ nhất của kiểu long

    // 1. Lấy danh sách nước đi (Trả về struct _POINT có x, y, c)
    std::vector<_POINT> moves = getPossibleMoves();

    // 2. Duyệt qua từng nước đi
    for (auto p : moves) {
        // --- THỬ NƯỚC ĐI ---
        // Lưu ý: p.y là Dòng, p.x là Cột -> _A[p.y][p.x]
        _A[p.y][p.x].c = -1;

        // --- GỌI MINIMAX ---
        long moveVal = minimax(depth - 1, -LLONG_MAX, LLONG_MAX, false);

        // --- HOÀN TÁC ---
        _A[p.y][p.x].c = 0;

        // --- CHỌN NƯỚC TỐT NHẤT ---
        if (moveVal > bestVal) {
            bestMove.x = p.x; // x là cột
            bestMove.y = p.y; // y là dòng
            bestMove.score = moveVal;
            bestVal = moveVal;
        }
    }

    return bestMove;
}


