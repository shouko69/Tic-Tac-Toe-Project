#include "Control.h"
#include "View.h"
#include "Model.h"
#include "GameState.h"
#include "GameAssets.h"
#include "truecolor_utils.h"
#include "SaveLoad.h"
#include "SaveLoadUI.h"
#include <Windows.h>
#include <conio.h>
#include <string>
#include <vector>
#include <random>
#include <ctime>
#include <ctype.h>
#include <map>

using namespace std;

extern const int TOTAL_ITEMS;
extern const int TOTAL_PAUSE_ITEMS;
extern const int TOTAL_NEW_GAME_OPTIONS;

GameState currentState = MENU;

typedef void (*StateFunc)();

void ClearInputBuffer() {
    while (_kbhit()) {
        _getch();
    }
}

void InitConsole() {
    system("chcp 65001");
    SetCursorVisible(false);
}

void SetCursorVisible(bool visible) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;
    if (GetConsoleCursorInfo(hConsole, &cursorInfo)) {
        cursorInfo.bVisible = visible ? TRUE : FALSE;
        SetConsoleCursorInfo(hConsole, &cursorInfo);
    }
}

void ShowCursorForBoard() {
    SetCursorVisible(true);
}

void ShowComingSoonPopup() {
    ClearScreenWithColor(30, 30, 30);
    SetColorRGB(255, 255, 255);
    GotoXY(65, 22); cout << "==================================";
    GotoXY(65, 23); cout << "    TINH NANG DANG PHAT TRIEN     ";
    GotoXY(65, 24); cout << "          (COMING SOON)           ";
    GotoXY(65, 25); cout << "==================================";
    GotoXY(65, 28); cout << ">> Nhan phim bat ky de quay lai <<";
    ClearInputBuffer();
    _getch();
}

std::string GetRandomName() {
    static const std::vector<std::string> RANDOM_NAMES = {
        "ShadowStriker", "CyberNinja", "PixelAvenger", "GigaWizard",
        "QuantumLeaper", "VoidWalker", "ChronoMaster", "Serpent",
        "Phoenix", "Titan", "RogueAI", "Helios", "Nemesis",
        "Vortex", "GalacticHero", "NightRider", "Blaze"
    };
    static std::mt19937 rng(static_cast<unsigned int>(time(0)));
    std::uniform_int_distribution<int> dist(0, RANDOM_NAMES.size() - 1);
    return RANDOM_NAMES[dist(rng)];
}

void StartGame() {
    ClearScreenWithColor(97, 100, 151);
    SetColorRGB(255, 255, 255);
    ResetData();

    _currentPlayer = 1;
    _moveCount = 0;
    _gameWinner = 0;
    _TURN = true;
    _X = 0; _Y = 0;
    _turnTimer = 120;
    _lastTimeCheck = time(0);

    DrawGameUI_2P();
    GotoBoard(_X, _Y);
    SetCursorVisible(true);
}

void ExitGame() {
    ResetColor();
    ClearScreenWithColor(0, 0, 0);
    GabageCollect();
}

void MoveRight() { if (_X < BOARD_SIZE - 1) { _X++; GotoBoard(_X, _Y); } }
void MoveLeft() { if (_X > 0) { _X--; GotoBoard(_X, _Y); } }
void MoveDown() { if (_Y < BOARD_SIZE - 1) { _Y++; GotoBoard(_X, _Y); } }
void MoveUp() { if (_Y > 0) { _Y--; GotoBoard(_X, _Y); } }

int HandleMainMenuInput() {
    ClearInputBuffer();
    int selected = 0, old_selected = 0;
    DrawFullMenu(selected);
    while (true) {
        int key = toupper(_getch());
        old_selected = selected;
        if (key == 'W') selected = (selected - 1 + TOTAL_ITEMS) % TOTAL_ITEMS;
        else if (key == 'S') selected = (selected + 1) % TOTAL_ITEMS;
        else if (key == 13) return selected;

        if (selected != old_selected) {
            DrawMenuItem(old_selected, false);
            DrawMenuItem(selected, true);
        }
    }
}

