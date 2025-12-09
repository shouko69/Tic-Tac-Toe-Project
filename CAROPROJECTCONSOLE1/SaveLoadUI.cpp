/********************************************************************************
 * SaveLoadUI.cpp
 *
 * Chịu trách nhiệm cho tất cả các giao diện người dùng (UI) liên quan đến
 * việc Lưu và Tải game.
 ********************************************************************************/
#include "SaveLoadUI.h"
#include "SaveLoad.h"
#include "GameAssets.h"
 // =================================================================================
 // 1. CÁC HẰNG SỐ CẤU HÌNH GIAO DIỆN
 // =================================================================================

 // Khai báo lại các hằng số chiều rộng cột để các hàm có thể sử dụng
const int COL_WIDTH_NAME = 25;
const int COL_WIDTH_DATE = 20;
const int COL_WIDTH_TYPE = 15;


bool FileExists(const std::string& filename) {
    struct stat buffer;
    return (stat(filename.c_str(), &buffer) == 0);
}
const std::string SAVE_OPTIONS[] = {
    "Save to New File",
    "Overwrite Existing",
    "Back"
};
// =================================================================================
// 3. CÁC HÀM GIAO DIỆN CHÍNH
// =================================================================================

/**
 * @brief Hiển thị màn hình Tải Game với danh sách cuộn 3 slot.
 * @return true nếu người dùng chọn tải, false nếu chọn BACK.
 */
