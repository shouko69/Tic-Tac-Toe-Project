/********************************************************************************
 * SaveLoadUI.cpp
 *
 * Chịu trách nhiệm cho tất cả các giao diện người dùng (UI) liên quan đến
 * việc Lưu và Tải game.
 ********************************************************************************/

#include "SaveLoadUI.h"
#include "SaveLoad.h"
#include "GameState.h"
#include "Model.h"
#include "View.h"
#include "truecolor_utils.h"
#include "control.h"

#include <iostream>
#include <vector>
#include <string>
#include <conio.h>
#include <windows.h>
#include <algorithm>
#include <ctime>
#include <iomanip>
#include <cstring>

 // =================================================================================
 // 1. CÁC HẰNG SỐ CẤU HÌNH GIAO DIỆN
 // =================================================================================

 // Khai báo lại các hằng số chiều rộng cột để các hàm có thể sử dụng
const int COL_WIDTH_NAME = 25;
const int COL_WIDTH_DATE = 20;
const int COL_WIDTH_TYPE = 15;
GameStateData GetCurrentGameStateData();
void ApplyLoadedData(const GameStateData& data);
void ShowSaveGameScreen();
bool ShowLoadGameScreen();
void ShowOverwriteScreen();
void ShowNewSaveScreen();
void ShowConfirmationScreen(const std::string& filename);
void DrawCenteredCell(const char* text, int cellX, int cellY, int cellWidth, int fg_r, int fg_g, int fg_b, int bg_r, int bg_g, int bg_b);
void DrawTableFrame(int startX, int startY, int totalContentRows, int nameWidth, int dateWidth, int typeWidth);
void PerformSave(const std::string& filename);


// =================================================================================
// 3. CÁC HÀM GIAO DIỆN CHÍNH
// =================================================================================

/**
 * @brief Hiển thị màn hình Tải Game với danh sách cuộn 3 slot.
 * @return true nếu người dùng chọn tải, false nếu chọn BACK.
 */
