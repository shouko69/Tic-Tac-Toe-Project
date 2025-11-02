#pragma once
#include <iostream>
#include <windows.h>

inline void EnableTrueColor() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);
}

inline void SetColorRGB(int r, int g, int b) {
    std::cout << "\033[38;2;" << r << ";" << g << ";" << b << "m";
}

inline void SetBgRGB(int r, int g, int b) {
    std::cout << "\033[48;2;" << r << ";" << g << ";" << b << "m";
}

inline void ResetColor() {
    std::cout << "\033[0m";
}