bool ShowLoadGameScreen() {
    std::vector<GameStateData> saves;
    // --- LOAD FILE LIST (GIỮ NGUYÊN) ---
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

    ClearScreenWithColor(97, 100, 151);
    const int totalSaves = saves.size();

    if (saves.empty()) {
        const char* msg = "No save files found. Press ESC to return...";
        GotoXY((CONSOLE_WIDTH - strlen(msg)) / 2, CONSOLE_HEIGHT / 2);
        SetColorRGB(255, 255, 255); std::cout << msg;
        while (_getch() != 27);
        ResetColor(); return false;
    }

    // --- CẤU HÌNH UI ---
    const int SCREEN_W = 165;
    const int SCREEN_H = 110;
    const int MAX_VISIBLE_SAVES = 3;
    const int totalOptions = totalSaves + 1;
    const int ROW_SPACING = 4;

    int viewOffset = 0, selected = 0;

    const int tableTotalWidth = COL_WIDTH_NAME + COL_WIDTH_DATE + COL_WIDTH_TYPE + 4;
    const int contentHeight = 3 + (MAX_VISIBLE_SAVES * ROW_SPACING);
    const int MASTER_UI_START_X = (SCREEN_W - tableTotalWidth) / 2;
    const int MASTER_UI_START_Y = ((SCREEN_H - contentHeight) / 2) - 31;

    // --- TỌA ĐỘ ---
    const int tableStartX = MASTER_UI_START_X;
    const int headerY = MASTER_UI_START_Y;
    const int listStartY = MASTER_UI_START_Y + 3;

    const int nameX = tableStartX + 1;
    const int dateX = nameX + COL_WIDTH_NAME + 1;
    const int typeX = dateX + COL_WIDTH_DATE + 1;

    const int buttonBackX = typeX;
    const int buttonBackY = listStartY + (MAX_VISIBLE_SAVES * ROW_SPACING) + 2;

    // [QUAN TRỌNG]: VẼ NỀN TĨNH 1 LẦN DUY NHẤT Ở ĐÂY (BÊN NGOÀI WHILE)
    SetColorRGB(97, 100, 151);
    DrawLargebackground1(0, 0);
    DrawLoadgame(25, 2);
    DrawBackloadframe(100, 33);
    DrawLoadframe(((SCREEN_W - tableTotalWidth) / 2) - 3, ((SCREEN_H - contentHeight) / 2) - 33);

    // Vẽ Header 1 lần luôn cũng được vì nó không đổi
    DrawCenteredCell("NAME", nameX, headerY, COL_WIDTH_NAME, 0, 255, 255, 218, 66, 76);
    DrawCenteredCell("DATE", dateX, headerY, COL_WIDTH_DATE, 0, 255, 255, 218, 66, 76);
    DrawCenteredCell("TYPE", typeX, headerY, COL_WIDTH_TYPE, 0, 255, 255, 218, 66, 76);

    while (true) {
        // [LƯU Ý]: Trong này KHÔNG gọi lại hàm vẽ Background nữa

        // 1. VẼ DANH SÁCH FILE SAVE (Chỉ vẽ lại phần nội dung thay đổi)
        for (int i = 0; i < MAX_VISIBLE_SAVES; ++i) {
            int actualIndex = viewOffset + i;
            int rowY = listStartY + (i * ROW_SPACING);

            if (actualIndex < totalSaves) {
                bool isRowSelected = (actualIndex == selected);
                int bg_r = 218, bg_g = 66, bg_b = 76;
                int text_r, text_g, text_b;
                std::string displayName;

                if (isRowSelected) {
                    text_r = 255; text_g = 255; text_b = 255;
                    displayName = std::string(1, char(15)) + " " + saves[actualIndex].name + " " + std::string(1, char(15));
                }
                else {
                    text_r = 255; text_g = 255; text_b = 0;
                    displayName = saves[actualIndex].name;
                }

                DrawCenteredCell(displayName.c_str(), nameX, rowY, COL_WIDTH_NAME, text_r, text_g, text_b, bg_r, bg_g, bg_b);
                DrawCenteredCell(saves[actualIndex].date, dateX, rowY, COL_WIDTH_DATE, text_r, text_g, text_b, bg_r, bg_g, bg_b);
                DrawCenteredCell(saves[actualIndex].type, typeX, rowY, COL_WIDTH_TYPE, text_r, text_g, text_b, bg_r, bg_g, bg_b);
            }
            else {
                // Xóa dòng thừa bằng màu nền background
                DrawCenteredCell("", nameX, rowY, COL_WIDTH_NAME, 0, 0, 0, 97, 100, 151);
                DrawCenteredCell("", dateX, rowY, COL_WIDTH_DATE, 0, 0, 0, 97, 100, 151);
                DrawCenteredCell("", typeX, rowY, COL_WIDTH_TYPE, 0, 0, 0, 97, 100, 151);
            }
        }

        // 2. VẼ NÚT BACK
        bool isBackSelected = (selected == totalSaves);
        int back_bg_r = 218, back_bg_g = 66, back_bg_b = 76;
        int back_txt_r, back_txt_g, back_txt_b;
        std::string backLabel;

        // Padding gốc (lề ngoài cùng)
        std::string pad5 = "     ";

        if (isBackSelected) {
            back_txt_r = 255; back_txt_g = 255; back_txt_b = 255;
            // Cấu trúc: [Pad5] + [Icon] + [Space] + BACK + [Space] + [Icon] + [Pad5]
            // Tổng cộng phần "thịt" thêm vào là: Icon + Space (2 ký tự mỗi bên)
            backLabel = "   " + std::string(1, char(15)) + " BACK " + std::string(1, char(15)) + "   ";
        }
        else {
            back_txt_r = 255; back_txt_g = 255; back_txt_b = 0;
            // Cấu trúc: [Pad5] + [Space] + [Space] + BACK + [Space] + [Space] + [Pad5]
            // Thêm 2 dấu cách mỗi bên để bù cho độ rộng của (Icon + Space) bên trên
            // Giúp khung hình chữ nhật KHÔNG bị co giãn khi chọn/bỏ chọn
            backLabel = pad5 + "BACK" + pad5;
        }

        int textLen = backLabel.length();
        int centeredX = (typeX + (30 - textLen) / 2) - 5;

        DrawCenteredCell(backLabel.c_str(), centeredX, buttonBackY + 1, textLen, back_txt_r, back_txt_g, back_txt_b, back_bg_r, back_bg_g, back_bg_b);

        // --- INPUT ---
        char key = toupper(_getch());
        if (key == 27) { ResetColor(); return false; }

        if (key == 'W') { selected = (selected - 1 + totalOptions) % totalOptions; }
        else if (key == 'S') { selected = (selected + 1) % totalOptions; }
        else if (key == 13) {
            if (selected == totalSaves) { ResetColor(); return false; }
            else { ApplyLoadedData(saves[selected]); ResetColor(); return true; }
        }

        if (selected < totalSaves) {
            if (selected < viewOffset) { viewOffset = selected; }
            if (selected >= viewOffset + MAX_VISIBLE_SAVES) { viewOffset = selected - MAX_VISIBLE_SAVES + 1; }
        }
    }
}
void ShowOverwriteScreen() {
    std::vector<GameStateData> saves;
    // --- LOAD FILE LIST ---
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

    ClearScreenWithColor(97, 100, 151);
    const int totalSaves = saves.size();

    if (saves.empty()) {
        const char* msg = "No save files to overwrite. Press ESC to return...";
        GotoXY((CONSOLE_WIDTH - strlen(msg)) / 2, CONSOLE_HEIGHT / 2);
        SetColorRGB(255, 255, 255); std::cout << msg;
        while (_getch() != 27);
        ResetColor(); return;
    }

    // --- CẤU HÌNH UI ---
    const int SCREEN_W = 165;
    const int SCREEN_H = 110;
    const int MAX_VISIBLE_SAVES = 3;
    const int totalOptions = totalSaves + 1;
    const int ROW_SPACING = 4;

    int viewOffset = 0, selected = 0;

    const int tableTotalWidth = COL_WIDTH_NAME + COL_WIDTH_DATE + COL_WIDTH_TYPE + 4;
    const int contentHeight = 3 + (MAX_VISIBLE_SAVES * ROW_SPACING);
    const int MASTER_UI_START_X = ((SCREEN_W - tableTotalWidth) / 2) + 1;
    const int MASTER_UI_START_Y = ((SCREEN_H - contentHeight) / 2) - 30;

    // --- TỌA ĐỘ ---
    const int tableStartX = MASTER_UI_START_X;
    const int headerY = MASTER_UI_START_Y;
    const int listStartY = MASTER_UI_START_Y + 3;

    const int nameX = tableStartX + 1;
    const int dateX = nameX + COL_WIDTH_NAME + 1;
    const int typeX = dateX + COL_WIDTH_DATE + 1;

    const int buttonBackY = listStartY + (MAX_VISIBLE_SAVES * ROW_SPACING) + 2;

    // [QUAN TRỌNG]: VẼ NỀN TĨNH 1 LẦN DUY NHẤT Ở ĐÂY (BÊN NGOÀI WHILE)
    SetColorRGB(97, 100, 151);
    DrawLargebackground(0, 0);
    DrawSavegame(25, 2);
    DrawBackloadframe1(100, 33);
    DrawLoadframe1(((SCREEN_W - tableTotalWidth) / 2) - 3, ((SCREEN_H - contentHeight) / 2) - 33);

    // Vẽ Header tĩnh
    DrawCenteredCell("NAME", nameX, headerY, COL_WIDTH_NAME, 0, 255, 255, 218, 66, 76);
    DrawCenteredCell("DATE", dateX, headerY, COL_WIDTH_DATE, 0, 255, 255, 218, 66, 76);
    DrawCenteredCell("TYPE", typeX, headerY, COL_WIDTH_TYPE, 0, 255, 255, 218, 66, 76);

    while (true) {
        // [LƯU Ý]: Trong này KHÔNG gọi lại hàm vẽ Background nữa

        // 1. VẼ DANH SÁCH FILE SAVE
        for (int i = 0; i < MAX_VISIBLE_SAVES; ++i) {
            int actualIndex = viewOffset + i;
            int rowY = listStartY + (i * ROW_SPACING);

            if (actualIndex < totalSaves) {
                bool isRowSelected = (actualIndex == selected);
                int bg_r = 218, bg_g = 66, bg_b = 76;
                int text_r, text_g, text_b;
                std::string displayName;

                if (isRowSelected) {
                    text_r = 255; text_g = 255; text_b = 255;
                    displayName = std::string(1, char(15)) + " " + saves[actualIndex].name + " " + std::string(1, char(15));
                }
                else {
                    text_r = 255; text_g = 255; text_b = 0;
                    displayName = saves[actualIndex].name;
                }

                DrawCenteredCell(displayName.c_str(), nameX, rowY, COL_WIDTH_NAME, text_r, text_g, text_b, bg_r, bg_g, bg_b);
                DrawCenteredCell(saves[actualIndex].date, dateX, rowY, COL_WIDTH_DATE, text_r, text_g, text_b, bg_r, bg_g, bg_b);
                DrawCenteredCell(saves[actualIndex].type, typeX, rowY, COL_WIDTH_TYPE, text_r, text_g, text_b, bg_r, bg_g, bg_b);
            }
            else {
                // Xóa dòng thừa
                DrawCenteredCell("", nameX, rowY, COL_WIDTH_NAME, 0, 0, 0, 97, 100, 151);
                DrawCenteredCell("", dateX, rowY, COL_WIDTH_DATE, 0, 0, 0, 97, 100, 151);
                DrawCenteredCell("", typeX, rowY, COL_WIDTH_TYPE, 0, 0, 0, 97, 100, 151);
            }
        }

        // 2. VẼ NÚT BACK
        bool isBackSelected = (selected == totalSaves);
        int back_bg_r = 218, back_bg_g = 66, back_bg_b = 76;
        int back_txt_r, back_txt_g, back_txt_b;
        std::string backLabel;

        // Padding gốc (lề ngoài cùng)
        std::string pad5 = "     ";

        if (isBackSelected) {
            back_txt_r = 255; back_txt_g = 255; back_txt_b = 255;
            // Cấu trúc: [Pad5] + [Icon] + [Space] + BACK + [Space] + [Icon] + [Pad5]
            // Tổng cộng phần "thịt" thêm vào là: Icon + Space (2 ký tự mỗi bên)
            backLabel = "   " + std::string(1, char(15)) + " BACK " + std::string(1, char(15)) + "  ";
        }
        else {
            back_txt_r = 255; back_txt_g = 255; back_txt_b = 0;
            // Cấu trúc: [Pad5] + [Space] + [Space] + BACK + [Space] + [Space] + [Pad5]
            // Thêm 2 dấu cách mỗi bên để bù cho độ rộng của (Icon + Space) bên trên
            // Giúp khung hình chữ nhật KHÔNG bị co giãn khi chọn/bỏ chọn
            backLabel = pad5 + "BACK" + pad5;
        }

        int textLen = backLabel.length();
        int centeredX = (typeX + (30 - textLen) / 2) - 5;

        DrawCenteredCell(backLabel.c_str(), centeredX, buttonBackY + 1, textLen, back_txt_r, back_txt_g, back_txt_b, back_bg_r, back_bg_g, back_bg_b);

        // --- INPUT ---
        char key = toupper(_getch());
        if (key == 27) { ResetColor(); return; }

        if (key == 'W') {
            selected = (selected - 1 + totalOptions) % totalOptions;
        }
        else if (key == 'S') {
            selected = (selected + 1) % totalOptions;
        }
        else if (key == 13) { // Enter
            if (selected == totalSaves) {
                ResetColor();
                return;
            }
            else {
                std::string filenameToOverwrite = std::string(saves[selected].name) + ".txt";
                PerformSave(filenameToOverwrite);
                ResetColor();
                return;
            }
        }

        if (selected < totalSaves) {
            if (selected < viewOffset) { viewOffset = selected; }
            if (selected >= viewOffset + MAX_VISIBLE_SAVES) { viewOffset = selected - MAX_VISIBLE_SAVES + 1; }
        }
    }
}


