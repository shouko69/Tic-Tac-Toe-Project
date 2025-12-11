#pragma once
#include <vector>
#include <string>

#define ESCAPE_KEY -2

extern int _X;
extern int _Y;
extern int _COMMAND;

void InitConsole();
void SetCursorVisible(bool visible);
void ShowCursorForBoard();
void ShowComingSoonPopup();
std::string GetRandomName();

void StartGame();
void ExitGame();

void MoveRight();
void MoveLeft();
void MoveUp();
void MoveDown();
void ClearInputBuffer();
int HandleMainMenuInput();
int HandlePauseMenuInput();
int HandleNewGameMenuInput();
int HandleGameOverScreen();
bool Handle2PlayerNameInput();

void Handle2PlayerGame();

void HandleNewSaveInput();
bool HandleFileSelection(int mode);
void ShowSaveGameScreen();
bool ShowLoadGameScreen();

void RunIntro();
void RunWinEffect(int winner);
void RunGuide();
void RunAbout();

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

void StartApp();