bool ShowLoadGameScreen() {
    std::vector<GameStateData> saves;
    // ... (Phần code tải file của bạn giữ nguyên) ...
    std::string searchPath = SAVE_DIR + "*.txt";
    WIN32_FIND_DATAA findData;
    HANDLE hFind = FindFirstFileA(searchPath.c_str(), &findData);
    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            if (!(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
                GameStateData temp;
                if (LoadGameFromFile(temp, findData.cFileName)) { saves.push_back(temp); }
            }
        } while (FindNextFileA(hFind, &findData));
        FindClose(hFind);
    }


    ClearScreenWithColor(33, 0, 84);
    const int totalSaves = saves.size();

    if (saves.empty()) {
        const char* msg = "No save files found. Press ESC to return...";
        GotoXY((CONSOLE_WIDTH - strlen(msg)) / 2, CONSOLE_HEIGHT / 2);
        SetColorRGB(255, 255, 255); std::cout << msg;
        while (_getch() != 27);
        ResetColor(); return false;
    }

    const int MAX_VISIBLE_SAVES = 3;
    int viewOffset = 0, selected = 0;
    const int totalOptions = totalSaves + 1;

    const int MASTER_UI_START_X = 20;
    const int MASTER_UI_START_Y = 10;

    const int tableTotalWidth = COL_WIDTH_NAME + COL_WIDTH_DATE + COL_WIDTH_TYPE + 4;
    const int tableStartX = MASTER_UI_START_X;
    const int tableStartY = MASTER_UI_START_Y;
    const int nameX = tableStartX + 1, dateX = nameX + COL_WIDTH_NAME + 1, typeX = dateX + COL_WIDTH_DATE + 1;

    while (true) {
        // SỬA ĐỔI: Gọi hàm vẽ khung mới. Hàm này sẽ lo toàn bộ các đường kẻ.
        // Lưu ý tham số thứ 3 là số hàng nội dung, không phải số hàng ký tự.
        DrawTableFrame(tableStartX, tableStartY, MAX_VISIBLE_SAVES, COL_WIDTH_NAME, COL_WIDTH_DATE, COL_WIDTH_TYPE);

        // Vẽ header
        DrawCenteredCell("NAME", nameX, tableStartY + 1, COL_WIDTH_NAME, 0, 0, 0, 230, 230, 230);
        DrawCenteredCell("DATE", dateX, tableStartY + 1, COL_WIDTH_DATE, 0, 0, 0, 230, 230, 230);
        DrawCenteredCell("TYPE", typeX, tableStartY + 1, COL_WIDTH_TYPE, 0, 0, 0, 230, 230, 230);

        // Vòng lặp vẽ nội dung các ô
        for (int i = 0; i < MAX_VISIBLE_SAVES; ++i) {
            int actualIndex = viewOffset + i;
            int rowY = tableStartY + 3 + (i * 2); // Tọa độ Y cho nội dung vẫn như cũ

            if (actualIndex < totalSaves) {
                bool isRowSelected = (actualIndex == selected);

                // SỬA ĐỔI: Xác định màu nền cho cả hàng
                int bg_r, bg_g, bg_b;
                if (isRowSelected) {
                    // Màu xanh highlight khi được chọn
                    bg_r = 5; bg_g = 157; bg_b = 184;
                }
                else {
                    // Màu nền mặc định
                    bg_r = 48; bg_g = 25; bg_b = 52;
                }

               

                // Vẽ chuỗi đó ra cột NAME
                DrawCenteredCell(saves[actualIndex].name, nameX, rowY, COL_WIDTH_NAME, 255, 255, 255, bg_r, bg_g, bg_b);
                DrawCenteredCell(saves[actualIndex].date, dateX, rowY, COL_WIDTH_DATE, 255, 255, 255, bg_r, bg_g, bg_b);
                DrawCenteredCell(saves[actualIndex].type, typeX, rowY, COL_WIDTH_TYPE, 255, 255, 255, bg_r, bg_g, bg_b);

                // XÓA BỎ: Không cần vẽ lại đường kẻ ở đây nữa vì DrawTableFrame đã xử lý
            }
            else {
                // Vẽ các ô trống, không cần vẽ đường kẻ
                DrawCenteredCell("", nameX, rowY, COL_WIDTH_NAME, 0, 0, 0, 48, 25, 52);
                DrawCenteredCell("", dateX, rowY, COL_WIDTH_DATE, 0, 0, 0, 48, 25, 52);
                DrawCenteredCell("", typeX, rowY, COL_WIDTH_TYPE, 0, 0, 0, 48, 25, 52);
                // XÓA BỎ: Không cần vẽ lại đường kẻ ở đây nữa
            }
        }

        // ... (Phần code vẽ nút BACK và xử lý input của bạn giữ nguyên) ...
        bool isBackSelected = (selected == totalSaves);
        const char* backText = "BACK";
        int backY = tableStartY + 3 + (MAX_VISIBLE_SAVES * 2) + 2;
        int backStartX = tableStartX + (tableTotalWidth / 2) - 4; // Căn giữa với bảng
        if (isBackSelected) {
            DrawCenteredCell(backText, backStartX, backY, 8, 255, 100, 180, 0, 0, 0);
        }
        else {
            DrawCenteredCell(backText, backStartX, backY, 8, 255, 255, 255, 33, 0, 84);
        }
        char key = toupper(_getch());
        if (key == 27) { ResetColor(); return false; }
        if (key == 'W') { selected = (selected - 1 + totalOptions) % totalOptions; }
        else if (key == 'S') { selected = (selected + 1) % totalOptions; }
        else if (key == 13) {
            if (isBackSelected) {
                ResetColor();
                return false; // Trả về false khi chọn BACK
            }
            else {
                ApplyLoadedData(saves[selected]); // HÀNH ĐỘNG: Tải game
                ResetColor();
                return true; // Trả về true khi tải thành công
            }
        }

        if (selected < totalSaves) {
            if (selected < viewOffset) { viewOffset = selected; }
            if (selected >= viewOffset + MAX_VISIBLE_SAVES) { viewOffset = selected - MAX_VISIBLE_SAVES + 1; }
        }
    }
}