int HandlePauseMenuInput() {
    ClearInputBuffer();
    int selected = 0, old_selected = 0;
    DrawFullPauseMenu(selected);
    while (true) {
        int key = toupper(_getch());
        old_selected = selected;
        if (key == 'W') selected = (selected - 1 + TOTAL_PAUSE_ITEMS) % TOTAL_PAUSE_ITEMS;
        else if (key == 'S') selected = (selected + 1) % TOTAL_PAUSE_ITEMS;
        else if (key == 13) return selected;
        else if (key == 27) return 0;

        if (selected != old_selected) {
            DrawPauseMenuItem(old_selected, false);
            DrawPauseMenuItem(selected, true);
        }
    }
}

int HandleNewGameMenuInput() {
    ClearInputBuffer();
    int selected = 1, old_selected = 1;
    DrawFullNewGameMenu(selected);
    while (true) {
        int key = _getch();
        old_selected = selected;
        int upperKey = toupper(key);
        if (upperKey == 'W') selected = (selected - 1 + TOTAL_NEW_GAME_OPTIONS) % TOTAL_NEW_GAME_OPTIONS;
        else if (upperKey == 'S') selected = (selected + 1) % TOTAL_NEW_GAME_OPTIONS;
        else if (key == 13) return selected;
        else if (key == 27) return ESCAPE_KEY;

        if (selected != old_selected) {
            DrawNewGameMenuItem(old_selected, false);
            DrawNewGameMenuItem(selected, true);
        }
    }
}

int HandleGameOverScreen() {
    ClearInputBuffer();
    int selectedOption = 1;
    DrawGameOverScreen(selectedOption, true);
    while (true) {
        int key = _getch();
        int old_selected = selectedOption;
        if (key == 9) selectedOption = (selectedOption + 1) % 3;
        else if (key == 13 || key == 32) return selectedOption;
        else if (key == 27) return 2;

        if (selectedOption != old_selected) {
            DrawGameOverScreen(selectedOption, false);
        }
    }
}

bool Handle2PlayerNameInput() {
    ClearInputBuffer();
    int activeControl = 0;
    char p1_buffer[MAX_NAME_LEN] = "";
    char p2_buffer[MAX_NAME_LEN] = "";
    const int INPUT_P1_X = 51, INPUT_P1_Y = 26;
    const int INPUT_P2_X = 104, INPUT_P2_Y = 26;

    DrawFull2PlayerNameScreen();

    while (true) {
        SetCursorVisible(false);
        Update2PlayerNameScreen(activeControl, p1_buffer, p2_buffer);

        if (activeControl == 0) { GotoXY(INPUT_P1_X + strlen(p1_buffer), INPUT_P1_Y); SetCursorVisible(true); }
        else if (activeControl == 1) { GotoXY(INPUT_P2_X + strlen(p2_buffer), INPUT_P2_Y); SetCursorVisible(true); }
        else SetCursorVisible(false);

        int key = _getch();
        if (key == 27) { SetCursorVisible(false); return false; }
        if (key == 9) { activeControl = (activeControl + 1) % 3; continue; }

        switch (activeControl) {
        case 0:
            if (isprint(key) && strlen(p1_buffer) < MAX_NAME_LEN - 1) {
                size_t len = strlen(p1_buffer); p1_buffer[len] = key; p1_buffer[len + 1] = '\0';
            }
            else if (key == 8 && strlen(p1_buffer) > 0) p1_buffer[strlen(p1_buffer) - 1] = '\0';
            else if (key == 13) {
                if (strlen(p1_buffer) == 0) strcpy_s(p1_buffer, MAX_NAME_LEN, GetRandomName().c_str());
                activeControl = 1;
            }
            break;
        case 1:
            if (isprint(key) && strlen(p2_buffer) < MAX_NAME_LEN - 1) {
                size_t len = strlen(p2_buffer); p2_buffer[len] = key; p2_buffer[len + 1] = '\0';
            }
            else if (key == 8) {
                if (strlen(p2_buffer) > 0) p2_buffer[strlen(p2_buffer) - 1] = '\0';
                else activeControl = 0;
            }
            else if (key == 13) {
                if (strlen(p2_buffer) == 0) strcpy_s(p2_buffer, MAX_NAME_LEN, GetRandomName().c_str());
                if (strlen(p1_buffer) > 0) {
                    strcpy_s(_player1_name, MAX_NAME_LEN, p1_buffer);
                    strcpy_s(_player2_name, MAX_NAME_LEN, p2_buffer);
                    SetCursorVisible(false);
                    return true;
                }
            }
            break;
        case 2:
            if (key == 13) { SetCursorVisible(false); return false; }
            break;
        }
    }
}

