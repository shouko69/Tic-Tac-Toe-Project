#pragma once

#include "GameState.h"
// lưu độ dài của các chữ cho chức năng 
#define MAX_NAME_LEN 21    // <= 20 ký tự + '\0'
#define MAX_DATE_LEN 20      // "DD-MM-YYYY HH:MM"
#define MAX_TYPE_LEN 12      // "1 Player" / "2 Players"

#define BOARD_SIZE 12 // kich thuoc ban co;
struct _POINT { int x, y, c; }; // x: toa do dong`, y: toa do cot, c: danh dau
extern _POINT _A[BOARD_SIZE][BOARD_SIZE]; //Ma tran ban co
extern bool _TURN; //true la luot nguoi thu nhat, false la luot nguoi thu hai
// Khai báo các biến để lưu tên hai người chơi.
extern int _X; // Tọa độ dòng hiện tại của con trỏ
extern int _Y; // Tọa độ cột hiện tại của con trỏ
extern char _player1_name[MAX_NAME_LEN];
extern char _player2_name[MAX_NAME_LEN];

struct GameStateData {
    // --- Dữ liệu cũ (GIỮ NGUYÊN) ---
    int board[BOARD_SIZE][BOARD_SIZE];
    int currentPlayer;
    int cursorX;
    int cursorY;
    bool loadSuccess;

    // --- THÊM MỚI---
    char p1Name[MAX_NAME_LEN]; // Tên người chơi 1
    char p2Name[MAX_NAME_LEN]; // Tên người chơi 2
    int p1Score;               // Điểm P1
    int p2Score;               // Điểm P2
    int moveCount;             // Số lượt đi
    // ---------------------------------------

    // --- Metadata cũ (GIỮ NGUYÊN) ---
    char name[MAX_NAME_LEN];
    char date[MAX_DATE_LEN];
    char type[MAX_TYPE_LEN];
};
// dung cho chi so trong game PLAY_2P
extern int _player1_score; // Dùng cho WINS
extern int _player2_score; // Dùng cho WINS
extern int _moveCount;     // Dùng cho MOVES
extern int _currentPlayer; // 1 hoặc 2, để xác định lượt đi
extern char _player1_name[];
extern char _player2_name[];
// 0 = chưa có, 1 = P1(X) thắng, 2 = P2(O) thắng, -1 = Hòa
extern int _gameWinner;
void ResetData();
void GabageCollect();
int TestBoard();
int CheckBoard(int pX, int pY);
bool IsFullBoard();
bool CheckWin();

