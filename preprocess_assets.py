from PIL import Image
import os

def resize_and_crop(image_path, output_path, size):
    with Image.open(image_path) as img:
        # Calculate aspect ratio
        img_ratio = img.width / img.height
        target_ratio = size[0] / size[1]
        
        if img_ratio > target_ratio:
            # Image is wider than target
            new_width = int(target_ratio * img.height)
            offset = (img.width - new_width) // 2
            img = img.crop((offset, 0, offset + new_width, img.height))
        else:
            # Image is taller than target
            new_height = int(img.width / target_ratio)
            offset = (img.height - new_height) // 2
            img = img.crop((0, offset, img.width, offset + new_height))
            
        img = img.resize(size, Image.LANCZOS)
        img.save(output_path)

# Assets directory
assets_dir = "/home/ubuntu/win7_3ds/assets"

# 1. Wallpaper for Top Screen (400x240)
resize_and_crop(os.path.join(assets_dir, "wallpaper.jpg"), 
                os.path.join(assets_dir, "top_wallpaper.png"), (400, 240))

# 2. Wallpaper for Bottom Screen (320x240)
resize_and_crop(os.path.join(assets_dir, "wallpaper.jpg"), 
                os.path.join(assets_dir, "bottom_wallpaper.png"), (320, 240))

# 3. Start Button (e.g., 32x32 for taskbar)
# Note: The original start button might need specific cropping if it's a sprite sheet
with Image.open(os.path.join(assets_dir, "start_button.png")) as img:
    # Assuming the first one in the sheet or just resizing the whole thing if it's single
    img = img.resize((32, 32), Image.LANCZOS)
    img.save(os.path.join(assets_dir, "start_icon.png"))

print("Assets preprocessed successfully.")
