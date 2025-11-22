#include "View.h"
#include "Control.h"
#include "Model.h"
#include "GameState.h"
#include <conio.h>
#include <ctype.h>
#include <iostream>
#include <windows.h>
#include "truecolor_utils.h"
#include "SaveLoadUI.h"



GameState currentState = MENU;


int main() {
    // ----- BẮT ĐẦU THIẾT LẬP CONSOLE -----
    CenterConsole();
    SetConsoleFont(L"Terminal", 0, 15);
    ResizeConsoleWindow(CONSOLE_WIDTH, CONSOLE_HEIGHT);
    EnableTrueColor();
    FixConsoleWindow();
    InitConsole(); 
    CenterConsole();
    StartIntro();

    while (true) {
        switch (currentState) {
        case MENU: {
            int choice = HandleMainMenuInput();
            switch (choice) {
            case 0: currentState = NEW_GAME_MODE; break;
            case 1: currentState = LOAD; break;
            case 2: currentState = SETTINGS; break;
            case 3: currentState = GUIDE; break;
            case 4: currentState = ABOUT; break;
            case 5: currentState = EXIT; break;
            }
            break;
        }
        case NEW_GAME_MODE: {         
            int choice = HandleNewGameMenuInput();
            switch (choice) {
            case 0:
                ClearScreenWithColor(89, 79, 175);
                SetColorRGB(255, 255, 255);
                GotoXY(65, 22); // Căn chỉnh vị trí cho đẹp
                std::cout << "==================================";
                GotoXY(65, 23);
                std::cout << "    TINH NANG DANG PHAT TRIEN     ";
                GotoXY(65, 24);
                std::cout << "          (COMING SOON)           ";
                GotoXY(65, 25);
                std::cout << "==================================";
                GotoXY(65, 28);
                std::cout << ">> Nhan phim bat ky de quay lai <<";
                _getch();
                currentState = MENU;
                break;
            case 1: // Người dùng chọn "2 PLAYERS"
                currentState = PLAYER_NAME_INPUT;
                break;
            case 2: // Người dùng chọn "BACK"
                currentState = MENU;
                break;
            case ESCAPE_KEY: // Người dùng nhấn phím ESC
                currentState = MENU;
                break;
            }

            break; // Kết thúc case NEW_GAME_MODE
        }
        case PLAYER_NAME_INPUT: {
            
            

            // Gọi hàm xử lý. Chương trình sẽ "kẹt" lại bên trong hàm này
            // cho đến khi người dùng hoàn tất (trả về true) hoặc thoát (trả về false).
            bool isReady = HandlePlayerNameInput();
            
            // Sử dụng switch-case để xử lý kết quả boolean trả về.
            // Mặc dù chỉ có 2 trường hợp, cách này vẫn đảm bảo tính nhất quán.
            switch (isReady) {
            case true: // Người chơi đã xác nhận tên
                // Chuyển sang trạng thái chơi game 2 người.
                // Bạn sẽ cần tạo một case cho PLAY_2P.
                StartGame();
                currentState = PLAY_2P;
                break;

            case false: // Người chơi đã chọn thoát ra
                // Quay trở lại màn hình chọn chế độ chơi.
                currentState = NEW_GAME_MODE;
                break;
            }

            // Dọn dẹp bộ đệm bàn phím sau khi thoát khỏi vòng lặp của HandlePlayerNameInput.
            // Đây là bước cực kỳ quan trọng để tránh lỗi "trôi lệnh".

            break; // Kết thúc case PLAYER_NAME_INPUT
        }
                 // --- TRẠNG THÁI CHƠI GAME ---
        case PLAY_2P:{
            
            Handle2PlayerGame();
            GotoBoard(_X, _Y);
            Sleep(15);
            break;
        }

        case GAME_OVER: {
            SetCursorVisible(false);
            // Gọi hàm xử lý và chờ nó trả về kết quả
            int choice = HandleGameOverScreen();

            // Xử lý kết quả
            switch (choice) {
            case 0:
                ShowSaveGameScreen();
                break;
            case 1: // Chọn "Play again"
                StartGame();
                currentState = PLAY_2P;
                break;
            case 2: // Chọn "Back" hoặc nhấn ESC
                _player1_score = 0;
                _player2_score = 0;
                currentState = MENU;
                break;
            }
            
            break;
        }


                /*  --- PAUSE MENU ---*/
        case PAUSE: {
            SetCursorVisible(false); // ẩn con trỏ menu

            // HandlePauseMenuInput nên trả về 0-4
            int pauseChoice = HandlePauseMenuInput();

            switch (pauseChoice) {

            case 0: // Resume
                // --- BẮT BUỘC SỬA LỖI Ở ĐÂY ---
                DrawGameUI();
                
                DrawBoard(BOARD_SIZE);        // 2. Vẽ lại khung bàn cờ (từ View.h)
                RedrawBoardState(); // 3. Vẽ lại các quân X, O đã đánh (hàm mới ở trên)
                // ---------------------------------

                SetCursorVisible(true); // Bật con trỏ
                currentState = PLAY_2P;    // Quay lại trạng thái chơi
                break;

            case 1: // Restart
                // StartGame() đã bao gồm cả việc vẽ bàn cờ mới
                SetCursorVisible(true);
                StartGame();
                currentState = PLAY_2P;
                break;

            case 2: // Save
                ShowSaveGameScreen();
                break;

            //case 3: // Settings
            //    currentState = SETTINGS; // Chuyển sang trạng thái Cài đặt
            //    break;

            case 4: // Quit
                currentState = MENU;
                SetCursorVisible(false);
                break;
            }
            break;
        }

                  // --- SETTINGS / LOAD / GUIDE / ABOUT ---
        case SETTINGS:
            // viết tạm vì tính năng này chưa hoàn thành 
            ClearScreenWithColor(30, 30, 30);
            SetColorRGB(255, 255, 255); 
            GotoXY(65, 22); // Căn chỉnh vị trí cho đẹp
            std::cout << "==================================";
            GotoXY(65, 23);
            std::cout << "    TINH NANG DANG PHAT TRIEN     ";
            GotoXY(65, 24);
            std::cout << "          (COMING SOON)           ";
            GotoXY(65, 25);
            std::cout << "==================================";
            GotoXY(65, 28);
            std:: cout << ">> Nhan phim bat ky de quay lai <<";
            // 3. Đợi phím
            _getch();
            currentState = MENU;
            break;
        case LOAD: {
            // Reset dữ liệu tạm trước khi load
            ResetData();

            bool loadSuccess = ShowLoadGameScreen();

            if (loadSuccess) {
                // --- BƯỚC 1: VẼ LẠI UI CƠ BẢN ---
                ClearScreenWithColor(89, 79, 175);
                SetColorRGB(0, 0, 0);
                DrawGameUI();

                // --- BƯỚC 2: KIỂM TRA XEM FILE SAVE NÀY ĐÃ KẾT THÚC CHƯA ---
                // Gọi hàm kiểm tra thắng thua ngay lập tức trên dữ liệu vừa load
                int status = TestBoard();

                if (status != 2) {
                    // => ĐÂY LÀ FILE SAVE ĐÃ KẾT THÚC (CÓ NGƯỜI THẮNG)

                    // 1. Dọn sạch bàn cờ
                    for (int i = 0; i < BOARD_SIZE; i++) {
                        for (int j = 0; j < BOARD_SIZE; j++) {
                            _A[i][j].c = 0;
                        }
                    }

                    // --- SỬA LỖI TẠI ĐÂY (RESET BIẾN) ---
                    _moveCount = 0;      // Reset số nước đi về 0
                    _currentPlayer = 1;  // Luôn bắt đầu lại là Player 1
                    _TURN = true;        // Luôn là lượt của X
                    _X = 0; _Y = 0;      // Con trỏ về góc

                    // --- QUAN TRỌNG: VẼ LẠI SỐ MOVES LÊN MÀN HÌNH ---
                    // Nếu không gọi hàm này, biến đã về 0 nhưng màn hình vẫn hiện số cũ (7)
                    UpdateDynamic2P_UI();
                    // -------------------------------------------------

                    // 4. Vẽ lại bàn cờ (Lúc này trắng tinh)
                    RedrawBoardState();

                    // 5. Thông báo (Tùy chọn)
                    GotoXY(LEFT, TOP - 2);
                    SetColorRGB(255, 255, 0); // Màu vàng cho nổi
                    std::cout << "Loaded finished game. Starting new round...";
                    SetColorRGB(0, 0, 0);
                }
                else {
                    // Game chưa kết thúc -> Vẽ lại các quân cờ cũ để chơi tiếp
                    RedrawBoardState();
                }

                // --- BƯỚC 3: ĐƯA CON TRỎ VỀ ĐÚNG CHỖ ---
                GotoBoard(_X, _Y);

                // --- BƯỚC 4: VÀO GAME ---
                currentState = PLAY_2P;
                SetCursorVisible(true);
            }
            else {
                // Nếu load thất bại hoặc bấm Back
                currentState = MENU;
            }
            break;
        }
        case GUIDE:
            break;
        case ABOUT:
            StartAbout();
            break;
        case EXIT:
            ExitGame();
            return 0;
        }
    }
}
