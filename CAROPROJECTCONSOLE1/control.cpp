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
    //Đưa toạ độ về 0 (Góc trái trên bàn cờ)
    _X = 0;
    _Y = 0;
    DrawGameUI();

    GotoBoard(_X, _Y);
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
    // Chỉ cho phép tăng nếu chưa chạm lề phải (BOARD_SIZE - 1)
    if (_X < BOARD_SIZE - 1) {
        _X++; // Tăng chỉ số cột lên 1 (ví dụ từ ô 0 sang ô 1)
        GotoBoard(_X, _Y); // Hàm này sẽ tự đưa con trỏ đến giữa ô mới
    }
}

void MoveLeft() {
    // Chỉ cho phép giảm nếu lớn hơn 0
    if (_X > 0) {
        _X--; 
        GotoBoard(_X, _Y);
    }
}

void MoveDown() {
    // Chỉ cho phép xuống nếu chưa chạm đáy
    if (_Y < BOARD_SIZE - 1) {
        _Y++;
        GotoBoard(_X, _Y);
    }
}

void MoveUp() {
    // Chỉ cho phép lên nếu lớn hơn 0
    if (_Y > 0) {
        _Y--;
        GotoBoard(_X, _Y);
    }
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
    // 1. Đưa con trỏ về vị trí hiện tại
    GotoBoard(_X, _Y);

    // 2. Chờ phím bấm
    int key = _getch();

    // Xử lý mã phím mũi tên (0 hoặc 224)
    if (key == 0 || key == 224) {
        key = _getch();
        // Mã mũi tên: 72(Lên), 80(Xuống), 75(Trái), 77(Phải)
    }
    else {
        key = toupper(key); // Chuyển w -> W, f -> F
    }

    if (key == 27) { // ESC
        currentState = PAUSE;
        return;
    }

    // --- LOGIC PHÂN QUYỀN (SỬA LẠI NGHIÊM NGẶT) ---
    bool validAction = false; // Cờ đánh dấu có hành động hợp lệ hay không

    // === NẾU LÀ LƯỢT PLAYER 1 (X) ===
    if (_currentPlayer == 1) {
        // CHỈ CHO PHÉP CÁC PHÍM NÀY:
        if (key == 'W') { MoveUp(); }
        else if (key == 'S') { MoveDown(); }
        else if (key == 'A') { MoveLeft(); }
        else if (key == 'D') { MoveRight(); }

        // Nút đánh của P1: Chỉ phím F hoặc SPACE (32).
        // TUYỆT ĐỐI KHÔNG CÓ KEY 13 (ENTER) Ở ĐÂY
        else if (key == 'F' || key == 32) {
            validAction = true;
        }
    }

    // === NẾU LÀ LƯỢT PLAYER 2 (O) ===
    else if (_currentPlayer == 2) {
        // CHỈ CHO PHÉP CÁC PHÍM NÀY:
        if (key == 72) { MoveUp(); }        // Mũi tên Lên
        else if (key == 80) { MoveDown(); } // Mũi tên Xuống
        else if (key == 75) { MoveLeft(); } // Mũi tên Trái
        else if (key == 77) { MoveRight(); } // Mũi tên Phải

        // Nút đánh của P2: Chỉ phím ENTER (13) hoặc Numpad Enter.
        else if (key == 13) {
            validAction = true;
        }
    }

    // --- XỬ LÝ ĐÁNH CỜ ---
    if (validAction) {
        // Kiểm tra ô trống
        if (CheckBoard(_X, _Y) != 0) {
            _moveCount++;

            // --- SỬA ĐỔI TẠI ĐÂY ---
            // Thay vì GotoBoard rồi cout thủ công, ta gọi hàm vẽ xịn xò của bạn.

            if (_currentPlayer == 1) {
                DrawXmini(_X, _Y); // Gọi hàm vẽ X (nhớ truyền toạ độ logic 0,1,2...)
            }
            else {
                DrawOmini(_X, _Y); // Gọi hàm vẽ O
            }

            // Lưu ý: Hàm DrawX/DrawO của bạn nên tự xử lý việc:
            // 1. GotoBoard(_X, _Y) hoặc tính toán toạ độ màn hình.
            // 2. SetBgRGB(...) để không bị lỗi nền đen.
            // -----------------------

            // Kiểm tra thắng thua (Giữ nguyên)
            int status = TestBoard();
            if (status != 2) {
                if (status == -1) { _gameWinner = 1; _player1_score++; }
                else if (status == 1) { _gameWinner = 2; _player2_score++; }
                else { _gameWinner = 0; }

                currentState = GAME_OVER;
            }
            else {
                // Đổi lượt
                _currentPlayer = (_currentPlayer == 1) ? 2 : 1;
                _TURN = !_TURN;
                UpdateDynamic2P_UI();
            }
        }
    }
}
   
// --- HÀM MÀN HÌNH KẾT QUẢ ---
int HandleGameOverScreen() {
    int selectedOption = 1; // Mặc định chọn Play Again (giữa)

    // LẦN ĐẦU TIÊN: Vẽ TOÀN BỘ màn hình (Nền, Chữ Win, Stats, Nút)
    // Bạn cần sửa hàm DrawGameOverScreen thêm tham số bool drawAll
    DrawGameOverScreen(selectedOption, true);

    while (true) {
        int key = _getch();

        // Chuẩn hóa phím về chữ hoa để a hay A đều ăn
        int upperKey = toupper(key);

        int old_selected = selectedOption;

        // --- XỬ LÝ PHÍM TAB (Mã ASCII = 9) ---
        if (key == 9) {
            // Cộng 1 để qua phải, chia lấy dư cho 3 để quay vòng (0 -> 1 -> 2 -> 0)
            selectedOption = (selectedOption + 1) % 3;
        }
        else if (key == 13 || key == 32) { // Enter hoặc Space
            return selectedOption;
        }
        else if (key == 27) { // ESC
            return 2; // Coi như chọn Back
        }

        // --- CHỈ VẼ LẠI KHI CÓ SỰ THAY ĐỔI ---
        if (selectedOption != old_selected) {
            // QUAN TRỌNG: Tham số false nghĩa là "Chỉ vẽ lại nút bấm, đừng xóa màn hình"
            DrawGameOverScreen(selectedOption, false);
        }
    }
}