void RunIntro() {
    DrawLoadingScreen();
    while (true) {
        // Kiểm tra nếu có phím nhấn
        if (_kbhit()) {
            ClearInputBuffer();
            break;
        }

        DrawPressAnyKeyPrompt(true);
        for (int i = 0; i < 5; i++) {
            if (_kbhit()) break;
            Sleep(100);
        }
        if (_kbhit()) continue; 

        DrawPressAnyKeyPrompt(false);
        for (int i = 0; i < 5; i++) {
            if (_kbhit()) break;
            Sleep(100);
        }
    }
}

void RunWinEffect(int winner) {
    ClearInputBuffer();
    int colorIdx = 0;
    while (true) {
        if (_kbhit()) { _getch(); break; }
        DrawWinEffectFrame(winner, colorIdx);
        colorIdx++;
        Sleep(200);
    }
}

void RunAbout() {
    ClearInputBuffer();
    DrawAboutScreen();
    while (true) {
        if (_kbhit()) {
            int key = _getch();
            if (key == 27 || key == 13) break;
        }
    }
}

void RunGuide() {
    ClearInputBuffer();
    DrawGuideScreen();
    while (true) {
        if (_kbhit()) {
            int key = _getch();
            if (key == 27 || key == 13) break;
        }
        Sleep(50);
    }
}

void Handle2PlayerGame() {
    _lastTimeCheck = time(0);
    GotoBoard(_X, _Y);
    SetCursorVisible(true);

    while (true) {
        while (!_kbhit()) {
            time_t now = time(0);
            if (now != _lastTimeCheck) {
                _turnTimer--;
                _lastTimeCheck = now;
                SetCursorVisible(false);
                DrawTimerOnly();
                GotoBoard(_X, _Y);
                SetCursorVisible(true);
            }
            if (_turnTimer <= 0) {
                _gameWinner = (_currentPlayer == 1) ? 2 : 1;
                SetCursorVisible(false);
                RunWinEffect(_gameWinner);
                currentState = GAME_OVER;
                return;
            }
            Sleep(100);
        }

        int key = _getch();
        if (key == 0 || key == 224) key = _getch();
        else key = toupper(key);

        if (key == 27) { currentState = PAUSE; return; }

        bool validAction = false;
        int oldX = _X, oldY = _Y;

        if (_currentPlayer == 1) {
            if (key == 'W') MoveUp(); else if (key == 'S') MoveDown();
            else if (key == 'A') MoveLeft(); else if (key == 'D') MoveRight();
            else if (key == 'F') validAction = true;
        }
        else {
            if (key == 72) MoveUp(); else if (key == 80) MoveDown();
            else if (key == 75) MoveLeft(); else if (key == 77) MoveRight();
            else if (key == 13) validAction = true;
        }

        if (_X != oldX || _Y != oldY) GotoBoard(_X, _Y);

        if (validAction) {
            if (CheckBoard(_X, _Y) != 0) {
                _moveCount++;
                SetCursorVisible(false);

                int drawX = LEFT + _X * CELL_VISUAL_WIDTH;
                int drawY = TOP + _Y * CELL_VISUAL_HEIGHT;

                if (_currentPlayer == 1) DrawXmini(drawX + 2, drawY + 1);
                else DrawOmini(drawX + 2, drawY + 1);

                int status = TestBoard();
                if (status != 2) {
                    if (status == -1) { _gameWinner = 1; _player1_score++; }
                    else if (status == 1) { _gameWinner = 2; _player2_score++; }
                    else { _gameWinner = 0; }
                    _round++;
                    RunWinEffect(_gameWinner);
                    currentState = GAME_OVER;
                    return;
                }
                else {
                    _turnTimer = 120;
                    _lastTimeCheck = time(0);
                    _currentPlayer = (_currentPlayer == 1) ? 2 : 1;
                    _TURN = !_TURN;
                    UpdateDynamic2P_UI();
                    GotoBoard(_X, _Y);
                    SetCursorVisible(true);
                }
            }
        }
    }
}

