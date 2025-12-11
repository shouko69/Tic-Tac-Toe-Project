
#pragma once
#include "Model.h"
#include <string>
#include <vector>
extern const std::string SAVE_DIR;
bool SaveGameToFile(const GameStateData& data, const std::string& filename);
bool LoadGameFromFile(GameStateData& data, const std::string& filename);

bool FileExists(const std::string& filename);
std::vector<GameStateData> GetSavedGames();
GameStateData GetCurrentGameStateData();
void ApplyLoadedData(const GameStateData& data);
void PerformSave(const std::string& filename);