/**
 * @brief Hiển thị màn hình Ghi đè với logic tương tự màn hình Load.
 */
 /**
  * @brief Hiển thị màn hình Ghi đè. Cấu trúc hàm này tương tự ShowLoadGameScreen.
  */
void ShowOverwriteScreen() {
    std::vector<GameStateData> saves;
    // Tìm và tải tất cả các file save hiện có
    std::string searchPath = SAVE_DIR + "*.txt";
    WIN32_FIND_DATAA findData;
    HANDLE hFind = FindFirstFileA(searchPath.c_str(), &findData);
    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            if (!(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
                GameStateData temp;
                if (LoadGameFromFile(temp, findData.cFileName)) { saves.push_back(temp); }
            }
        } while (FindNextFileA(hFind, &findData));
        FindClose(hFind);
    }

    ClearScreenWithColor(33, 0, 84);
    const int totalSaves = saves.size();

    // Trường hợp không có file save nào để ghi đè
    if (saves.empty()) {
        const char* msg = "No save files to overwrite. Press ESC to return...";
        GotoXY((CONSOLE_WIDTH - strlen(msg)) / 2, CONSOLE_HEIGHT / 2);
        SetColorRGB(255, 255, 255); std::cout << msg;
        while (_getch() != 27);
        ResetColor(); return;
    }

    // Các biến điều khiển giao diện và lựa chọn
    const int MAX_VISIBLE_SAVES = 3;
    int viewOffset = 0, selected = 0;
    const int totalOptions = totalSaves + 1; // +1 cho nút BACK

    // Các hằng số tọa độ và kích thước
    const int MASTER_UI_START_X = 20;
    const int MASTER_UI_START_Y = 10;
    const int tableTotalWidth = COL_WIDTH_NAME + COL_WIDTH_DATE + COL_WIDTH_TYPE + 4;
    const int tableStartX = MASTER_UI_START_X;
    const int tableStartY = MASTER_UI_START_Y;
    const int nameX = tableStartX + 1, dateX = nameX + COL_WIDTH_NAME + 1, typeX = dateX + COL_WIDTH_DATE + 1;

    // Vòng lặp chính của giao diện
    while (true) {
        // Vẽ toàn bộ khung bảng
        DrawTableFrame(tableStartX, tableStartY, MAX_VISIBLE_SAVES, COL_WIDTH_NAME, COL_WIDTH_DATE, COL_WIDTH_TYPE);

        // Vẽ header của bảng
        DrawCenteredCell("NAME", nameX, tableStartY + 1, COL_WIDTH_NAME, 0, 0, 0, 230, 230, 230);
        DrawCenteredCell("DATE", dateX, tableStartY + 1, COL_WIDTH_DATE, 0, 0, 0, 230, 230, 230);
        DrawCenteredCell("TYPE", typeX, tableStartY + 1, COL_WIDTH_TYPE, 0, 0, 0, 230, 230, 230);

        // Vòng lặp vẽ nội dung các hàng save
        for (int i = 0; i < MAX_VISIBLE_SAVES; ++i) {
            int actualIndex = viewOffset + i;
            int rowY = tableStartY + 3 + (i * 2);

            if (actualIndex < totalSaves) {
                bool isRowSelected = (actualIndex == selected);

                // Xác định màu nền cho cả hàng
                int bg_r, bg_g, bg_b;
                if (isRowSelected) {
                    // Màu đỏ highlight khi được chọn để ghi đè
                    bg_r = 200; bg_g = 0; bg_b = 0;
                }
                else {
                    // Màu nền mặc định
                    bg_r = 48; bg_g = 25; bg_b = 52;
                }

                // Áp dụng cùng màu nền cho tất cả các ô trong hàng
                DrawCenteredCell(saves[actualIndex].name, nameX, rowY, COL_WIDTH_NAME, 255, 255, 255, bg_r, bg_g, bg_b);
                DrawCenteredCell(saves[actualIndex].date, dateX, rowY, COL_WIDTH_DATE, 255, 255, 255, bg_r, bg_g, bg_b);
                DrawCenteredCell(saves[actualIndex].type, typeX, rowY, COL_WIDTH_TYPE, 255, 255, 255, bg_r, bg_g, bg_b);
            }
            else {
                // Vẽ các ô trống nếu không có đủ file save để lấp đầy
                DrawCenteredCell("", nameX, rowY, COL_WIDTH_NAME, 0, 0, 0, 48, 25, 52);
                DrawCenteredCell("", dateX, rowY, COL_WIDTH_DATE, 0, 0, 0, 48, 25, 52);
                DrawCenteredCell("", typeX, rowY, COL_WIDTH_TYPE, 0, 0, 0, 48, 25, 52);
            }
        }

        // Vẽ nút BACK
        bool isBackSelected = (selected == totalSaves);
        const char* backText = "BACK";
        int backY = tableStartY + 3 + (MAX_VISIBLE_SAVES * 2) + 2;
        int backStartX = tableStartX + (tableTotalWidth / 2) - 4; // Căn giữa với bảng
        if (isBackSelected) {
            DrawCenteredCell(backText, backStartX, backY, 8, 255, 100, 180, 0, 0, 0);
        }
        else {
            DrawCenteredCell(backText, backStartX, backY, 8, 255, 255, 255, 33, 0, 84);
        }

        // Xử lý input từ người dùng
        char key = toupper(_getch());
        if (key == 27) { ResetColor(); return; } // Phím ESC
        if (key == 'W') { selected = (selected - 1 + totalOptions) % totalOptions; }
        else if (key == 'S') { selected = (selected + 1) % totalOptions; }
        else if (key == 13) { // Phím Enter
            if (isBackSelected) {
                ResetColor();
                return; // Thoát và quay về menu trước
            }
            else {
                // Thực hiện hành động ghi đè
                std::string filenameToOverwrite = std::string(saves[selected].name) + ".txt";
                PerformSave(filenameToOverwrite);
                ResetColor();
                return; // Thoát sau khi ghi đè thành công
            }
        }

        // Cập nhật viewOffset để cuộn danh sách
        if (selected < totalSaves) {
            if (selected < viewOffset) {
                viewOffset = selected;
            }
            if (selected >= viewOffset + MAX_VISIBLE_SAVES) {
                viewOffset = selected - MAX_VISIBLE_SAVES + 1;
            }
        }
    }
}


