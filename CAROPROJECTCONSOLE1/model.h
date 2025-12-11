#pragma once
#include <ctime>
#include "GameState.h"
#include <conio.h>
#include <iostream>
#include "truecolor_utils.h"
#include <cstdlib> 
#include "control.h"

#define MAX_NAME_LEN 21
#define MAX_DATE_LEN 20
#define MAX_TYPE_LEN 12
#define BOARD_SIZE 12

struct _POINT { int x, y, c; };

extern _POINT _A[BOARD_SIZE][BOARD_SIZE];
extern bool _TURN;
extern int _X;
extern int _Y;
extern char _player1_name[MAX_NAME_LEN];
extern char _player2_name[MAX_NAME_LEN];

struct GameStateData {
    int board[BOARD_SIZE][BOARD_SIZE];
    int currentPlayer;
    int cursorX;
    int cursorY;
    bool loadSuccess;
    char p1Name[MAX_NAME_LEN];
    char p2Name[MAX_NAME_LEN];
    int p1Score;
    int p2Score;
    int moveCount;
    int timeLeft;
    int round;
    char name[MAX_NAME_LEN];
    char date[MAX_DATE_LEN];
    char type[MAX_TYPE_LEN];
};

extern int _player1_score;
extern int _player2_score;
extern int _moveCount;
extern int _currentPlayer;
extern int _turnTimer;
extern int _round;
extern time_t _lastTimeCheck;
extern int _gameWinner;

void ResetData();
void GabageCollect();
int TestBoard();
int CheckBoard(int pX, int pY);
bool IsFullBoard();
bool CheckWin();


