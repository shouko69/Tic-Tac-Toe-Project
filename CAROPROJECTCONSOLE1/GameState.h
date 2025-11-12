#pragma once
#include "Model.h" // Để lấy hằng số BOARD_SIZE
#define MAX_NAME_LEN 20     // <= 20 ký tự + '\0'
#define MAX_DATE_LEN 20      // "DD-MM-YYYY HH:MM"
#define MAX_TYPE_LEN 12      // "1 Player" / "2 Players"
enum GameState {
	MENU,
	PLAY,
	LOAD,
	SETTINGS,
	GUIDE,
	ABOUT,
	EXIT,
	PAUSE,
	SAVE,
};

struct GameStateData {
	int board[BOARD_SIZE][BOARD_SIZE]; // Trạng thái bàn cờ (_A)
	int currentPlayer;                 // Lượt của người chơi hiện tại (_TURN)
	int cursorX;                       // Vị trí con trỏ X (_X)
	int cursorY;                       // Vị trí con trỏ Y (_Y)
	bool loadSuccess;
	// Thêm bất kỳ thông tin nào khác bạn muốn lưu, ví dụ: điểm số, thời gian...
	char name[MAX_NAME_LEN];    // NAME -> Tên file save
	char date[MAX_DATE_LEN];    // DATE -> "DD-MM-2025 14:12"
	char type[MAX_TYPE_LEN];    // TYPE -> "1 Player" / "2 Players"
};

extern GameState currentState;


