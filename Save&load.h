#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <limits>
#include <cstdlib>        
#include <ctime>         
#include <windows.h>    
      
using namespace std;

// Thư mục mặc định để lưu các file game
const string SAVE_DIR = "caro_saves\\"; // Dùng dấu gạch chéo ngược cho Windows

// Tên file mặc định dùng khi người chơi chọn EXIT (tự động lưu)
const string QUICK_SAVE_NAME = "quick_save";

// --- KHAI BÁO HÀM MỚI ---
bool saveGame(const string& filename);
bool loadGame(const string& filename);
void listAndLoadGames(); 

// Hàm chuyển đổi _TURN (bool) sang int (vì file lưu int)
int getTurnInt();
void setTurnFromInt(int turnInt);