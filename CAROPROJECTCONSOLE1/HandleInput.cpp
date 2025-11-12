#include "HandleInput.h"
#include "view.h"
#include <conio.h>
#include <ctype.h>
extern const int TOTAL_ITEMS;
extern const int TOTAL_PAUSE_ITEMS;

int HandleMainMenuInput() {
    ClearScreenWithColor(255, 255, 255);

    // 1. VẼ TOÀN BỘ MENU (CHỈ GỌI MỘT LẦN)
    // Hàm này sẽ vẽ LOGO, khung, hướng dẫn và mục menu đầu tiên (selected=0).
    DrawFullMenu(0); 
    
    int selected = 0;
    int old_selected = 0; // Biến lưu lại vị trí cũ để vẽ lại
    
    while (true) {
        // --- XỬ LÝ INPUT (Chờ nhấn phím) ---
        char key = toupper(_getch());

        // LƯU VỊ TRÍ CŨ TRƯỚC KHI CẬP NHẬT
        old_selected = selected; 

        if (key == 'W') {
            selected--;
            if (selected < 0) selected = TOTAL_ITEMS - 1; // Vòng lên cuối
        }
        else if (key == 'S') {
            selected++;
            if (selected >= TOTAL_ITEMS) selected = 0; // Vòng lại đầu
        }
        else if (key == 13) // Enter
        {
            return selected; // Trả về lựa chọn
        }
 
        // --- CẬP NHẬT GIAO DIỆN (CHỈ VẼ LẠI 2 MỤC) ---
        // Chỉ vẽ lại khi vị trí chọn thay đổi
        if (selected != old_selected) {
            
            // 1. VẼ LẠI MỤC CŨ: Bỏ trạng thái chọn (is_selected = false)
            DrawMenuItem(old_selected, false); 
            
            // 2. VẼ LẠI MỤC MỚI: Kích hoạt trạng thái chọn (is_selected = true)
            DrawMenuItem(selected, true);
        }
    }
}
int HandlePauseMenuInput() {
    ClearScreenWithColor(255, 255, 255);
    // 1. VẼ TOÀN BỘ PAUSE MENU (CHỈ GỌI MỘT LẦN)
    DrawFullPauseMenu(0); // Vẽ toàn bộ menu Pause, mục đầu tiên selected = 0

    int selected = 0;
    int old_selected = 0; // Lưu vị trí cũ

    while (true) {
        // --- NHẬN INPUT ---
        char key = toupper(_getch());

        old_selected = selected; // Lưu lại vị trí cũ

        if (key == 'W') {
            selected--;
            if (selected < 0) selected = TOTAL_PAUSE_ITEMS - 1; // Vòng về cuối
        }
        else if (key == 'S') {
            selected++;
            if (selected >= TOTAL_PAUSE_ITEMS) selected = 0; // Vòng lại đầu
        }
        else if (key == 13) // Enter
        {
            return selected; // Trả về lựa chọn hiện tại
        }
        else if (key == 27) // ESC
        {
            return 0; // ESC => quay lại game hoặc thoát
        }

        // --- VẼ LẠI MENU NẾU ĐỔI VỊ TRÍ ---
        if (selected != old_selected) {
            DrawPauseMenuItem(old_selected, false); // Mục cũ: bỏ chọn
            DrawPauseMenuItem(selected, true);      // Mục mới: chọn
        }
    }
}