// =================================================================================
// 4. CÁC HÀM GIAO DIỆN PHỤ
// =================================================================================
void DrawSaveMenuItem(int index, bool is_selected) {
    // --- 1. CẤU HÌNH TỌA ĐỘ ---
    // Bạn chỉnh toạ độ X, Y cho 3 mục Save tại đây
    // X = 75 cho thẳng hàng với menu cũ, Y cách nhau 3 dòng
    int POS_X[] = { 71, 71, 71 };
    int POS_Y[] = { 24, 27, 30 };

    // Kiểm tra index an toàn
    if (index < 0 || index >= 3) return;

    int x = POS_X[index];
    int y = POS_Y[index];

    std::string text = SAVE_OPTIONS[index];
    std::string displayStr;

    // --- 2. STYLE ICON & CHUỖI (Giống Menu NewGame) ---
    char icon = char(15); // Dấu ☼

    if (is_selected) {
        // Có icon 2 bên khi được chọn
        displayStr = std::string(" ") + icon + " " + text + " " + icon + " ";
    }
    else {
        // Thêm khoảng trắng để xóa icon cũ khi không chọn
        displayStr = "   " + text + "   ";
    }

    // --- 3. VẼ ---
    GotoXY(x, y);
    SetBgRGB(218, 66, 76); // Nền Đỏ (Giống style bạn đưa)

    if (is_selected) {
        SetColorRGB(255, 255, 255); // Chọn: Trắng
        std::cout << displayStr;
    }
    else {
        SetColorRGB(255, 255, 0);   // Thường: Vàng
        std::cout << displayStr;
    }

    // --- 4. RESET MÀU ---
    SetBgRGB(0, 0, 0); // Trả về nền đen (hoặc màu nền chung của game)
    SetColorRGB(255, 255, 255);
}
void ShowSaveGameScreen() {
    int selectedOption = 0;
    const int totalOptions = 3;

    // Xóa màn hình + Nền trắng
    ClearScreenWithColor(255, 255, 255);
    DrawLargebackground(0, 0);
    DrawSavegame(23, 2);
    DrawSaveframe(64, 20);
    while (true) {
        // --- 1. VẼ MENU ---
        for (int i = 0; i < totalOptions; ++i) {
            // Gọi hàm vẽ từng dòng (đã viết ở trên)
            DrawSaveMenuItem(i, (i == selectedOption));
        }

        // --- 2. XỬ LÝ PHÍM (W, S, Enter, Esc) ---
        char key = _getch();

        // Nếu là phím ESC (Mã ASCII 27) -> Thoát ngay
        if (key == 27) {
            ResetColor();
            return;
        }

        // Chuyển chữ thường thành hoa (w -> W, s -> S) để dễ so sánh
        key = toupper(key);

        if (key == 'W') {
            // Lên: trừ index, cộng totalOptions để tránh số âm
            selectedOption = (selectedOption - 1 + totalOptions) % totalOptions;
        }
        else if (key == 'S') {
            // Xuống: cộng index
            selectedOption = (selectedOption + 1) % totalOptions;
        }
        else if (key == 13) { // Phím Enter (Mã ASCII 13)
            switch (selectedOption) {
            case 0:
                ShowNewSaveScreen();
                ClearScreenWithColor(255, 255, 255);
                DrawLargebackground(0, 0);
                DrawSavegame(23, 2);
                DrawSaveframe(64, 20);
                break;
            case 1:
                ShowOverwriteScreen();
                ClearScreenWithColor(255, 255, 255);
                DrawLargebackground(0, 0);
                DrawSavegame(23, 2);
                DrawSaveframe(64, 20);
                break;
            case 2: // Back (Option cuối cùng trong menu)
                ResetColor();
                return;
            }
        }
    }
}

