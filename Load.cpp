#include "Save&load.h"
#include "model.h"  

// HÀM HỖ TRỢ ĐỂ CHUYỂN ĐỔI INT SANG BOOL
void setTurnFromInt(int turnInt) {
    _TURN = (turnInt == -1); // -1 là true (X), 1 là false (O)
}

bool loadGame(const string& filename) { // Đã thay đổi chữ ký hàm
    string fullPath = SAVE_DIR + filename + ".txt";
    ifstream file(fullPath);

    if (file.is_open()) {
        int temp_size;
        int temp_turn;

        // 1. Đọc kích thước bàn cờ
        if (!(file >> temp_size) || temp_size != BOARD_SIZE) { // Kiểm tra kích thước
            file.close();
            cerr << "\n Lỗi: Kích thước bàn cờ không khớp." << endl;
            return false;
        }

        // 2. Đọc người chơi hiện tại
        if (!(file >> temp_turn)) { file.close(); return false; }
        setTurnFromInt(temp_turn); // Gán vào biến toàn cục _TURN

        // 3. Đọc trạng thái bàn cờ
        for (int i = 0; i < BOARD_SIZE; ++i) { // Dùng BOARD_SIZE từ model.h
            for (int j = 0; j < BOARD_SIZE; ++j) {
                if (!(file >> _A[i][j].c)) { file.close(); return false; } // Gán vào _A[i][j].c
            }
        }

        file.close();
        cout << "\n Game đã được tải thành công: " << filename << endl;
        return true;
    }
    else {
        return false; // File không tồn tại hoặc lỗi khác
    }
}