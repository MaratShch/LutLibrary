import png  # Install using 'pip install pypng'

def save_png_as_txt(input_png_file, output_txt_file):
    """
    Reads a PNG file, extracts RGB pixel data (supports 8 and 16 bits per channel),
    and saves it to a TXT file. Correctly handles true bit depth detection.
    
    Args:
        input_png_file (str): Path to the input PNG file.
        output_txt_file (str): Path to the output TXT file.
    """
    try:
        # Read PNG file metadata
        reader = png.Reader(input_png_file)
        png_info = reader.read()
        
        # Extract image properties
        width, height = png_info[0], png_info[1]
        bit_depth = png_info[3]["bitdepth"]  # Actual bit depth per channel
        color_type = png_info[3]["planes"]  # Number of color channels
        interlaced = png_info[3].get("interlace", 0)  # 0 = non-interlaced
        
        # Calculate bits per pixel
        bits_per_pixel = bit_depth * color_type
        
        # Print image properties
        print(f"Image resolution: {width}x{height}")
        print(f"Interlaced: {'Yes' if interlaced else 'No'}")
        print(f"Bits per pixel: {bits_per_pixel}")
        
        # Open output TXT file
        with open(output_txt_file, "w") as txt_file:
            # Iterate over pixel rows
            for row in png_info[2]:
                # Split pixel data into channels (depends on color type)
                for i in range(0, len(row), color_type):
                    if bit_depth > 8:
                        # Handle 16 bits per channel
                        r, g, b = row[i], row[i + 1], row[i + 2]
                        txt_file.write(f"0x{r:04X} 0x{g:04X} 0x{b:04X}\n")
                    else:
                        # Handle 8 bits per channel
                        r, g, b = row[i], row[i + 1], row[i + 2]
                        txt_file.write(f"0x{r:02X} 0x{g:02X} 0x{b:02X}\n")

        print(f"Pixel data saved successfully to {output_txt_file}")

    except Exception as e:
        print(f"Error processing the file: {e}")

# Example usage
# save_png_as_txt("input.png", "output.txt")


# Example usage
input_png = "neutral_hald_512.png"
output_txt = "decoded_neutral_hald_512.txt"
save_png_as_txt(input_png, output_txt)
