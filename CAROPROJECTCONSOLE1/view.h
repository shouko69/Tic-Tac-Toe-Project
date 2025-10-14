#pragma once

//*****BIENTOANCUC*****

#define LEFT 3 // Toa do trai man hinh ban co
#define TOP 1 // Toa do tren man hinh ban co

// *****HAMKHAIBAO*****
void FixConsoleWindow();
void GotoXY(int x, int y);
void DrawBoard(int pSize);
int ProcessFinish(int pWhoWin);
int AskContinue();

