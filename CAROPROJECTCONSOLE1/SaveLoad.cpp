#include "SaveLoad.h"
#include <fstream>
#include <windows.h>
#include <limits>
#include "Model.h" 

const std::string SAVE_DIR = "caro_saves\\";

bool FileExists(const std::string& filename) {
    struct stat buffer;
    return (stat(filename.c_str(), &buffer) == 0);
}

bool SaveGameToFile(const GameStateData& data, const std::string& filename) {
    CreateDirectoryA(SAVE_DIR.c_str(), NULL);
    std::string fullPath = SAVE_DIR + filename;
    std::ofstream file(fullPath);
    if (!file.is_open()) return false;

    file << data.currentPlayer << std::endl;
    file << data.cursorX << std::endl;
    file << data.cursorY << std::endl;
    file << data.p1Score << std::endl;
    file << data.p2Score << std::endl;
    file << data.moveCount << std::endl;
    file << data.timeLeft << std::endl;
    file << data.round << std::endl;

    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            file << data.board[i][j] << " ";
        }
        file << std::endl;
    }

    file << data.p1Name << std::endl;
    file << data.p2Name << std::endl;
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
    file >> data.p1Score;
    file >> data.p2Score;
    file >> data.moveCount;
    file >> data.timeLeft;
    file >> data.round;

    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            file >> data.board[i][j];
        }
    }

    file.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
    file.getline(data.p1Name, MAX_NAME_LEN);
    file.getline(data.p2Name, MAX_NAME_LEN);
    file.getline(data.name, MAX_NAME_LEN);
    file.getline(data.date, MAX_DATE_LEN);
    file.getline(data.type, MAX_TYPE_LEN);

    file.close();
    return file.good();
}

std::vector<GameStateData> GetSavedGames() {
    std::vector<GameStateData> saves;
    std::string searchPath = SAVE_DIR + "*.txt";
    WIN32_FIND_DATAA findData;
    HANDLE hFind = FindFirstFileA(searchPath.c_str(), &findData);
    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            if (!(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
                GameStateData temp;
                if (LoadGameFromFile(temp, findData.cFileName)) {
                    saves.push_back(temp);
                }
            }
        } while (FindNextFileA(hFind, &findData));
        FindClose(hFind);
    }
    return saves;
}

GameStateData GetCurrentGameStateData() {
    GameStateData currentData;
    currentData.currentPlayer = (_TURN ? -1 : 1);
    currentData.cursorX = _X;
    currentData.cursorY = _Y;
    currentData.timeLeft = _turnTimer;
    currentData.round = _round;
    strcpy_s(currentData.p1Name, MAX_NAME_LEN, _player1_name);
    strcpy_s(currentData.p2Name, MAX_NAME_LEN, _player2_name);
    currentData.p1Score = _player1_score;
    currentData.p2Score = _player2_score;
    currentData.moveCount = _moveCount;

    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            currentData.board[i][j] = _A[i][j].c;
        }
    }

    time_t now = time(0); tm ltm; localtime_s(&ltm, &now);
    strftime(currentData.date, sizeof(currentData.date), "%d-%m-%Y %H:%M", &ltm);
    strcpy_s(currentData.type, sizeof(currentData.type), "2 Players");

    return currentData;
}

void ApplyLoadedData(const GameStateData& data) {
    for (int i = 0; i < BOARD_SIZE; ++i)
        for (int j = 0; j < BOARD_SIZE; ++j)
            _A[i][j].c = data.board[i][j];

    _TURN = (data.currentPlayer == -1);
    _currentPlayer = (_TURN ? 1 : 2);
    _X = data.cursorX;
    _Y = data.cursorY;
    strcpy_s(_player1_name, MAX_NAME_LEN, data.p1Name);
    strcpy_s(_player2_name, MAX_NAME_LEN, data.p2Name);
    _player1_score = data.p1Score;
    _player2_score = data.p2Score;
    _moveCount = data.moveCount;
    _turnTimer = data.timeLeft;
    _round = data.round;
    _lastTimeCheck = time(0);
}

void PerformSave(const std::string& filename) {
    GameStateData dataToSave = GetCurrentGameStateData();
    std::string baseName = filename.substr(0, filename.find(".txt"));
    strcpy_s(dataToSave.name, sizeof(dataToSave.name), baseName.c_str());
    SaveGameToFile(dataToSave, filename);
}