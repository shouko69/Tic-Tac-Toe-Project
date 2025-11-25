#include "View.h"
#include "Control.h"
#include "truecolor_utils.h"

int main() {
    // 1. Thiết lập Console
    SetConsoleFont(L"Terminal", 0, 15);
    ResizeConsoleWindow(CONSOLE_WIDTH, CONSOLE_HEIGHT);
    EnableTrueColor();
    FixConsoleWindow();
    InitConsole();
    CenterConsole();
    // 2. Intro Group6
    ShowGroupIntro();
    // 3 Chạy Intro
    StartIntro();
    // 4. Bắt đầu Game (Gọi sang Control.cpp)
    StartApp(); /*Kỹ thuật sử dụng: Table-Driven Design (Thiết kế điều khiển bằng bảng) : Thay vì sử dụng cấu trúc rẽ nhánh switch-case 
    truyền thống (gây rối và khó bảo trì), chương trình sử dụng std::map kết hợp với Con trỏ hàm (Function Pointer) để quản lý luồng đi của game.*/
    return 0;
}