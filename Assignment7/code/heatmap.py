import os
import struct
import glob
import numpy as np
import matplotlib.pyplot as plt
import imageio
import re

def read_matrix(filename):
    with open(filename, "rb") as f:
        rows = struct.unpack("i", f.read(4))[0]
        cols = struct.unpack("i", f.read(4))[0]
        data = struct.unpack("d" * (rows * cols), f.read(8 * rows * cols))
        matrix = np.array(data).reshape((rows, cols))
    return matrix

def save_heatmap(matrix, frame_idx, out_dir):
    plt.imshow(matrix, cmap="hot", interpolation="nearest")
    plt.axis("off")
    fname = f"{out_dir}/heatmap_{frame_idx:03d}.png"
    plt.savefig(fname, bbox_inches="tight", pad_inches=0)
    plt.close()
    return fname

def extract_frame_number(filename):
    match = re.search(r'frame(\d+)', filename)
    return int(match.group(1)) if match else -1

def make_gif(image_files, out_path):
    frames = [imageio.v2.imread(img) for img in image_files]
    imageio.mimsave(out_path, frames, duration=0.3)

def main():
    input_folder = "frames"
    output_folder = "heatmaps"
    os.makedirs(output_folder, exist_ok=True)

    binary_files = sorted(
        glob.glob(os.path.join(input_folder, "frame*")),
        key=extract_frame_number
    )

    image_files = []

    for idx, file in enumerate(binary_files):
        matrix = read_matrix(file)
        img_file = save_heatmap(matrix, idx, output_folder)
        image_files.append(img_file)

    make_gif(image_files, "matrix_animation.gif")
    print("GIF saved as matrix_animation.gif 🎉")

if __name__ == "__main__":
    main()