// =================================================================================
// 4. CÁC HÀM GIAO DIỆN PHỤ
// =================================================================================

/**
 * @brief Hiển thị menu lựa chọn chính cho việc Save Game. Đã sửa lỗi hiển thị.
 */
void ShowSaveGameScreen() {
    int selectedOption = 0;
    const int totalOptions = 3;
    const int MENU_WIDTH = 30;
    const int startX = (CONSOLE_WIDTH - MENU_WIDTH) / 2;
    const int startY = (CONSOLE_HEIGHT / 2) - 2;

    ClearScreenWithColor(255, 255, 255);

    while (true) {
        for (int i = 0; i < totalOptions; ++i) {
            GotoXY(startX, startY + i);
            SetColorRGB(0, 0, 0);
            std::string text;
            if (i == 0) text = "Save to New File";
            else if (i == 1) text = "Overwrite Existing Save";
            else if (i == 2) text = "Back";
            std::cout << std::left << std::setw(MENU_WIDTH) << text;
        }

        SetColorRGB(255, 100, 180);
        GotoXY(startX, startY + selectedOption);
        std::string selectedText;
        if (selectedOption == 0) selectedText = ">> Save to New File";
        else if (selectedOption == 1) selectedText = ">> Overwrite Existing Save";
        else if (selectedOption == 2) selectedText = ">> Back";
        std::cout << std::left << std::setw(MENU_WIDTH) << selectedText;

        char key = toupper(_getch());
        if (key == 'W') { selectedOption = (selectedOption - 1 + totalOptions) % totalOptions; }
        else if (key == 'S') { selectedOption = (selectedOption + 1) % totalOptions; } // SỬA LỖI: selected -> selectedOption
        else if (key == 13) {
            switch (selectedOption) {
            case 0: ShowNewSaveScreen(); break;
            case 1: ShowOverwriteScreen(); break;
            case 2: ResetColor(); return;
            }
            ClearScreenWithColor(255, 255, 255);
        }
    }
}

