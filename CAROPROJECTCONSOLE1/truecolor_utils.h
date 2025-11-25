// truecolor_utils.h
#pragma once
#include <iostream>
#include <windows.h>

//kích hoạt màu RGB
inline void EnableTrueColor() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);
}
// đặt màu chữ
inline void SetColorRGB(int r, int g, int b) {
    std::cout << "\033[38;2;" << r << ";" << g << ";" << b << "m";
} 
// đặt màu background
inline void SetBgRGB(int r, int g, int b) {
    std::cout << "\033[48;2;" << r << ";" << g << ";" << b << "m";
}
inline void ResetColor() {
    std::cout << "\x1b[0m";
} //reset màu
// Hàm này chỉ dùng KHI ĐANG Ở chế độ _O_U16TEXT
inline void SetColorW(int r, int g, int b) {
    std::wcout << L"\x1b[38;2;" << r << L";" << g << L";" << b << L"m";
}
inline void SetBgW(int r, int g, int b) {
    std::wcout << L"\x1b[48;2;" << r << L";" << g << L";" << b << L"m";
}
inline void ResetColorW() {
    std::wcout << L"\x1b[0m";
}