#include "Control.h"
#include "View.h"
#include "Model.h"
#include "GameState.h"
#include "GameAssets.h"
#include "truecolor_utils.h"

#include <Windows.h>
#include <conio.h>
#include <io.h>
#include <fcntl.h>
#include <string>
#include <string.h>
#include <iostream>
#include <vector>
#include <random>
#include <ctime>
#include <ctype.h>

using namespace std;

// --- CÁC BIẾN TOÀN CỤC TỪ FILE KHÁC ---
extern const int TOTAL_ITEMS;
extern const int TOTAL_PAUSE_ITEMS;
extern const int TOTAL_NEW_GAME_OPTIONS;

// ==================================================================================
// 1. HÀM HỆ THỐNG & TIỆN ÍCH (SYSTEM & UTILS)
// ==================================================================================

void InitConsole() {
    system("chcp 65001"); // Thiết lập mã hóa UTF-8
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

// ==================================================================================
// 2. QUẢN LÝ TRẠNG THÁI GAME (GAME STATE MANAGEMENT)
// ==================================================================================

void StartGame() {
    // Dọn dẹp và reset dữ liệu
    ClearScreenWithColor(89, 79, 175);
    SetColorRGB(255, 255, 255);
    ResetData();

    // Thiết lập thông số game mới
    _currentPlayer = 1;
    _moveCount = 0;
    _gameWinner = 0;
    _TURN = true;
    _X = 0;
    _Y = 0;

    // Vẽ giao diện
    DrawGameUI();
    GotoBoard(_X, _Y);
    SetCursorVisible(true);
}

void ExitGame() {
    ResetColor();
    ClearScreenWithColor(0, 0, 0);
    GabageCollect();
}

// ==================================================================================
// 3. ĐIỀU KHIỂN DI CHUYỂN (MOVEMENT CONTROL)
// ==================================================================================

void MoveRight() {
    if (_X < BOARD_SIZE - 1) {
        _X++;
        GotoBoard(_X, _Y);
    }
}

void MoveLeft() {
    if (_X > 0) {
        _X--;
        GotoBoard(_X, _Y);
    }
}

void MoveDown() {
    if (_Y < BOARD_SIZE - 1) {
        _Y++;
        GotoBoard(_X, _Y);
    }
}

void MoveUp() {
    if (_Y > 0) {
        _Y--;
        GotoBoard(_X, _Y);
    }
}

// ==================================================================================
// 4. XỬ LÝ INPUT MENU (MENU INPUT HANDLING)
// ==================================================================================

int HandleMainMenuInput() {
    int selected = 0;
    int old_selected = 0;
    DrawFullMenu(selected);

    while (true) {
        int key = toupper(_getch());
        old_selected = selected;

        if (key == 'W') {
            selected = (selected - 1 + TOTAL_ITEMS) % TOTAL_ITEMS;
        }
        else if (key == 'S') {
            selected = (selected + 1) % TOTAL_ITEMS;
        }
        else if (key == 13) { // Enter
            return selected;
        }

        if (selected != old_selected) {
            DrawMenuItem(old_selected, false);
            DrawMenuItem(selected, true);
        }
    }
}

int HandlePauseMenuInput() {
    int selected = 0;
    int old_selected = 0;
    DrawFullPauseMenu(selected);

    while (true) {
        int key = toupper(_getch());
        old_selected = selected;

        if (key == 'W') {
            selected = (selected - 1 + TOTAL_PAUSE_ITEMS) % TOTAL_PAUSE_ITEMS;
        }
        else if (key == 'S') {
            selected = (selected + 1) % TOTAL_PAUSE_ITEMS;
        }
        else if (key == 13) { // Enter
            return selected;
        }
        else if (key == 27) { // ESC -> Resume
            return 0;
        }

        if (selected != old_selected) {
            DrawPauseMenuItem(old_selected, false);
            DrawPauseMenuItem(selected, true);
        }
    }
}

int HandleNewGameMenuInput() {
    int selected = 1;
    int old_selected = selected;
    DrawFullNewGameMenu(selected);

    while (true) {
        int key = _getch();
        old_selected = selected;
        int upperKey = toupper(key);

        if (upperKey == 'W') {
            selected = (selected - 1 + TOTAL_NEW_GAME_OPTIONS) % TOTAL_NEW_GAME_OPTIONS;
        }
        else if (upperKey == 'S') {
            selected = (selected + 1) % TOTAL_NEW_GAME_OPTIONS;
        }
        else if (key == 224) {
            key = _getch();
            if (key == 72) selected = (selected - 1 + TOTAL_NEW_GAME_OPTIONS) % TOTAL_NEW_GAME_OPTIONS;
            if (key == 80) selected = (selected + 1) % TOTAL_NEW_GAME_OPTIONS;
        }
        else if (key == 13) { // Enter
            return selected; // Chặn chọn "1 Player" nếu chưa làm
        }
        else if (key == 27) { // ESC
            return ESCAPE_KEY;
        }

        if (selected != old_selected) {
            DrawNewGameMenuItem(old_selected, false);
            DrawNewGameMenuItem(selected, true);
        }
    }
}

// ==================================================================================
// 5. XỬ LÝ GAMEPLAY & NHẬP LIỆU (GAMEPLAY INPUT HANDLING)
// ==================================================================================

bool HandlePlayerNameInput() {
    int activeControl = 0;
    char p1_buffer[MAX_NAME_LEN] = "";
    char p2_buffer[MAX_NAME_LEN] = "";
    DrawFullPlayerNameScreen();

    while (true) {
        UpdatePlayerNameScreen(activeControl, p1_buffer, p2_buffer);
        int key = _getch();

        if (key == 27) return false; // ESC
        if (key == 9) { // TAB
            activeControl = (activeControl + 1) % 3;
            continue;
        }

        switch (activeControl) {
        case 0: // Player 1
            if (isprint(key) && strlen(p1_buffer) < MAX_NAME_LEN - 1) {
                size_t len = strlen(p1_buffer);
                p1_buffer[len] = key; p1_buffer[len + 1] = '\0';
            }
            else if (key == 8 && strlen(p1_buffer) > 0) { // Backspace
                p1_buffer[strlen(p1_buffer) - 1] = '\0';
            }
            else if (key == 13) { // Enter
                if (strlen(p1_buffer) == 0) strcpy_s(p1_buffer, MAX_NAME_LEN, GetRandomName().c_str());
                activeControl = 1;
            }
            break;

        case 1: // Player 2
            if (isprint(key) && strlen(p2_buffer) < MAX_NAME_LEN - 1) {
                size_t len = strlen(p2_buffer);
                p2_buffer[len] = key; p2_buffer[len + 1] = '\0';
            }
            else if (key == 8) { // Backspace
                if (strlen(p2_buffer) > 0) p2_buffer[strlen(p2_buffer) - 1] = '\0';
                else activeControl = 0;
            }
            else if (key == 13) { // Enter
                if (strlen(p2_buffer) == 0) strcpy_s(p2_buffer, MAX_NAME_LEN, GetRandomName().c_str());
                if (strlen(p1_buffer) > 0) {
                    strcpy_s(_player1_name, MAX_NAME_LEN, p1_buffer);
                    strcpy_s(_player2_name, MAX_NAME_LEN, p2_buffer);
                    return true;
                }
            }
            break;

        case 2: // Nút BACK
            if (key == 13) return false;
            break;
        }
    }
}

void Handle2PlayerGame() {
    GotoBoard(_X, _Y);
    int key = _getch();

    if (key == 0 || key == 224) key = _getch();
    else key = toupper(key);

    if (key == 27) { // ESC -> Pause
        currentState = PAUSE;
        return;
    }

    bool validAction = false;

    // --- PHÂN QUYỀN NGƯỜI CHƠI ---
    if (_currentPlayer == 1) {
        if (key == 'W') MoveUp();
        else if (key == 'S') MoveDown();
        else if (key == 'A') MoveLeft();
        else if (key == 'D') MoveRight();
        else if (key == 'F' || key == 32) validAction = true; // Space/F để đánh
    }
    else if (_currentPlayer == 2) {
        if (key == 72) MoveUp();
        else if (key == 80) MoveDown();
        else if (key == 75) MoveLeft();
        else if (key == 77) MoveRight();
        else if (key == 13) validAction = true; // Enter để đánh
    }

    // --- XỬ LÝ NƯỚC ĐI ---
    if (validAction) {
        if (CheckBoard(_X, _Y) != 0) { // Nếu ô trống
            _moveCount++;

            if (_currentPlayer == 1) DrawXmini(_X, _Y);
            else DrawOmini(_X, _Y);

            // Kiểm tra thắng thua
            int status = TestBoard();
            if (status != 2) { // Có kết quả (Thắng hoặc Hòa)
                if (status == -1) { _gameWinner = 1; _player1_score++; }
                else if (status == 1) { _gameWinner = 2; _player2_score++; }
                else { _gameWinner = 0; }

                currentState = GAME_OVER;
            }
            else { // Chưa có kết quả -> Đổi lượt
                _currentPlayer = (_currentPlayer == 1) ? 2 : 1;
                _TURN = !_TURN;
                UpdateDynamic2P_UI();
            }
        }
    }
}

int HandleGameOverScreen() {
    int selectedOption = 1;
    DrawGameOverScreen(selectedOption, true);

    while (true) {
        int key = _getch();
        int old_selected = selectedOption;

        if (key == 9) { // TAB
            selectedOption = (selectedOption + 1) % 3;
        }
        else if (key == 13 || key == 32) { // Enter/Space
            return selectedOption;
        }
        else if (key == 27) { // ESC
            return 2; // Back
        }

        if (selectedOption != old_selected) {
            DrawGameOverScreen(selectedOption, false);
        }
    }
}
