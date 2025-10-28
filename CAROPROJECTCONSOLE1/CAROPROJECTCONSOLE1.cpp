#include "View.h"
#include "Control.h"
#include "Model.h"
#include "GameState.h"
#include "HandleInput.h"
#include <conio.h>
#include <ctype.h>
#include <iostream>
#include <windows.h>

GameState currentState = MENU;

int main() {
    FixConsoleWindow();
    InitConsole(); 

    while (true) {
        switch (currentState) {

            // --- MENU CHÍNH ---
        case MENU: {
            int choice = HandleMainMenuInput(); // menu chính vẽ lần đầu bên trong hàm này

            switch (choice) {
            case 0: // PLAY
                SetCursorVisible(true); // bật con trỏ cho bàn cờ
                StartGame();            // reset bàn cờ, _X/_Y về ô đầu
                currentState = PLAY;
                break;
            case 1: currentState = LOAD; break;
            case 2: currentState = SETTINGS; break;
            case 3: currentState = GUIDE; break;
            case 4: currentState = ABOUT; break;
            case 5: currentState = EXIT; break;
            }
            break;
        }

                 // --- TRẠNG THÁI CHƠI GAME ---
        case PLAY: {
            if (_kbhit()) {
                bool validEnter = true;
                _COMMAND = toupper(_getch());

                if (_COMMAND == 27) { // ESC → Pause Menu
                    currentState = PAUSE;      // trực tiếp về menu chính
                    SetCursorVisible(false);  // ẩn con trỏ menu
                    break;
                }

                // Di chuyển con trỏ
                else if (_COMMAND == 'A') MoveLeft();
                else if (_COMMAND == 'D') MoveRight();
                else if (_COMMAND == 'W') MoveUp();
                else if (_COMMAND == 'S') MoveDown();

                else if (_COMMAND == 13) { // ENTER đánh quân
                    GotoXY(_X, _Y);
                    int checkResult = CheckBoard(_X, _Y);
                    switch (checkResult) {
                    case -1: std::cout << "X"; break;
                    case 1:  std::cout << "O"; break;
                    case 0:  validEnter = false; break;
                    }

                    if (validEnter) {
                        int status = ProcessFinish(TestBoard());
                        if (status == -1 || status == 1 || status == 0) {
                            if (AskContinue() != 'Y') {
                                currentState = MENU;
                                SetCursorVisible(false); // menu ẩn con trỏ
                            }
                            else {
                                StartGame();
                                SetCursorVisible(true); // bật con trỏ chơi tiếp
                            }
                        }
                    }
                    GotoXY(_X, _Y);
                    validEnter = true;
                }
            }
            Sleep(15); // tránh tốn CPU
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
                system("cls");        // 1. Xóa menu PAUSE
                DrawBoard(BOARD_SIZE);        // 2. Vẽ lại khung bàn cờ (từ View.h)
                RedrawBoardState(); // 3. Vẽ lại các quân X, O đã đánh (hàm mới ở trên)
                // ---------------------------------

                SetCursorVisible(true); // Bật con trỏ
                currentState = PLAY;    // Quay lại trạng thái chơi
                break;

            case 1: // Restart
                // StartGame() đã bao gồm cả việc vẽ bàn cờ mới
                SetCursorVisible(true);
                StartGame();
                currentState = PLAY;
                break;

            //case 2: // Save
            //    // Thêm logic Save Game ở đây
            //    // Sau khi Save, có thể quay lại PAUSE hoặc PLAY
            //    // currentState = PAUSE; // Quay lại menu pause
            //    break;

            //case 3: // Settings
            //    currentState = SETTINGS; // Chuyển sang trạng thái Cài đặt
            //    break;

            case 4: // Quit
                system("cls"); // Xóa màn hình pause trước khi về Menu chính
                currentState = MENU;
                SetCursorVisible(false);
                break;
            }
            break;
        }

                  // --- SETTINGS / LOAD / GUIDE / ABOUT ---
        case SETTINGS:
        case LOAD:
        case GUIDE:
        case ABOUT: // tạm thời cho các case cùng một lệnh if
            if (_kbhit() && toupper(_getch()) == 27) {
                currentState = MENU;
                SetCursorVisible(false); // menu ẩn con trỏ
            }
            break;

            // --- THOÁT ---
        case EXIT:
            ExitGame();
            return 0;
        }
    }
}