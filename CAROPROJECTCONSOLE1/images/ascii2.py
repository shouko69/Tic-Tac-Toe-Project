import os
import glob
import re # Thêm thư viện xử lý chuỗi mạnh mẽ
from PIL import Image

SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
OUTPUT_DIR = os.path.dirname(SCRIPT_DIR)

OUTPUT_H = os.path.join(OUTPUT_DIR, "GameAssets.h")
OUTPUT_CPP = os.path.join(OUTPUT_DIR, "GameAssets.cpp")

def get_pixel_data(img, target_w=None, target_h=None):
    img = img.convert("RGBA")
    if target_w and target_h:
        img = img.resize((target_w, target_h), Image.Resampling.NEAREST)
    
    width, height = img.size
    data_block = []
    
    for y in range(height):
        x = 0
        while x < width:
            try:
                r, g, b, a = img.getpixel((x, y))
                val = (r, g, b) if a >= 100 else -1
            except: val = -1

            if val == -1:
                x += 1; continue
            
            run_len = 1
            while (x + run_len < width):
                try:
                    nr, ng, nb, na = img.getpixel((x + run_len, y))
                    nval = (nr, ng, nb) if na >= 100 else -1
                except: nval = -1
                if nval == val: run_len += 1
                else: break
            
            r, g, b = val
            data_block.append(f"    {{{y}, {x}, {r}, {g}, {b}, {run_len}}},")
            x += run_len
            
    if data_block: data_block[-1] = data_block[-1].rstrip(',')
    return width, height, "\n".join(data_block)

def ensure_files_exist():
    if not os.path.exists(OUTPUT_H):
        with open(OUTPUT_H, "w", encoding="utf-8") as f:
            f.write("#pragma once\n// Auto-generated Assets\n\n")
    if not os.path.exists(OUTPUT_CPP):
        with open(OUTPUT_CPP, "w", encoding="utf-8") as f:
            f.write('#include "GameAssets.h"\n#include "View.h"\n#include <vector>\n\n')

def update_cpp_file(func_name, new_code_chunk):
    with open(OUTPUT_CPP, "r", encoding="utf-8") as f:
        content = f.read()

    # Regex để tìm block code cũ của hàm này
    # Tìm từ "// --- Asset: TênHàm" cho đến dấu "}" đóng hàm
    pattern = re.compile(rf"// --- Asset: {func_name}.*?void {func_name}.*?^}}", re.DOTALL | re.MULTILINE)

    if pattern.search(content):
        print(f"   ⚠️  Phát hiện hàm {func_name} đã tồn tại!")
        user_choice = input("      👉 Bạn có muốn GHI ĐÈ (Xóa cũ thay mới) không? (y/n): ").strip().lower()
        
        if user_choice == 'y':
            # Thay thế đoạn code cũ bằng code mới
            new_content = pattern.sub(new_code_chunk.strip(), content)
            with open(OUTPUT_CPP, "w", encoding="utf-8") as f:
                f.write(new_content)
            print(f"      ✅ Đã GHI ĐÈ thành công {func_name}!")
        else:
            print("      ⛔ Đã hủy thao tác.")
    else:
        # Nếu chưa có thì thêm vào cuối file
        with open(OUTPUT_CPP, "a", encoding="utf-8") as f:
            f.write("\n" + new_code_chunk)
        print(f"   ✅ Đã thêm mới {func_name} vào cuối file.")

def process_asset(filepath):
    filename = os.path.basename(filepath)
    print(f"\n------------------------------------------------")
    print(f"🖼️  Đang xử lý: {filename}")
    
    try:
        img = Image.open(filepath)
        w_orig, h_orig = img.size
        print(f"   📏 Kích thước gốc: {w_orig}x{h_orig}")
        
        print("   👉 Resize không?")
        print("      - ENTER: Giữ nguyên (Dùng cho Brick).")
        print("      - Nhập '3 4': Ép về 3x4 (Dùng cho X, O).")
        size_input = input("   Lựa chọn: ").strip()
        
        target_w, target_h = None, None
        if size_input:
            try:
                parts = size_input.split()
                target_w, target_h = int(parts[0]), int(parts[1])
            except: pass

        name_no_ext = os.path.splitext(filename)[0]
        clean_name = name_no_ext.replace(" ", "_").replace("-", "_")
        camel_name = "".join(x.title() for x in clean_name.split("_"))
        func_name = f"Draw{camel_name}"

        # Lấy dữ liệu pixel
        final_w, final_h, data_str = get_pixel_data(img, target_w, target_h)
        
        # Code mới chuẩn bị ghi
        cpp_chunk = f"""
// --- Asset: {func_name} ({final_w}x{final_h}) ---
static const std::vector<DrawInstructionTrueColor> {func_name}_Data = {{
{data_str}
}};
void {func_name}(int x, int y) {{
    DrawImageHalfBlock(x, y, {func_name}_Data);
}}
"""
        # 1. Cập nhật Header (Nếu chưa có thì thêm)
        with open(OUTPUT_H, "r", encoding="utf-8") as f: h_content = f.read()
        if func_name not in h_content:
            with open(OUTPUT_H, "a", encoding="utf-8") as f:
                f.write(f"void {func_name}(int x, int y); // {filename}\n")
        
        # 2. Cập nhật CPP (Thông minh: Hỏi ghi đè)
        update_cpp_file(func_name, cpp_chunk)

    except Exception as e: print(f"   ❌ Lỗi: {e}")

def main():
    ensure_files_exist()
    all_pngs = glob.glob(os.path.join(SCRIPT_DIR, "*.png"))
    if not all_pngs: return

    print("Danh sách file:")
    for i, f in enumerate(all_pngs):
        print(f"  [{i+1}] {os.path.basename(f)}")
    
    user_input = input("\nChọn file (Số thứ tự): ").strip()
    if user_input.isdigit():
        idx = int(user_input) - 1
        if 0 <= idx < len(all_pngs):
            process_asset(all_pngs[idx])

if __name__ == "__main__":
    main()