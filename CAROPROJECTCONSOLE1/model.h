#pragma once

#define BOARD_SIZE 12 // kich thuoc ban co;
struct _POINT { int x, y, c; }; // x: toa do dong`, y: toa do cot, c: danh dau
extern _POINT _A[BOARD_SIZE][BOARD_SIZE]; //Ma tran ban co
extern bool _TURN; //true la luot nguoi thu nhat, false la luot nguoi thu hai


void ResetData();
void GabageCollect();
int TestBoard();
int CheckBoard(int pX, int pY);
bool IsFullBoard();
bool CheckWin();

