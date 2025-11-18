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

    file << data.currentPlayer << std::endl;
    file << data.cursorX << std::endl;
    file << data.cursorY << std::endl;

    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            file << data.board[i][j] << " ";
        }
        file << std::endl;
    }

    file << data.name << std::endl;
    file << data.date << std::endl;
    file << data.type << std::endl;

    file.close();
    return true;
}

bool LoadGameFromFile(GameStateData& data, const std::string& filename) {
    std::string fullPath = SAVE_DIR + filename;
    std::ifstream file(fullPath);
    if (!file.is_open()) return false;

    file >> data.currentPlayer;
    file >> data.cursorX;
    file >> data.cursorY;

    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            file >> data.board[i][j];
        }
    }

    // Bỏ qua ký tự xuống dòng còn sót lại trước khi đọc chuỗi
    file.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
    
    file.getline(data.name, MAX_NAME_LEN);
    file.getline(data.date, MAX_DATE_LEN);
    file.getline(data.type, MAX_TYPE_LEN);

    file.close();
    return file.good();
}