#include "Save&load.h"
#include <vector>
#include <conio.h> 
#include <limits>  

string listAndGetFileName() {
    system("cls");
    cout << "\n--- DANH SÁCH MÀN CHƠI ĐÃ LƯU ---" << endl;
    vector<string> savedFiles;
    int index = 1;

    // Đường dẫn tìm kiếm (Sử dụng wildcard * để tìm tất cả file .txt)
    string searchPath = SAVE_DIR + "*.txt";

    WIN32_FIND_DATAA findFileData;
    HANDLE hFind = INVALID_HANDLE_VALUE;

    // Bắt đầu tìm kiếm file .txt trong thư mục
    hFind = FindFirstFileA(searchPath.c_str(), &findFileData);

    if (hFind == INVALID_HANDLE_VALUE) {
        cout << " Chưa có màn chơi nào được lưu hoặc lỗi truy cập vào thư mục." << endl;
        return;
    }

    // Lặp qua các file được tìm thấy
    do {
        string filename(findFileData.cFileName);
      
        // Chỉ xử lý các file thông thường (Không phải thư mục)
        if (!(findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
            // Lấy tên gốc của file (bỏ đi 4 ký tự cuối là ".txt")
            if (filename.length() > 4) {
                string baseName = filename.substr(0, filename.length() - 4);
                savedFiles.push_back(baseName);
                cout << index++ << ". " << baseName << endl;
            }
            
        string filename(findFileData.cFileName);
        if (!(findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
            if (filename.length() > 4) {
                string baseName = filename.substr(0, filename.length() - 4);
                savedFiles.push_back(baseName);
                cout << index++ << ". " << baseName << endl;
            }
        }
    } while (FindNextFileA(hFind, &findFileData) != 0);
    FindClose(hFind);

    if (savedFiles.empty()) {
        cout << "Chưa có màn chơi nào được lưu." << endl;
        cout << "\nNhấn phím bất kỳ để quay lại menu chính";
        _getch();
        return "";
    }

    // Cho người dùng chọn file để tải
    int choice=-1;
    do {
        cout << "\nChọn số thứ tự màn cần tải (Nhập 0 để hủy): ";
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        
        if (!(cin >> choice)) {
            cin.clear(); // Xóa cờ lỗi
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Xóa bộ đệm
            choice = -1; 
            cout << "Lỗi nhập, vui lòng nhập lại số thứ tự." << endl;
        } else if (choice == 0) {
            return ""; // Hủy
        } else if (choice < 1 || choice > savedFiles.size()) {
            cout << "Số thứ tự không hợp lệ, vui lòng nhập lại." << endl;
        }
    } while (choice < 1 || choice > savedFiles.size());

    return savedFiles[choice - 1]; // Trả về tên file
}

    // Tải game với tên file được chọn
    loadGame(game, savedFiles[choice - 1]);

}

