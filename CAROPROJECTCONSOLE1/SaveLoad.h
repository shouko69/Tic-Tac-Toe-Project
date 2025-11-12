#pragma once
#include <string>
#include "GameState.h"

// Thư mục mặc định để lưu, có thể truy cập từ nơi khác
extern const std::string SAVE_DIR;

bool SaveGameToFile(const GameStateData& data, const std::string& filename);
bool LoadGameFromFile(GameStateData& data, const std::string& filename);