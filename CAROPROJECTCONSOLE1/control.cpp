#include "Control.h"
#include "View.h"
#include "Model.h"
#include <stdlib.h>
#include <Windows.h>
#include <string>
#include <conio.h>
#include <iostream>
#include "GameState.h"
#include <io.h>
#include <fcntl.h>
#include <chrono>
#include "truecolor_utils.h"
#include <string.h> // Cần cho strlen, strcpy_s
#include <vector>   // <-- Thêm thư viện vector
#include <random>   // <-- Thêm thư viện để tạo số ngẫu nhiên
#include <ctime>    // <-- Thêm thư viện để khởi tạo seed ngẫu nhiên
// random name
#include <ctype.h>
#include "GameAssets.h"
extern const int TOTAL_ITEMS;
extern const int TOTAL_PAUSE_ITEMS;



std::string GetRandomName() {
    // 1. ĐỊNH NGHĨA DANH SÁCH TÊN Ở ĐÂY, BÊN TRONG HÀM
    // Từ khóa 'static' đảm bảo rằng vector này chỉ được tạo MỘT LẦN DUY NHẤT
    // trong toàn bộ chương trình, dù hàm này có được gọi bao nhiêu lần đi nữa.
    static const std::vector<std::string> RANDOM_NAMES = {
        "ShadowStriker", "CyberNinja", "PixelAvenger", "GigaWizard",
        "QuantumLeaper", "VoidWalker", "ChronoMaster", "Serpent",
        "Phoenix", "Titan", "RogueAI", "Helios", "Nemesis",
        "Vortex", "GalacticHero", "NightRider", "Blaze"
    };

    // 2. LOGIC LẤY TÊN NGẪU NHIÊN (GIỮ NGUYÊN)
    static std::mt19937 rng(static_cast<unsigned int>(time(0)));
    std::uniform_int_distribution<int> dist(0, RANDOM_NAMES.size() - 1);

    return RANDOM_NAMES[dist(rng)];
}



void StartGame() {
	// Bước 1: Dọn dẹp màn hình cũ và chuẩn bị nền trắng cho bàn cờ.
	// Lệnh này vừa đặt nền trắng, vừa xóa sạch mọi thứ.
	ClearScreenWithColor(89, 79, 175);
	SetColorRGB(255, 255, 255);
	ResetData(); // Khoi tao du lieu goc

	// Reset các biến dành riêng cho chế độ 2 người chơi
	_currentPlayer = 1;  // Luôn bắt đầu với Player 1
	_moveCount = 0;      // Xóa lịch sử các nước đi
	_gameWinner = 0;     // Chưa có ai thắng khi game mới bắt đầu
	// --- ĐỒNG BỘ HÓA LƯỢT ĐI KHI BẮT ĐẦU ---
	_currentPlayer = 1;     // Lượt của Player 1
	_TURN = true;         // Tương ứng với lượt của X (true)
    DrawGameUI();

	GotoXY(_X, _Y);
	SetCursorVisible(true);
}

void ExitGame() {
	// Bước 1: Đưa màu sắc của console về trạng thái mặc định.
	ResetColor();
	// Bước 2: Xóa màn hình về nền đen (màu console mặc định) cho sạch sẽ.
// Lệnh này an toàn hơn system("cls") rất nhiều.
	ClearScreenWithColor(0, 0, 0);
	GabageCollect();
	// co the luu game trc khi exit
}

void MoveRight() {
    int right_boundary = LEFT + (BOARD_SIZE - 1) * 4;
    if (_X < right_boundary) _X += 4;
}

void MoveLeft() {
    if (_X > LEFT + 2) _X -= 4;
}

void MoveDown() {
    int bottom_boundary = TOP + (BOARD_SIZE - 1) * 2;
    if (_Y < bottom_boundary) _Y += 2;
}

void MoveUp() {
    if (_Y > TOP + 1) _Y -= 2;
}

