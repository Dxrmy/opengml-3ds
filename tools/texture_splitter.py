#!/usr/bin/env python3
import os
import sys
import json
import argparse
from PIL import Image

MAX_SIZE = 1024

def split_texture(filepath, output_dir):
    try:
        img = Image.open(filepath)
    except Exception as e:
        print(f"Error opening {filepath}: {e}")
        return None

    width, height = img.size
    if width <= MAX_SIZE and height <= MAX_SIZE:
        return None # No need to split

    print(f"Splitting {filepath} ({width}x{height})")
    
    filename = os.path.basename(filepath)
    name, ext = os.path.splitext(filename)
    
    chunks = []
    
    for y in range(0, height, MAX_SIZE):
        for x in range(0, width, MAX_SIZE):
            box_right = min(x + MAX_SIZE, width)
            box_bottom = min(y + MAX_SIZE, height)
            
            crop_w = box_right - x
            crop_h = box_bottom - y
            
            box = (x, y, box_right, box_bottom)
            chunk_img = img.crop(box)
            
            chunk_filename = f"{name}_chunk_{x}_{y}{ext}"
            chunk_filepath = os.path.join(output_dir, chunk_filename)
            
            chunk_img.save(chunk_filepath)
            
            chunks.append({
                "file": chunk_filename,
                "offset_x": x,
                "offset_y": y,
                "width": crop_w,
                "height": crop_h
            })
            
    return chunks

def main():
    parser = argparse.ArgumentParser(description="Split large textures for 3DS compatibility (max 1024x1024).")
    parser.add_argument("input_dir", help="Directory to scan for PNG files.")
    parser.add_argument("-o", "--output_dir", help="Directory to save chunks (defaults to input_dir).", default=None)
    parser.add_argument("-m", "--map_file", help="Path to save the JSON coordinate map.", default="texture_map.json")
    
    args = parser.parse_args()
    
    input_dir = args.input_dir
    output_dir = args.output_dir if args.output_dir else input_dir
    map_file = args.map_file
    
    if not os.path.isdir(input_dir):
        print(f"Error: Input directory {input_dir} does not exist.")
        sys.exit(1)
        
    if not os.path.exists(output_dir):
        os.makedirs(output_dir)

    texture_map = {}
    split_count = 0

    for root, _, files in os.walk(input_dir):
        for file in files:
            if file.lower().endswith(".png"):
                filepath = os.path.join(root, file)
                
                # Try to avoid processing already chunked files if outputting to the same dir
                if "_chunk_" in file:
                    continue

                chunks = split_texture(filepath, output_dir)
                if chunks:
                    # Store relative path in the map for easier resolution later
                    rel_path = os.path.relpath(filepath, input_dir)
                    texture_map[rel_path] = chunks
                    split_count += 1

    if texture_map:
        with open(map_file, 'w') as f:
            json.dump(texture_map, f, indent=4)
        print(f"Successfully split {split_count} textures.")
        print(f"Texture map saved to {map_file}")
    else:
        print("No textures exceeded the 1024x1024 limit. No splits required.")

if __name__ == "__main__":
    main()