/**
 * @brief Hiển thị màn hình cho phép người dùng nhập tên file save mới.
 */
void ShowNewSaveScreen() {
    // 1. Setup ban đầu
    SetCursorVisible(true); // Bật con trỏ

    std::string saveName = "";
    std::string message = "*Less than 20 characters    ";
    bool isError = false;
    bool isSuccess = false;

    // Tọa độ vẽ
    const int contentWidth = 40;
    const int centerX = (CONSOLE_WIDTH - contentWidth) / 2;
    const int startY = (CONSOLE_HEIGHT / 2) - 4;

    // --- VẼ TĨNH 1 LẦN (NGOÀI VÒNG LẶP) ---
    ClearScreenWithColor(255, 255, 255);
    DrawLargebackground(0, 0);
    DrawSavegame(23, 2);
    DrawSaveframe1(64, 20);
    SetColorRGB(255, 255, 255);
    SetBgRGB(218, 66, 76);
    std::string title = "FILE'S NAME";
    GotoXY(((CONSOLE_WIDTH - title.length()) / 2) + 3, startY);
    std::cout << title;

    GotoXY(centerX + 8, startY + 3);
    std::cout << "Enter: ";

    GotoXY(centerX + 13 + 3, startY + 5);
    std::cout << "OK   |   BACK";
    // -------------------------------------

    while (true) {
        // --- CẬP NHẬT TRẠNG THÁI ---

        // 1. Vẽ dòng thông báo (Lỗi/Thành công)
        if (isError) SetColorRGB(255, 255, 255);      // Đỏ
        else if (isSuccess) SetColorRGB(255, 255, 255); // Xanh lá
        else SetColorRGB(255, 255, 255);                // Đen

        GotoXY(centerX + 10, startY + 2);
        std::cout << message << " "; // In đè khoảng trắng để xóa chữ cũ

        SetColorRGB(255, 255, 255); // Reset màu

        // 2. Nếu đã lưu thành công -> Hiện dòng "Press any key"
        if (isSuccess) {
            std::string msgContinue = "Press any key to continue!";
            GotoXY((CONSOLE_WIDTH - msgContinue.length()) / 2, startY + 7);
            std::cout << msgContinue;

            SetCursorVisible(false); // Tắt con trỏ cho đẹp
            _getch(); // Chờ người dùng bấm phím để đọc thông báo
            return;   // Thoát ra menu
        }

        // 3. Đặt con trỏ về đúng chỗ nhập
        GotoXY(centerX + 7 + saveName.length() + 8, startY + 3);

        // 4. Nhập liệu
        char ch = _getch();

        if (ch == 13) { // Phím ENTER
            if (saveName.empty()) continue;

            std::string fileNameFull = saveName + ".txt";
            std::string pathToCheck = SAVE_DIR + fileNameFull;
            if (FileExists(pathToCheck)) { 
                isError = true;
                isSuccess = false;
                message = "*File's name has existed!";
            }
            else {
                // Gọi hàm lưu (Hàm này đã sửa ở Bước 1 nên không hiện màn hình trắng nữa)
                PerformSave(fileNameFull);

                isError = false;
                isSuccess = true;
                message = "*Save file successfully";
                // Vòng lặp quay lại -> Sẽ hiện chữ xanh và chờ bấm phím thoát
            }
        }
        else if (ch == 27) { // Phím ESC (Back)
            SetCursorVisible(false);
            return;
        }
        else if (ch == 8) { // Phím Backspace
            if (!saveName.empty()) {
                std::cout << "\b \b";
                saveName.pop_back();
                isError = false;
                message = "*Less than 20 characters";
            }
        }
        // Cho phép nhập chữ thường, số, khoảng trắng
        else if (saveName.length() < 20 && (isalnum(ch) || ch == ' ' || ch == '_')) {
            std::cout << ch;
            saveName += ch;
            isError = false;
            message = "*Less than 20 characters";
        }
    }
}
// =================================================================================
// 5. CÁC HÀM XỬ LÝ LOGIC NỀN
// =================================================================================

