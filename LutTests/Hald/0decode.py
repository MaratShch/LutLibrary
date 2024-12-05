from PIL import Image

def save_png_as_txt(input_png_file, output_txt_file):
    """
    Reads a PNG file, extracts RGB pixel data, and saves it to a TXT file.
    
    Args:
        input_png_file (str): Path to the input PNG file.
        output_txt_file (str): Path to the output TXT file.
    """
    try:
        # Open the PNG file
        with Image.open(input_png_file) as img:
            # Check interlace status
            interlaced = img.info.get("interlace", 0)  # Default to 0 (non-interlaced)
            print(f"Interlaced: {'Yes' if interlaced else 'No'}")

            # Ensure the image is in RGB mode
            img = img.convert("RGB")
            width, height = img.size
            pixels = img.load()

        # Open the output TXT file
        with open(output_txt_file, 'w') as txt_file:
            for y in range(height):
                for x in range(width):
                    r, g, b = pixels[x, y]
                    # Write each pixel's RGB values as 0x-prefixed hexadecimal
                    txt_file.write(f"0x{r:02X} 0x{g:02X} 0x{b:02X}\n")

        print(f"RGB data saved successfully to {output_txt_file}")

    except Exception as e:
        print(f"Error processing the file: {e}")

# Example usage
input_png = "contrast.HCLUT.png"
output_txt = "decoded_contrast.HCLUT.txt"
save_png_as_txt(input_png, output_txt)