/**
 * @brief Hiển thị màn hình cho phép người dùng nhập tên file save mới.
 */
void ShowNewSaveScreen() {
    ClearScreenWithColor(255, 255, 255);
    SetColorRGB(0, 0, 0);
    const int contentWidth = 25;
    const int startX = (CONSOLE_WIDTH - contentWidth) / 2;
    const int startY = (CONSOLE_HEIGHT / 2) - 2;

    GotoXY(startX, startY);     std::cout << "Enter save name:";
    GotoXY(startX, startY + 1); std::cout << "(Less than 20 characters)";
    GotoXY(startX, startY + 3); std::cout << "____________________";
    GotoXY(startX, startY + 3);

    std::string saveName = "";
    char ch;
    while (true) {
        ch = _getch();
        if (ch == 13) { if (!saveName.empty()) break; }
        else if (ch == 27) { ResetColor(); return; }
        else if (ch == 8) { if (!saveName.empty()) { saveName.pop_back(); std::cout << "\b \b"; } }
        else if (saveName.length() < 20 && isalnum(ch)) { saveName += ch; std::cout << ch; }
    }
    PerformSave(saveName + ".txt");
}

/**
 * @brief Hiển thị một màn hình thông báo ngắn gọn.
 */
void ShowConfirmationScreen(const std::string& filename) {
    ClearScreenWithColor(255, 255, 255);
    SetColorRGB(0, 0, 0);
    const int startY = CONSOLE_HEIGHT / 2 - 1;
    std::string msg1 = "Game state saved to: " + filename;
    std::string msg2 = "Press any key to continue...";
    GotoXY((CONSOLE_WIDTH - msg1.length()) / 2, startY); std::cout << msg1;
    GotoXY((CONSOLE_WIDTH - msg2.length()) / 2, startY + 2); std::cout << msg2;
    _getch();
    ResetColor();
}


// =================================================================================
// 5. CÁC HÀM XỬ LÝ LOGIC NỀN
// =================================================================================

/**
 * @brief Thực hiện hành động lưu game.
 */
void PerformSave(const std::string& filename) {
    GameStateData dataToSave = GetCurrentGameStateData();
    std::string baseName = filename.substr(0, filename.find(".txt"));
    strcpy_s(dataToSave.name, sizeof(dataToSave.name), baseName.c_str());
    if (SaveGameToFile(dataToSave, filename)) {
        ShowConfirmationScreen(filename);
    }
}

/**
 * @brief Lấy trạng thái hiện tại của game và đóng gói vào struct.
 */
