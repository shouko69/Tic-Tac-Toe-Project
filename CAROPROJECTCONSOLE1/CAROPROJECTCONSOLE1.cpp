#include "View.h"
#include "Control.h"
#include "Model.h"
#include "GameState.h"
#include "HandleInput.h"
#include <conio.h>
#include <ctype.h>
#include <iostream>
#include <windows.h>
#include "truecolor_utils.h"
#include "SaveLoadUI.h"



GameState currentState = MENU;

int main() {
    // ----- BẮT ĐẦU THIẾT LẬP CONSOLE -----
    // BƯỚC 1: CHỌN "VIÊN GẠCH" TRƯỚC TIÊN!
    // Đặt font chữ thành một kích thước nhỏ để tăng "độ phân giải".
    SetConsoleFont(L"Consolas", 0, 15);

    // Bạn có thể thử 8x12 hoặc font "Consolas" nếu muốn
    // BƯỚC 2: BÂY GIỜ MỚI "XÂY TƯỜNG"!
    // Đặt kích thước cửa sổ bằng số lượng "viên gạch" đã chọn.
    ResizeConsoleWindow(CONSOLE_WIDTH, CONSOLE_HEIGHT);
    // BƯỚC 3: CÁC THIẾT LẬP KHÁC
    // Kích hoạt màu 24-bit
    EnableTrueColor();
    // Khóa kích thước cửa sổ để người dùng không thay đổi được
    FixConsoleWindow();
    // Hỗ trợ ký tự Unicode (cho các ký tự vẽ vời phức tạp)
    InitConsole(); 
    StartIntro();
    CenterConsole();
   

    while (true) {
        switch (currentState) {

            // --- MENU CHÍNH ---
        case MENU: {
            int choice = HandleMainMenuInput(); // menu chính vẽ lần đầu bên trong hàm này

            switch (choice) {
            case 0: // PLAY
                SetCursorVisible(true); // bật con trỏ cho bàn cờ
                StartGame();            // reset bàn cờ, _X/_Y về ô đầu
                /*StartTimerThread();*/
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
            // --- PHẦN 1: XỬ LÝ INPUT (CHỈ KHI CÓ PHÍM BẤM) ---
            // Nhiệm vụ của phần này chỉ là CẬP NHẬT TRẠNG THÁI (thay đổi giá trị _X, _Y, _A...)
            if (_kbhit()) {
                bool validEnter = true;
                _COMMAND = toupper(_getch());

                if (_COMMAND == 27) { // ESC → Pause Menu
                    currentState = PAUSE;
                    SetCursorVisible(false);
                    break; // Thoát khỏi case PLAY ngay
                }

                // Di chuyển con trỏ (chỉ thay đổi biến _X, _Y)
                else if (_COMMAND == 'A') MoveLeft();
                else if (_COMMAND == 'D') MoveRight();
                else if (_COMMAND == 'W') MoveUp();
                else if (_COMMAND == 'S') MoveDown();

                // Đánh cờ (thay đổi mảng _A và vẽ quân cờ MỘT LẦN để có phản hồi ngay)
                else if (_COMMAND == 13) {
                    GotoXY(_X, _Y); // Di chuyển tới ô để vẽ quân cờ
                    int checkResult = CheckBoard(_X, _Y);
                    switch (checkResult) {
                    case -1:
                        SetColorRGB(255, 0, 0); std::cout << "X"; break;
                    case 1:
                        SetColorRGB(0, 0, 255); std::cout << "O"; break;
                    case 0:
                        validEnter = false; break;
                    }

                    if (validEnter) {
                        int status = ProcessFinish(TestBoard());
                        if (status != 2) { // Nếu game đã kết thúc (thắng, thua, hòa)
                            if (AskContinue() != 'Y') {
                                currentState = MENU;
                                SetCursorVisible(false);
                            }
                            else {
                                StartGame();
                                SetCursorVisible(true);
                            }
                            break; // Thoát khỏi case PLAY
                        }
                    }
                }
            }

            // --- PHẦN 2: CẬP NHẬT & VẼ (LUÔN LUÔN CHẠY) ---
            // Bất kể người dùng có bấm phím hay không, lệnh này phải được gọi
            // để "ghim" con trỏ vào đúng vị trí _X, _Y hiện tại.
            // Đây là dòng code sửa cả 2 lỗi của bạn.
            GotoXY(_X, _Y);

            // --- PHẦN 3: TẠM NGHỈ ---
            Sleep(15);
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
                ClearScreenWithColor(255, 255, 255);
                SetColorRGB(0, 0, 0);
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
        case LOAD: {
            ResetData();
            // ShowLoadGameScreen sẽ tự lo việc xóa menu chính, vẽ màn hình load,
            // và gọi ApplyLoadedData nếu người dùng chọn một file.
            // Nó chỉ trả về true (thành công) hoặc false (hủy bỏ).
            bool loadSuccess = ShowLoadGameScreen();

            if (loadSuccess) {
                // Dữ liệu ĐÃ được áp dụng bên trong ShowLoadGameScreen.
                // Bây giờ chúng ta chỉ cần vẽ lại trạng thái game mới.

                // 1. Xóa màn hình Load để chuẩn bị vẽ bàn cờ
                ClearScreenWithColor(255, 255, 255); // Dùng màu nền của bàn cờ

                // 2. Đặt lại màu vẽ (ví dụ: màu đen cho khung)
                SetColorRGB(0, 0, 0);

                // 3. Vẽ khung bàn cờ trống
                DrawBoard(BOARD_SIZE);

                // 4. Vẽ lại các quân cờ dựa trên mảng _A đã được cập nhật
                RedrawBoardState();

                // 5. Di chuyển con trỏ vật lý đến vị trí (_X, _Y) đã được load
                GotoXY(_X, _Y);

                // 6. Chuyển trạng thái game sang đang chơi
                currentState = PLAY;

                // 7. Hiển thị lại con trỏ
                SetCursorVisible(true);
            }
            
            else {
                currentState = MENU;
            }
            break; // Kết thúc case LOAD
        }
        case GUIDE:
        case ABOUT:
            StartAbout();
            break;
        case EXIT:
            ExitGame();
            return 0;
        }
    }
}