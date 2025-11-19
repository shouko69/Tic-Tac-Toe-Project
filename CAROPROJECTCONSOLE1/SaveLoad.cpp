#include "SaveLoad.h"
#include <fstream>
#include <windows.h>
#include <limits>

const std::string SAVE_DIR = "caro_saves\\";

bool SaveGameToFile(const GameStateData& data, const std::string& filename) {
    CreateDirectoryA(SAVE_DIR.c_str(), NULL);
    std::string fullPath = SAVE_DIR + filename;

    std::ofstream file(fullPath);
    if (!file.is_open()) return false;

    // 1. Ghi các thông số cơ bản
    file << data.currentPlayer << std::endl;
    file << data.cursorX << std::endl;
    file << data.cursorY << std::endl;

    // --- THÊM MỚI: Ghi điểm số và lượt đi ---
    file << data.p1Score << std::endl;
    file << data.p2Score << std::endl;
    file << data.moveCount << std::endl;
    // ---------------------------------------

    // 2. Ghi bàn cờ (Giữ nguyên)
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            file << data.board[i][j] << " ";
        }
        file << std::endl;
    }

    // 3. Ghi các chuỗi ký tự (Tên và Metadata)
    // Lưu ý: Ghi chuỗi thì dùng endl để xuống dòng cho dễ đọc
    file << data.p1Name << std::endl; // <-- LƯU TÊN P1
    file << data.p2Name << std::endl; // <-- LƯU TÊN P2

    file << data.name << std::endl;   // Tên file save
    file << data.date << std::endl;
    file << data.type << std::endl;

    file.close();
    return true;
}

bool LoadGameFromFile(GameStateData& data, const std::string& filename) {
    std::string fullPath = SAVE_DIR + filename;
    std::ifstream file(fullPath);
    if (!file.is_open()) return false;

    // 1. Đọc thông số cơ bản
    file >> data.currentPlayer;
    file >> data.cursorX;
    file >> data.cursorY;

    // --- THÊM MỚI: Đọc điểm và lượt đi ---
    file >> data.p1Score;
    file >> data.p2Score;
    file >> data.moveCount;
    // ------------------------------------

    // 2. Đọc bàn cờ
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            file >> data.board[i][j];
        }
    }

    // 3. Đọc chuỗi ký tự
    // QUAN TRỌNG: Sau khi đọc số (>>), ký tự xuống dòng '\n' vẫn còn.
    // Phải dùng ignore() để bỏ nó đi trước khi dùng getline().
    file.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');

    file.getline(data.p1Name, MAX_NAME_LEN); // <-- ĐỌC TÊN P1
    file.getline(data.p2Name, MAX_NAME_LEN); // <-- ĐỌC TÊN P2

    file.getline(data.name, MAX_NAME_LEN);   // Đọc tên file save
    file.getline(data.date, MAX_DATE_LEN);
    file.getline(data.type, MAX_TYPE_LEN);

    file.close();
    return file.good();
}