GameStateData GetCurrentGameStateData() {
    GameStateData currentData;

    // 1. Dữ liệu cơ bản (Cũ)
    currentData.currentPlayer = (_TURN ? -1 : 1);
    currentData.cursorX = _X;
    currentData.cursorY = _Y;

    // --- 2. CHỖ NỐI DÂY QUAN TRỌNG (THÊM MỚI VÀO ĐÂY) ---
    // Copy tên thật từ biến toàn cục vào gói tin save
    strcpy_s(currentData.p1Name, MAX_NAME_LEN, _player1_name);
    strcpy_s(currentData.p2Name, MAX_NAME_LEN, _player2_name);

    // Copy điểm số và lượt đi
    currentData.p1Score = _player1_score;
    currentData.p2Score = _player2_score;
    currentData.moveCount = _moveCount;
    // ---------------------------------------------------

    // 3. Copy bàn cờ (Giữ nguyên)
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            currentData.board[i][j] = _A[i][j].c;
        }
    }

    // 4. Metadata (Giữ nguyên)
    time_t now = time(0); tm ltm; localtime_s(&ltm, &now);
    strftime(currentData.date, sizeof(currentData.date), "%d-%m-%Y %H:%M", &ltm);
    strcpy_s(currentData.type, sizeof(currentData.type), "2 Players");

    return currentData;
}

/**
 * @brief Áp dụng dữ liệu từ file save vào game.
 */
void ApplyLoadedData(const GameStateData& data) {
    // 1. Bung bàn cờ (Giữ nguyên)
    for (int i = 0; i < BOARD_SIZE; ++i)
        for (int j = 0; j < BOARD_SIZE; ++j)
            _A[i][j].c = data.board[i][j];

    // 2. Bung dữ liệu cơ bản
    _TURN = (data.currentPlayer == -1);
    _currentPlayer = (_TURN ? 1 : 2); // Cập nhật biến _currentPlayer cho đồng bộ
    _X = data.cursorX;
    _Y = data.cursorY;

    // --- 3. CHỖ NỐI DÂY QUAN TRỌNG (THÊM MỚI) ---
    // Lấy tên từ file đè vào biến game
    strcpy_s(_player1_name, MAX_NAME_LEN, data.p1Name);
    strcpy_s(_player2_name, MAX_NAME_LEN, data.p2Name);

    // Lấy điểm số
    _player1_score = data.p1Score;
    _player2_score = data.p2Score;
    _moveCount = data.moveCount;
    // -------------------------------------------
}




// =================================================================================
// 6.  CÁC HÀM HELPER CHUYÊN VẼ GIAO DIỆN
// =================================================================================

/**
 * @brief Vẽ một ô chữ nhật có màu nền và tự động căn chữ vào chính giữa ô đó.
 *        Đây là hàm cơ bản để xây dựng toàn bộ giao diện bảng.
 *
 * @param text Chuỗi ký tự (const char*) cần hiển thị.
 * @param cellX, cellY Tọa độ góc trên bên trái của ô (tính theo ký tự).
 * @param cellWidth Chiều rộng của ô (tính bằng số ký tự).
 * @param fg_r, fg_g, fg_b Các giá trị màu RGB cho chữ (Foreground).
 * @param bg_r, bg_g, bg_b Các giá trị màu RGB cho nền (Background).
 */
void DrawCenteredCell(const char* text, int cellX, int cellY, int cellWidth, int fg_r, int fg_g, int fg_b, int bg_r, int bg_g, int bg_b) {
    // Bước 1: Di chuyển con trỏ và đặt màu nền.
    GotoXY(cellX, cellY);
    SetBgRGB(bg_r, bg_g, bg_b);

    // Bước 2: "Sơn" toàn bộ ô bằng cách in các khoảng trắng.
    // Điều này đảm bảo màu nền lấp đầy từ mép trái đến mép phải của ô.
    for (int i = 0; i < cellWidth; ++i) {
        std::cout << " ";
    }

    // Bước 3: Tính toán vị trí để viết chữ cho nó nằm giữa ô.
    int textLen = strlen(text);
    int padding = cellWidth - textLen;
    if (padding < 0) padding = 0;
    int textX = cellX + (padding / 2);

    // Bước 4: Di chuyển con trỏ đến vị trí đã tính và in chữ với màu đã chọn.
    GotoXY(textX, cellY);
    SetColorRGB(fg_r, fg_g, fg_b); // Đặt màu chữ
    // SetBgRGB vẫn giữ nguyên màu nền đã đặt ở Bước 1.
    std::cout << text;
}


