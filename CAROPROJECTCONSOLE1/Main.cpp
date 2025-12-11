#include "View.h"
#include "Control.h"
#include "truecolor_utils.h"

int main() {
    SetConsoleFont(L"Terminal", 0, 15);
    ResizeConsoleWindow(CONSOLE_WIDTH, CONSOLE_HEIGHT);
    EnableTrueColor();
    FixConsoleWindow();
    InitConsole();
    CenterConsole();
    ShowGroupIntro();
    RunIntro();
    StartApp();
    return 0;
}