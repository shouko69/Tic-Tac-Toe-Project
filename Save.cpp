#include "Save&load.h"
#include "model.h"  

int getTurnInt() {
    return _TURN ? -1 : 1; // Quy ước: -1 cho X, 1 cho O
}

bool saveGame(const string& filename) { 
    CreateDirectoryA(SAVE_DIR.c_str(), NULL);
    string fullPath = SAVE_DIR + filename + ".txt";
    ofstream file(fullPath);

    if (file.is_open()) {
        // 1. Kích thước (Lưu BOARD_SIZE)
        file << BOARD_SIZE << endl;

        // 2. Người chơi hiện tại (Lưu _TURN: -1 hoặc 1)
        file << getTurnInt() << endl; // Sử dụng _TURN từ model.h

        // 3. Bàn cờ (Lưu từng ô _A[i][j].c)
        for (int i = 0; i < BOARD_SIZE; ++i) {
            for (int j = 0; j < BOARD_SIZE; ++j) {
                file << _A[i][j].c << " "; // Sử dụng _A từ model.h
            }
            file << endl;
        }

        file.close();
        cout << "\n ĐÃ LƯU MÀN CHƠI!" << endl;
        return true;
    }
    else {
        cerr << "\n KHÔNG THỂ LƯU" << endl;
        return false;
    }
}