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
    // BƯỚC 1: CHỌN "VIÊN GẠCH" TRƯỚC TIÊN!
    // Đặt font chữ thành một kích thước nhỏ để tăng "độ phân giải".
    SetConsoleFont(L"Terminal", 0, 15);

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
    CenterConsole();
   

    while (true) {
        switch (currentState) {

            // --- MENU CHÍNH ---
        case MENU: {
            int choice = HandleMainMenuInput(); // menu chính vẽ lần đầu bên trong hàm này

            switch (choice) {
            case 0: // PLAY
                currentState = NEW_GAME_MODE;
                break;
            case 1: currentState = LOAD; break;
            case 2: currentState = SETTINGS; break;
            case 3: currentState = GUIDE; break;
            case 4: currentState = ABOUT; break;
            case 5: currentState = EXIT; break;
            }
            break;
        }
        case NEW_GAME_MODE: {
            // Bước 1: Gọi hàm xử lý. Chương trình sẽ "dừng" ở đây
            // cho đến khi người dùng đưa ra một lựa chọn (Enter hoặc ESC).
            int choice = HandleNewGameMenuInput();

            // Bước 2: Xử lý kết quả mà hàm vừa trả về.
            switch (choice) {
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
            Sleep(15);
            break;
        }

        case GAME_OVER: {
            SetCursorVisible(false);
            // Gọi hàm xử lý và chờ nó trả về kết quả
            int choice = HandleGameOverScreen();

            // Xử lý kết quả
            switch (choice) {
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
                ClearScreenWithColor(255, 255, 255);
                SetColorRGB(0, 0, 0);
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
                currentState = PLAY_2P;

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