void HandleNewSaveInput() {
    ClearInputBuffer();
    std::string saveName = "";
    std::string message = "*Less than 20 characters";
    bool isSuccess = false;

    SetCursorVisible(false);
    DrawNewSaveInputScreen(saveName, message, isSuccess, true);
    SetCursorVisible(true);

    while (true) {
        char ch = _getch();

        SetCursorVisible(false);

        if (ch == 13) {
            if (saveName.empty()) {
                SetCursorVisible(true);
                continue;
            }

            std::string fileNameFull = saveName + ".txt";
            if (FileExists("caro_saves\\" + fileNameFull)) {
                message = "*File's name has existed!";
            }
            else {
                PerformSave(fileNameFull);
                message = "*Save file successfully";
                isSuccess = true;
            }

            DrawNewSaveInputScreen(saveName, message, isSuccess, false);

            if (isSuccess) {
                SetCursorVisible(false);
                _getch();
                return;
            }
        }
        else if (ch == 27) {
            SetCursorVisible(false);
            return;
        }
        else if (ch == 8) {
            if (!saveName.empty()) {
                saveName.pop_back();
                message = "*Less than 20 characters";
                DrawNewSaveInputScreen(saveName, message, isSuccess, false);
            }
        }
        else if (saveName.length() < 20 && (isalnum(ch) || ch == ' ' || ch == '_')) {
            saveName += ch;
            message = "*Less than 20 characters";
            DrawNewSaveInputScreen(saveName, message, isSuccess, false);
        }

        if (!isSuccess) {
            SetCursorVisible(true);
        }
    }
}

bool HandleFileSelection(int mode) {
    ClearInputBuffer();
    std::vector<GameStateData> saves = GetSavedGames();
    if (saves.empty()) {
        DrawFileSelectionScreen(saves, 0, 0, (mode == 0), true);
        const char* msg = "No save files found. Press ESC to return...";
        GotoXY((CONSOLE_WIDTH - strlen(msg)) / 2, CONSOLE_HEIGHT / 2);
        SetColorRGB(255, 255, 255); std::cout << msg;

        while (_getch() != 27);
        return false;
    }

    int selected = 0;
    int old_selected = -1;
    int viewOffset = 0;
    int totalSaves = saves.size();
    int totalOptions = totalSaves + 1;

    DrawFileSelectionScreen(saves, viewOffset, selected, (mode == 0), true);

    while (true) {
        if (selected != old_selected) {
            DrawFileSelectionScreen(saves, viewOffset, selected, (mode == 0), false);
            old_selected = selected;
        }

        char key = toupper(_getch());
        if (key == 27) return false;

        if (key == 'W') selected = (selected - 1 + totalOptions) % totalOptions;
        else if (key == 'S') selected = (selected + 1) % totalOptions;
        else if (key == 13) {
            if (selected == totalSaves) return false;

            if (mode == 0) {
                ApplyLoadedData(saves[selected]);
                return true;
            }
            else {
                std::string filename = std::string(saves[selected].name) + ".txt";
                PerformSave(filename);
                return true;
            }
        }

        if (selected < totalSaves) {
            if (selected < viewOffset) { viewOffset = selected; old_selected = -1; }
            if (selected >= viewOffset + 3) { viewOffset = selected - 3 + 1; old_selected = -1; }
        }
    }
}

void ShowSaveGameScreen() {
    ClearInputBuffer();
    int selected = 0;
    int old_selected = -1;

    DrawSaveMenu(selected, true);

    while (true) {
        if (selected != old_selected) {
            DrawSaveMenu(selected, false);
            old_selected = selected;
        }

        char key = toupper(_getch());
        if (key == 27) { ResetColor(); return; }

        if (key == 'W') selected = (selected - 1 + 3) % 3;
        else if (key == 'S') selected = (selected + 1) % 3;
        else if (key == 13) {
            switch (selected) {
            case 0: HandleNewSaveInput();
                DrawSaveMenu(selected, true);
                old_selected = -1;
                break;
            case 1: HandleFileSelection(1);
                DrawSaveMenu(selected, true);
                old_selected = -1;
                break;
            case 2: ResetColor(); return;
            }
        }
    }
}

bool ShowLoadGameScreen() {
    return HandleFileSelection(0);
}

