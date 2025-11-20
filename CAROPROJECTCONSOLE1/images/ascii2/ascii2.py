import os
import glob
from PIL import Image

# --- CẤU HÌNH ---
TARGET_WIDTH = 165 
TARGET_HEIGHT = 16  
# ----------------

SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
OUTPUT_DIR = os.path.dirname(SCRIPT_DIR)

OUTPUT_H = os.path.join(OUTPUT_DIR, "GameAssets.h")
OUTPUT_CPP = os.path.join(OUTPUT_DIR, "GameAssets.cpp")

def get_pixel_data(img):
    img = img.convert("RGBA")
    img = img.resize((TARGET_WIDTH, TARGET_HEIGHT), Image.Resampling.NEAREST)
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

def main():
    print(f"📂 Đang quét ảnh trong: {SCRIPT_DIR}")
    png_files = glob.glob(os.path.join(SCRIPT_DIR, "*.png"))
    
    if not png_files: return

    h_content = []
    cpp_content = []

    for filepath in png_files:
        filename = os.path.basename(filepath)
        name_no_ext = os.path.splitext(filename)[0]
        clean_name = name_no_ext.replace(" ", "_").replace("-", "_")
        camel_name = "".join(x.title() for x in clean_name.split("_"))
        func_name = f"Draw{camel_name}"
        
        try:
            img = Image.open(filepath)
            w, h, data_str = get_pixel_data(img)
            
            h_content.append(f"void {func_name}(int x, int y); // {filename}")
            
            cpp_chunk = f"""
// --- Asset: {func_name} ({w}x{h}) ---
static const std::vector<DrawInstructionTrueColor> {func_name}_Data = {{
{data_str}
}};
void {func_name}(int x, int y) {{
    DrawImageHalfBlock(x, y, {func_name}_Data);
}}
"""
            cpp_content.append(cpp_chunk)
            print(f"  ✅ {filename} -> {func_name}")
        except Exception as e: print(f"  ❌ Lỗi {filename}: {e}")

    # --- GHI FILE ---
    with open(OUTPUT_H, "w", encoding="utf-8") as f:
        f.write("#pragma once\n// Auto-generated\n\n")
        f.write("\n".join(h_content))
        
    with open(OUTPUT_CPP, "w", encoding="utf-8") as f:
        f.write('#include "GameAssets.h"\n')
        # --- SỬA Ở ĐÂY: Include View.h thay vì Graphics.h ---
        f.write('#include "View.h"\n') 
        # ---------------------------------------------------
        f.write('#include <vector>\n\n')
        f.write("\n".join(cpp_content))

    print("🎉 XONG!")

if __name__ == "__main__":
    main()