// Ẩn/hiện con trỏ
void SetCursorVisible(bool visible) {
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO cursorInfo;
	if (GetConsoleCursorInfo(hConsole, &cursorInfo)) {
		cursorInfo.bVisible = visible ? TRUE : FALSE;
		SetConsoleCursorInfo(hConsole, &cursorInfo);
	}
}

// Hàm InitConsole chuẩn
void InitConsole() {
	system("chcp 65001");
	SetCursorVisible(false);
}

// Khi vào bàn cờ hoặc chỗ cần con trỏ hiện
void ShowCursorForBoard() {
	SetCursorVisible(true);
}




//=================================================================================
// CÁC HÀM XỬ LÝ CHÍNH (HANDLE...)
//=================================================================================

// --- HÀM CHO MENU CHÍNH ---
int HandleMainMenuInput() {
    DrawFullMenu(0); // Vẽ menu lần đầu, chọn mục 0
    int selected = 0;
    int old_selected = 0;
    while (true) {
        int key = toupper(_getch());
        old_selected = selected;
        if (key == 'W' || (key == 224 && _getch() == 72)) { // W hoặc Mũi tên lên
            selected = (selected - 1 + TOTAL_ITEMS) % TOTAL_ITEMS;
        }
        else if (key == 'S' || (key == 224 && _getch() == 80)) { // S hoặc Mũi tên xuống
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
    // 0: Resume, 1: Restart, 2: Save, 3: Quit

    // Vẽ menu Pause lần đầu tiên
    DrawFullPauseMenu(0); // Giả sử hàm này có ClearScreen và vẽ toàn bộ menu

    int selected = 0;
    int old_selected = 0;

    while (true) {
        // Chờ và nhận phím bấm
        int key = toupper(_getch());

        old_selected = selected;

        // Xử lý di chuyển bằng W/S hoặc Mũi tên
        if (key == 'W' || (key == 224 && _getch() == 72)) { // W hoặc Mũi tên lên
            selected = (selected - 1 + TOTAL_PAUSE_ITEMS) % TOTAL_PAUSE_ITEMS;
        }
        else if (key == 'S' || (key == 224 && _getch() == 80)) { // S hoặc Mũi tên xuống
            selected = (selected + 1) % TOTAL_PAUSE_ITEMS;
        }
        // Xử lý phím Enter
        else if (key == 13) {
            return selected; // Trả về lựa chọn: 0, 1, 2, hoặc 3
        }
        // Xử lý phím ESC - Coi như chọn "Resume"
        else if (key == 27) {
            return 0; // Trả về 0 (Resume)
        }

        // Cập nhật giao diện (chỉ vẽ lại phần thay đổi)
        if (selected != old_selected) {
            DrawPauseMenuItem(old_selected, false);
            DrawPauseMenuItem(selected, true);
        }
    }
}
// --- HÀM CHO NEW GAME MODE ---
int HandleNewGameMenuInput() {
    int selected = 1;
    DrawFullNewGameMenu(selected);
    int old_selected = selected;
    while (true) {
        int key = _getch();
        old_selected = selected;
        if (toupper(key) == 'W') {
            selected = (selected - 1 + TOTAL_NEW_GAME_OPTIONS) % TOTAL_NEW_GAME_OPTIONS;
        }
        else if (toupper(key) == 'S') {
            selected = (selected + 1) % TOTAL_NEW_GAME_OPTIONS;
        }
        else if (key == 224) { // Mũi tên
            key = _getch();
            if (key == 72) selected = (selected - 1 + TOTAL_NEW_GAME_OPTIONS) % TOTAL_NEW_GAME_OPTIONS;
            if (key == 80) selected = (selected + 1) % TOTAL_NEW_GAME_OPTIONS;
        }
        else if (key == 13) { // Enter
            if (selected != 0) return selected; // Không cho chọn mục "1 Player"
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

// --- HÀM NHẬP TÊN NGƯỜI CHƠI ---
bool HandlePlayerNameInput() {
    int activeControl = 0;
    char p1_buffer[MAX_NAME_LEN] = "";
    char p2_buffer[MAX_NAME_LEN] = "";
    DrawFullPlayerNameScreen();

    while (true) {
        UpdatePlayerNameScreen(activeControl, p1_buffer, p2_buffer);
        int key = _getch();
        if (key == 27) return false;
        if (key == 9) {
            activeControl = (activeControl + 1) % 3;
            continue;
        }
        switch (activeControl) {
        case 0: { // Player 1
            if (isprint(key) && strlen(p1_buffer) < MAX_NAME_LEN - 1) {
                size_t len = strlen(p1_buffer);
                p1_buffer[len] = key; p1_buffer[len + 1] = '\0';
            }
            else if (key == 8 && strlen(p1_buffer) > 0) {
                p1_buffer[strlen(p1_buffer) - 1] = '\0';
            }
            else if (key == 13) {
                if (strlen(p1_buffer) == 0) {
                    std::string randomName = GetRandomName();
                    strcpy_s(p1_buffer, MAX_NAME_LEN, randomName.c_str());
                }
                activeControl = 1;
            }
            break;
        }
        case 1: { // Player 2
            if (isprint(key) && strlen(p2_buffer) < MAX_NAME_LEN - 1) {
                size_t len = strlen(p2_buffer);
                p2_buffer[len] = key; p2_buffer[len + 1] = '\0';
            }
            else if (key == 8) {
                if (strlen(p2_buffer) > 0) p2_buffer[strlen(p2_buffer) - 1] = '\0';
                else activeControl = 0;
            }
            else if (key == 13) {
                if (strlen(p2_buffer) == 0) {
                    std::string randomName = GetRandomName();
                    strcpy_s(p2_buffer, MAX_NAME_LEN, randomName.c_str());
                }
                if (strlen(p1_buffer) > 0) {
                    strcpy_s(_player1_name, MAX_NAME_LEN, p1_buffer);
                    strcpy_s(_player2_name, MAX_NAME_LEN, p2_buffer);
                    return true;
                }
            }
            break;
        }
        case 2: { // Nút BACK
            if (key == 13) return false;
            break;
        }
        }
    }
}

// --- HÀM CHƠI GAME 2 NGƯỜI ---


void Handle2PlayerGame() {
    // --- 1. LOGIC CHỚP TẮT THEO THỜI GIAN THỰC ---
    // Sử dụng biến static để lưu thời gian, không bị mất đi khi hàm chạy xong
    static DWORD lastBlinkTime = GetTickCount(); // Lấy thời gian hiện tại của hệ thống
    static bool isCursorVisible = true;

    DWORD currentTime = GetTickCount();
    const int BLINK_INTERVAL = 500; // Tốc độ chớp: 500ms (0.5 giây) - Bạn chỉnh số này nếu muốn nhanh/chậm

    // Tính toán: Nếu đã trôi qua 500ms thì đảo trạng thái
    if (currentTime - lastBlinkTime >= BLINK_INTERVAL) {
        isCursorVisible = !isCursorVisible; // Đảo ngược (Sáng -> Tối hoặc Tối -> Sáng)
        lastBlinkTime = currentTime;        // Cập nhật lại mốc thời gian

        // Áp dụng trạng thái mới
        if (isCursorVisible) {
            GotoXY(_X, _Y);         // Đảm bảo về đúng chỗ trước khi hiện
            SetCursorVisible(true);
        }
        else {
            SetCursorVisible(false);
        }
    }

    // Đảm bảo nếu đang ở trạng thái Sáng thì con trỏ phải nằm đúng chỗ
    // (Tránh trường hợp con trỏ bị trôi đi đâu đó)
    if (isCursorVisible) {
        GotoXY(_X, _Y);
    }

    // --- 2. XỬ LÝ INPUT ---
    if (_kbhit()) {
        int key = _getch();

        // === KỸ THUẬT "RESET NHỊP TIM" ===
        // Khi người chơi bấm phím, lập tức cho SÁNG NGAY và RESET đồng hồ.
        // Để người chơi thấy phản hồi tức thì, không bị cảm giác "khựng".
        isCursorVisible = true;
        lastBlinkTime = currentTime;
        SetCursorVisible(true);
        GotoXY(_X, _Y);
        // =================================

        if (key == 27) { // ESC
            currentState = PAUSE;
            return;
        }

        bool actionKeyPressed = false;

        // --- PLAYER 1 ---
        if (_currentPlayer == 1) {
            int upperKey = toupper(key);
            if (upperKey == 'W') MoveUp();
            else if (upperKey == 'A') MoveLeft();
            else if (upperKey == 'S') MoveDown();
            else if (upperKey == 'D') MoveRight();
            else if (upperKey == 'F') actionKeyPressed = true;
        }
        // --- PLAYER 2 ---
        else {
            if (key == 224 || key == 0) {
                int arrowKey = _getch();
                if (arrowKey == 72) MoveUp();
                else if (arrowKey == 80) MoveDown();
                else if (arrowKey == 75) MoveLeft();
                else if (arrowKey == 77) MoveRight();
            }
            else if (key == 13) actionKeyPressed = true;
        }

        // Sau khi di chuyển, cập nhật lại vị trí con trỏ vật lý ngay
        GotoXY(_X, _Y);

        // --- XỬ LÝ ĐÁNH CỜ ---
        if (actionKeyPressed) {
            int pY_idx = (_Y - TOP) / CELL_VISUAL_HEIGHT;
            int pX_idx = (_X - LEFT) / CELL_VISUAL_WIDTH;

            _TURN = (_currentPlayer == 1);

            if (CheckBoard(pX_idx, pY_idx) != 0) {
                _moveCount++;

                // --- CẬP NHẬT UI & BÀN CỜ (Chiến thuật Ninja) ---
                SetCursorVisible(false); // Tàng hình
                UpdateDynamic2P_UI();    // Vẽ điểm

                GotoXY(_X, _Y);          // Về bàn cờ
                SetBgRGB(89, 79, 175);
                if (_currentPlayer == 1) {
                    SetColorRGB(255, 0, 0); printf("X");
                }
                else {
                    SetColorRGB(0, 0, 255); printf("O");
                }
                SetColorRGB(0, 0, 0);
                SetBgRGB(89, 79, 175);
                // Sau khi vẽ xong X/O, ta trả lại trạng thái SÁNG cho con trỏ
                // Để chuẩn bị cho lượt sau
                isCursorVisible = true;
                lastBlinkTime = GetTickCount(); // Reset đồng hồ
                GotoXY(_X, _Y);
                SetCursorVisible(true);
                // ------------------------------------------------

                int status = TestBoard();
                if (status != 2) {
                    if (status == -1) _gameWinner = 1;
                    else if (status == 1) _gameWinner = 2;
                    else _gameWinner = 0;

                    if (_gameWinner == 1) _player1_score++;
                    else if (_gameWinner == 2) _player2_score++;

                    currentState = GAME_OVER;
                    return;
                }
                _currentPlayer = (_currentPlayer == 1) ? 2 : 1;
            }
        }
    }
}
   
// --- HÀM MÀN HÌNH KẾT QUẢ ---
int HandleGameOverScreen() {
    int selectedOption = 1;
    DrawGameOverScreen(selectedOption);
    int old_selected = selectedOption;
    while (true) {
        int key = _getch();
        old_selected = selectedOption;
        if (key == 9) { // TAB
            selectedOption = (selectedOption + 1) % 3;
        }
        else if (key == 224) { // Mũi tên
            key = _getch();
            if (key == 75) selectedOption = (selectedOption - 1 + 3) % 3;
            else if (key == 77) selectedOption = (selectedOption + 1) % 3;
        }
        else if (key == 13) { // Enter
            if (selectedOption != 0) return selectedOption;
        }
        else if (key == 27) { // ESC
            return 2; // Coi như chọn Back
        }
        if (selectedOption != old_selected) {
            DrawGameOverScreen(selectedOption);
        }
    }
}