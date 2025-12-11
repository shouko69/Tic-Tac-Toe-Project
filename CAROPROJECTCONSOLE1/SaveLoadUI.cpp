#include "SaveLoadUI.h"
#include "GameAssets.h"
#include <iostream>

const int COL_WIDTH_NAME = 25;
const int COL_WIDTH_DATE = 20;
const int COL_WIDTH_TYPE = 15;
const std::string SAVE_OPTIONS[] = { "Save to New File", "Overwrite Existing", "Back" };

void DrawCenteredCell(const char* text, int cellX, int cellY, int cellWidth, int fg_r, int fg_g, int fg_b, int bg_r, int bg_g, int bg_b) {
    GotoXY(cellX, cellY);
    SetBgRGB(bg_r, bg_g, bg_b);
    for (int i = 0; i < cellWidth; ++i) std::cout << " ";
    int textLen = strlen(text);
    int padding = cellWidth - textLen;
    if (padding < 0) padding = 0;
    int textX = cellX + (padding / 2);
    GotoXY(textX, cellY);
    SetColorRGB(fg_r, fg_g, fg_b);
    std::cout << text;
}

void DrawSaveMenu(int selectedIndex, bool init) {
    if (init) {
        ClearScreenWithColor(255, 255, 255);
        DrawLargebackground(0, 0);
        DrawSavegame(23, 2);
        DrawSaveframe(63, 18);
    }

    int POS_X[] = { 71, 71, 71 };
    int POS_Y[] = { 24, 27, 30 };
    for (int i = 0; i < 3; ++i) {
        int x = POS_X[i];
        int y = POS_Y[i];
        std::string text = SAVE_OPTIONS[i];
        std::string displayStr;
        char icon = char(15);
        bool is_selected = (i == selectedIndex);

        if (is_selected) displayStr = std::string(" ") + icon + " " + text + " " + icon + " ";
        else displayStr = "   " + text + "   ";

        GotoXY(x, y);
        SetBgRGB(218, 66, 76);
        if (is_selected) SetColorRGB(255, 255, 255);
        else SetColorRGB(255, 255, 0);
        std::cout << displayStr;

        SetBgRGB(0, 0, 0);
        SetColorRGB(255, 255, 255);
    }
}

void DrawNewSaveInputScreen(const std::string& currentName, const std::string& message, bool isSuccess, bool init) {
    const int contentWidth = 40;
    const int centerX = (CONSOLE_WIDTH - contentWidth) / 2;
    const int startY = (CONSOLE_HEIGHT / 2) - 4;

    if (init) {
        ClearScreenWithColor(255, 255, 255);
        DrawLargebackground(0, 0);
        DrawSavegame(23, 2);
        DrawSaveframe1(64, 18);

        SetColorRGB(255, 255, 255);
        SetBgRGB(218, 66, 76);
        std::string title = "FILE'S NAME";
        GotoXY(((CONSOLE_WIDTH - title.length()) / 2) + 2, startY);
        std::cout << title;

        GotoXY(centerX + 13 + 3, startY + 5);
        std::cout << "OK   |   BACK";
    }

    SetColorRGB(255, 255, 255);
    SetBgRGB(218, 66, 76);

    GotoXY(centerX + 8, startY + 3);
    std::cout << "Enter: " << std::string(20, ' ');
    GotoXY(centerX + 8, startY + 3);
    std::cout << "Enter: " << currentName;

    GotoXY(centerX + 8, startY + 2);
    std::cout << std::string(29, ' ');
    GotoXY(centerX + 8, startY + 2);
    std::cout << message;

    if (isSuccess) {
        std::string msgContinue = "Press any key to continue!";
        GotoXY((CONSOLE_WIDTH - msgContinue.length()) / 2 + 2, startY + 7);
        std::cout << msgContinue;
    }
    if (!isSuccess) {
        GotoXY(centerX + 8 + 7 + currentName.length(), startY + 3);
    }
}