//*
//*@brief Vẽ toàn bộ khung cho bảng hiển thị, bao gồm tất cả các đường kẻ ngang và dọc.
//*
//* @param startX, startY Tọa độ góc trên bên trái của toàn bộ khung.
//* @param numContentRows Số lượng hàng nội dung sẽ được hiển thị(ví dụ : MAX_VISIBLE_SAVES).
//* @param nameWidth, dateWidth, typeWidth Chiều rộng của mỗi cột.
//* /
void DrawTableFrame(int startX, int startY, int numContentRows, int nameWidth, int dateWidth, int typeWidth) {
    // Đặt màu mặc định cho khung
    SetColorRGB(255, 255, 255);
    SetBgRGB(33, 0, 84);

    // Tọa độ các đường kẻ dọc
    int x1 = startX;
    int x2 = x1 + 1 + nameWidth;
    int x3 = x2 + 1 + dateWidth;
    int x4 = x3 + 1 + typeWidth;

    // --- VẼ KHUNG ---

    // 1. Dòng trên cùng: ┌───┬───┬───┐
    GotoXY(x1, startY); std::cout << "┌"; for (int i = 0; i < nameWidth; ++i) std::cout << "─";
    std::cout << "┬"; for (int i = 0; i < dateWidth; ++i) std::cout << "─";
    std::cout << "┬"; for (int i = 0; i < typeWidth; ++i) std::cout << "─";
    std::cout << "┐";

    // 2. Hàng Header (chỉ vẽ đường kẻ dọc)
    int headerContentY = startY + 1;
    GotoXY(x1, headerContentY); std::cout << "│";
    GotoXY(x2, headerContentY); std::cout << "│";
    GotoXY(x3, headerContentY); std::cout << "│";
    GotoXY(x4, headerContentY); std::cout << "│";

    // 3. Dòng phân cách header: ├───┼───┼───┤
    int headerSeparatorY = startY + 2;
    GotoXY(x1, headerSeparatorY); std::cout << "├"; for (int i = 0; i < nameWidth; ++i) std::cout << "─";
    std::cout << "┼"; for (int i = 0; i < dateWidth; ++i) std::cout << "─";
    std::cout << "┼"; for (int i = 0; i < typeWidth; ++i) std::cout << "─";
    std::cout << "┤";

    // 4. Vẽ tất cả các hàng nội dung và đường phân cách của chúng
    for (int i = 0; i < numContentRows; ++i) {
        int contentY = headerSeparatorY + 1 + (i * 2);
        int separatorY = contentY + 1;

        // Vẽ các đường kẻ dọc cho hàng nội dung
        GotoXY(x1, contentY); std::cout << "│";
        GotoXY(x2, contentY); std::cout << "│";
        GotoXY(x3, contentY); std::cout << "│";
        GotoXY(x4, contentY); std::cout << "│";

        // Chỉ vẽ đường phân cách cho các hàng không phải hàng cuối cùng
        if (i < numContentRows - 1) {
            GotoXY(x1, separatorY); std::cout << "├"; for (int k = 0; k < nameWidth; ++k) std::cout << "─";
            std::cout << "┼"; for (int k = 0; k < dateWidth; ++k) std::cout << "─";
            std::cout << "┼"; for (int k = 0; k < typeWidth; ++k) std::cout << "─";
            std::cout << "┤";
        }
    }

    // 5. Dòng dưới cùng: └───┴───┴───┘
    int bottomY = headerSeparatorY + (numContentRows * 2);
    GotoXY(x1, bottomY); std::cout << "└"; for (int i = 0; i < nameWidth; ++i) std::cout << "─";
    std::cout << "┴"; for (int i = 0; i < dateWidth; ++i) std::cout << "─";
    std::cout << "┴"; for (int i = 0; i < typeWidth; ++i) std::cout << "─";
    std::cout << "┘";
}