#pragma once
#include <vector>
#include <string>
#include "view.h"
extern int _X, _Y; //toa do hien hanh tren ban co
extern int _COMMAND; // bien nhan gia tri phim nguoi dung
#define ESCAPE_KEY -2


void InitConsole();
void StartGame();
void ExitGame();
void MoveRight();
void MoveLeft();
void MoveUp();
void MoveDown();
void SetCursorVisible(bool visible);
void ShowCursorForBoard();
std::string GetRandomName();
int HandleNewGameMenuInput();
bool Handle2PlayerNameInput();
void Handle2PlayerGame();
int HandleGameOverScreen();
int HandleMainMenuInput();
int HandlePauseMenuInput();
// Hàm chính để chạy ứng dụng (thay thế vòng lặp trong main cũ)
void StartApp();
// Khai báo các hàm xử lý từng màn hình (để dùng trong map)
void RunMenuState();
void RunNewGameModeState();
void Run2PlayerNameInputState();
void RunPlayState();
void RunGameOverState();
void RunPauseState();
void RunSettingsState();
void RunSettingsPState();
void RunLoadState();
void RunAboutState();
void RunGuideState();