void DrawFileSelectionScreen(const std::vector<GameStateData>& saves, int viewOffset, int selectedIndex, bool isLoadMode, bool init) {
    const int SCREEN_W = 165;
    const int SCREEN_H = 110;
    const int MAX_VISIBLE_SAVES = 3;
    const int ROW_SPACING = 4;
    const int tableTotalWidth = COL_WIDTH_NAME + COL_WIDTH_DATE + COL_WIDTH_TYPE + 4;
    const int contentHeight = 3 + (MAX_VISIBLE_SAVES * ROW_SPACING);
    const int MASTER_UI_START_X = (SCREEN_W - tableTotalWidth) / 2;
    const int MASTER_UI_START_Y = ((SCREEN_H - contentHeight) / 2) - 31;
    const int nameX = MASTER_UI_START_X + 1;
    const int dateX = nameX + COL_WIDTH_NAME + 1;
    const int typeX = dateX + COL_WIDTH_DATE + 1;
    const int buttonBackY = MASTER_UI_START_Y + 3 + (MAX_VISIBLE_SAVES * ROW_SPACING) + 2;

    if (init) {
        SetColorRGB(97, 100, 151);
        if (isLoadMode) {
            DrawLargebackground1(0, 0);
            DrawLoadgame(25, 2);
            DrawBackloadframe(100, 33);
            DrawLoadframe(MASTER_UI_START_X - 3, MASTER_UI_START_Y - 2);
        }
        else {
            DrawLargebackground(0, 0);
            DrawSavegame(25, 2);
            DrawBackloadframe1(99, 33);
            DrawLoadframe1(MASTER_UI_START_X - 4, MASTER_UI_START_Y - 3);
        }

        DrawCenteredCell("NAME", nameX, MASTER_UI_START_Y, COL_WIDTH_NAME, 0, 255, 255, 218, 66, 76);
        DrawCenteredCell("DATE", dateX, MASTER_UI_START_Y, COL_WIDTH_DATE, 0, 255, 255, 218, 66, 76);
        DrawCenteredCell("TYPE", typeX, MASTER_UI_START_Y, COL_WIDTH_TYPE, 0, 255, 255, 218, 66, 76);
    }

    if (saves.empty()) return;

    int totalSaves = saves.size();
    for (int i = 0; i < MAX_VISIBLE_SAVES; ++i) {
        int actualIndex = viewOffset + i;
        int rowY = MASTER_UI_START_Y + 3 + (i * ROW_SPACING);

        if (actualIndex < totalSaves) {
            bool isRowSelected = (actualIndex == selectedIndex);
            int bg_r = 218, bg_g = 66, bg_b = 76;
            std::string displayName = saves[actualIndex].name;
            int text_r = 255, text_g = 255, text_b = 0;

            if (isRowSelected) {
                text_b = 255;
                displayName = std::string(1, char(15)) + " " + displayName + " " + std::string(1, char(15));
            }

            DrawCenteredCell(displayName.c_str(), nameX, rowY, COL_WIDTH_NAME, text_r, text_g, text_b, bg_r, bg_g, bg_b);
            DrawCenteredCell(saves[actualIndex].date, dateX, rowY, COL_WIDTH_DATE, text_r, text_g, text_b, bg_r, bg_g, bg_b);
            DrawCenteredCell(saves[actualIndex].type, typeX, rowY, COL_WIDTH_TYPE, text_r, text_g, text_b, bg_r, bg_g, bg_b);
        }
        else {
            DrawCenteredCell("", nameX, rowY, COL_WIDTH_NAME, 0, 0, 0, 97, 100, 151);
            DrawCenteredCell("", dateX, rowY, COL_WIDTH_DATE, 0, 0, 0, 97, 100, 151);
            DrawCenteredCell("", typeX, rowY, COL_WIDTH_TYPE, 0, 0, 0, 97, 100, 151);
        }
    }

    bool isBackSelected = (selectedIndex == totalSaves);
    int back_txt_b = isBackSelected ? 255 : 0;
    std::string backLabel = isBackSelected ? ("   " + std::string(1, char(15)) + " BACK " + std::string(1, char(15)) + "   ") : "     BACK     ";
    int textLen = backLabel.length();
    int centeredX = (typeX + (30 - textLen) / 2) - 5;
    DrawCenteredCell(backLabel.c_str(), centeredX, buttonBackY + 1, textLen, 255, 255, back_txt_b, 218, 66, 76);
}