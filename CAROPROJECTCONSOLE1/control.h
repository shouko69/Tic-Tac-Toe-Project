#pragma once
#include <vector>
#include <string>
extern int _X, _Y; //toa do hien hanh tren ban co
extern int _COMMAND; // bien nhan gia tri phim nguoi dung
#define ESCAPE_KEY -2


void InitConsole();
void StartGame();
void ExitGame();
void MoveRight();
void MoveLeft();
void MoveUp();
void MoveDown();
void SetCursorVisible(bool visible);
void ShowCursorForBoard();

std::string GetRandomName();


/*
 * Chức năng: Xử lý input của người dùng trên màn hình New Game.
 * Hàm này quản lý việc di chuyển lên/xuống và lựa chọn.
 * Trả về:
 *  - 0, 1, hoặc 2: nếu người dùng nhấn Enter vào một lựa chọn.
 *  - -1: nếu không có lựa chọn nào được đưa ra (người dùng chưa nhấn Enter).
 */
int HandleNewGameMenuInput();

/*
 * Chức năng: Quản lý toàn bộ màn hình nhập tên người chơi.
 * Trả về:
 *  - true: Nếu cả hai người chơi đã xác nhận tên và sẵn sàng vào game.
 *  - false: Nếu người chơi chọn thoát ra màn hình trước đó.
 */
bool HandlePlayerNameInput();
// xu li input che do 2P
void Handle2PlayerGame();
int HandleGameOverScreen();
int HandleMainMenuInput();
int HandlePauseMenuInput();