void RunMenuState() {
    int choice = HandleMainMenuInput();
    switch (choice) {
    case 0: currentState = NEW_GAME_MODE; break;
    case 1: currentState = LOAD; break;
    case 2: currentState = SETTINGS; break;
    case 3: currentState = GUIDE; break;
    case 4: currentState = ABOUT; break;
    case 5: currentState = EXIT; break;
    }
}

void RunNewGameModeState() {
    int choice = HandleNewGameMenuInput();
    switch (choice) {
    case 0: ShowComingSoonPopup(); currentState = MENU; break;
    case 1:
        _player1_score = 0; _player2_score = 0; _round = 1;
        currentState = PLAYER_NAME_INPUT;
        break;
    case 2: case ESCAPE_KEY: currentState = MENU; break;
    }
}

void Run2PlayerNameInputState() {
    if (Handle2PlayerNameInput()) {
        StartGame();
        currentState = PLAY_2P;
    }
    else {
        currentState = NEW_GAME_MODE;
    }
}

void RunPlayState() {
    Handle2PlayerGame();
    GotoBoard(_X, _Y);
    Sleep(15);
}

void RunGameOverState() {
    SetCursorVisible(false);
    int choice = HandleGameOverScreen();
    switch (choice) {
    case 0: ShowSaveGameScreen(); break;
    case 1: StartGame(); currentState = PLAY_2P; break;
    case 2: _player1_score = 0; _player2_score = 0; _round = 1; currentState = MENU; break;
    }
}

void RunPauseState() {
    SetCursorVisible(false);
    int pauseChoice = HandlePauseMenuInput();
    switch (pauseChoice) {
    case 0:
        DrawGameUI_2P(); DrawBoard(BOARD_SIZE); RedrawBoardState();
        SetCursorVisible(true); currentState = PLAY_2P; break;
    case 1:
        SetCursorVisible(true); StartGame(); currentState = PLAY_2P; break;
    case 2: ShowSaveGameScreen(); break;
    case 3: RunGuide(); break;
    case 4: RunSettingsPState(); break;
    case 5: currentState = MENU; SetCursorVisible(false); break;
    }
}

void RunSettingsState() { ShowComingSoonPopup(); currentState = MENU; }
void RunSettingsPState() { ShowComingSoonPopup(); currentState = PAUSE; }

void RunGuideState() {
    RunGuide();
    currentState = MENU;
}

void RunAboutState() {
    RunAbout();
    currentState = MENU;
}

void RunLoadState() {
    ResetData();
    if (ShowLoadGameScreen()) {
        ClearScreenWithColor(97, 100, 151);
        SetColorRGB(0, 0, 0);
        DrawGameUI_2P();

        if (TestBoard() != 2) {
            for (int i = 0; i < BOARD_SIZE; i++)
                for (int j = 0; j < BOARD_SIZE; j++) _A[i][j].c = 0;
            _moveCount = 0; _currentPlayer = 1; _TURN = true; _X = 0; _Y = 0;
            UpdateDynamic2P_UI(); RedrawBoardState();
            GotoXY(LEFT, TOP - 2);
            SetColorRGB(255, 255, 0); cout << "Loaded finished game. Starting new round..."; SetColorRGB(0, 0, 0);
        }
        else {
            RedrawBoardState();
        }
        GotoBoard(_X, _Y);
        currentState = PLAY_2P;
        SetCursorVisible(true);
    }
    else {
        currentState = MENU;
    }
}

void StartApp() {
    std::map<GameState, StateFunc> stateMachine;
    stateMachine[MENU] = RunMenuState;
    stateMachine[NEW_GAME_MODE] = RunNewGameModeState;
    stateMachine[PLAYER_NAME_INPUT] = Run2PlayerNameInputState;
    stateMachine[PLAY_2P] = RunPlayState;
    stateMachine[GAME_OVER] = RunGameOverState;
    stateMachine[PAUSE] = RunPauseState;
    stateMachine[SETTINGS] = RunSettingsState;
    stateMachine[LOAD] = RunLoadState;
    stateMachine[ABOUT] = RunAboutState;
    stateMachine[GUIDE] = RunGuideState;

    while (true) {
        if (currentState == EXIT) {
            ExitGame();
            return;
        }
        if (stateMachine.find(currentState) != stateMachine.end()) {
            stateMachine[currentState]();
        }
    }
}