/**
 * @brief Thực hiện hành động lưu game.
 */
void PerformSave(const std::string& filename) {
    GameStateData dataToSave = GetCurrentGameStateData();
    // Cắt đuôi .txt để lấy tên hiển thị (nếu cần logic cũ)
    std::string baseName = filename.substr(0, filename.find(".txt"));
    strcpy_s(dataToSave.name, sizeof(dataToSave.name), baseName.c_str());

    // CHỈ GỌI HÀM LƯU, KHÔNG GỌI MÀN HÌNH THÔNG BÁO NỮA
    SaveGameToFile(dataToSave, filename);
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
    // thời gian
    currentData.timeLeft = _turnTimer;
    currentData.round = _round;
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

    // 4. Metadata 
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

    // --- 3. CHỖ NỐI DÂY  ---
    // Lấy tên từ file đè vào biến game
    strcpy_s(_player1_name, MAX_NAME_LEN, data.p1Name);
    strcpy_s(_player2_name, MAX_NAME_LEN, data.p2Name);

    // Lấy điểm số
    _player1_score = data.p1Score;
    _player2_score = data.p2Score;
    _moveCount = data.moveCount;
    // thời gian còn lại trước khi save
    _turnTimer = data.timeLeft;
    _round = data.round;
    _lastTimeCheck = time(0);
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
    SetColorRGB(0, 0, 0);    
    SetBgRGB(97, 100, 151); // Màu nền

    // Tọa độ các đường kẻ dọc
    int x1 = startX;
    int x2 = x1 + 1 + nameWidth;
    int x3 = x2 + 1 + dateWidth;
    int x4 = x3 + 1 + typeWidth;

    // --- VẼ KHUNG NÉT ĐÔI ---
    // Ký tự sử dụng: ═ ║ ╔ ╗ ╚ ╝ ╠ ╣ ╦ ╩ ╬

    // 1. Dòng trên cùng: ╔═══╦═══╦═══╗
    GotoXY(x1, startY); std::cout << "╔"; for (int i = 0; i < nameWidth; ++i) std::cout << "═";
    std::cout << "╦"; for (int i = 0; i < dateWidth; ++i) std::cout << "═";
    std::cout << "╦"; for (int i = 0; i < typeWidth; ++i) std::cout << "═";
    std::cout << "╗";

    // 2. Hàng Header (đường kẻ dọc nét đôi)
    int headerContentY = startY + 1;
    GotoXY(x1, headerContentY); std::cout << "║";
    GotoXY(x2, headerContentY); std::cout << "║";
    GotoXY(x3, headerContentY); std::cout << "║";
    GotoXY(x4, headerContentY); std::cout << "║";

    // 3. Dòng phân cách header: ╠═══╬═══╬═══╣
    int headerSeparatorY = startY + 2;
    GotoXY(x1, headerSeparatorY); std::cout << "╠"; for (int i = 0; i < nameWidth; ++i) std::cout << "═";
    std::cout << "╬"; for (int i = 0; i < dateWidth; ++i) std::cout << "═";
    std::cout << "╬"; for (int i = 0; i < typeWidth; ++i) std::cout << "═";
    std::cout << "╣";

    // 4. Vẽ các hàng nội dung
    for (int i = 0; i < numContentRows; ++i) {
        int contentY = headerSeparatorY + 1 + (i * 2);
        int separatorY = contentY + 1;

        // Vẽ các đường kẻ dọc
        GotoXY(x1, contentY); std::cout << "║";
        GotoXY(x2, contentY); std::cout << "║";
        GotoXY(x3, contentY); std::cout << "║";
        GotoXY(x4, contentY); std::cout << "║";

        // Đường phân cách giữa các hàng (vẫn dùng nét đôi cho đồng bộ)
        if (i < numContentRows - 1) {
            GotoXY(x1, separatorY); std::cout << "╠"; for (int k = 0; k < nameWidth; ++k) std::cout << "═";
            std::cout << "╬"; for (int k = 0; k < dateWidth; ++k) std::cout << "═";
            std::cout << "╬"; for (int k = 0; k < typeWidth; ++k) std::cout << "═";
            std::cout << "╣";
        }
    }

    // 5. Dòng dưới cùng: ╚═══╩═══╩═══╝
    int bottomY = headerSeparatorY + (numContentRows * 2);
    GotoXY(x1, bottomY); std::cout << "╚"; for (int i = 0; i < nameWidth; ++i) std::cout << "═";
    std::cout << "╩"; for (int i = 0; i < dateWidth; ++i) std::cout << "═";
    std::cout << "╩"; for (int i = 0; i < typeWidth; ++i) std::cout << "═";
    std::cout << "╝";
}