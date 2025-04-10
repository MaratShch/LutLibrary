import math

# --- Configuration ---
LUT_SIZE = 128  # Set to 96 or 128, or any other desired size
OUTPUT_FILENAME = f"negative_lut_{LUT_SIZE}.cube"
PRECISION = 6  # Number of decimal places for output values in the file

# --- Generate LUT ---
print(f"Generating a {LUT_SIZE}x{LUT_SIZE}x{LUT_SIZE} Negative LUT...")

try:
    with open(OUTPUT_FILENAME, 'w') as f:
        # --- Write Header ---
        print(f"Writing header to {OUTPUT_FILENAME}...")
        f.write(f'TITLE "Linear Negative LUT ({LUT_SIZE}x{LUT_SIZE}x{LUT_SIZE})"\n')
        f.write(f'# Created by Python Script\n\n')

        # Define the input domain (usually 0.0 to 1.0 for normalized color)
        f.write(f'DOMAIN_MIN 0.0 0.0 0.0\n')
        f.write(f'DOMAIN_MAX 1.0 1.0 1.0\n\n')

        # Define the size of the LUT grid
        f.write(f'LUT_3D_SIZE {LUT_SIZE}\n\n')
        f.write(f'# LUT data points (R G B)\n')

        # --- Write LUT Data ---
        print(f"Generating and writing {LUT_SIZE**3} data points...")
        count = 0
        divisor = float(LUT_SIZE - 1)  # Pre-calculate divisor for normalization

        # The CUBE format expects data sorted with R varying fastest, then G, then B.
        # Therefore, the outer loop is B, then G, then R.
        for b_idx in range(LUT_SIZE):
            for g_idx in range(LUT_SIZE):
                for r_idx in range(LUT_SIZE):

                    # Calculate the normalized input coordinate (0.0 to 1.0)
                    # corresponding to the current grid point index.
                    r_in = r_idx / divisor
                    g_in = g_idx / divisor
                    b_in = b_idx / divisor

                    # Apply the linear negative transformation
                    # output = 1.0 - input
                    r_out = 1.0 - r_in
                    g_out = 1.0 - g_in
                    b_out = 1.0 - b_in

                    # Clamp values just in case of floating point inaccuracies near 0 or 1
                    # (less critical for this simple 1-x transform, but good practice)
                    r_out = max(0.0, min(1.0, r_out))
                    g_out = max(0.0, min(1.0, g_out))
                    b_out = max(0.0, min(1.0, b_out))

                    # Format the output RGB values with the specified precision
                    line = f"{r_out:.{PRECISION}f} {g_out:.{PRECISION}f} {b_out:.{PRECISION}f}\n"

                    # Write the formatted line to the file
                    f.write(line)
                    count += 1

                    # Optional: Print progress indicator (can slow down generation)
                    # if count % (LUT_SIZE * LUT_SIZE * 10) == 0: # Print every few slices
                    #     progress = (count / (LUT_SIZE**3)) * 100
                    #     print(f"  Progress: {progress:.1f}%", end='\r')


    print(f"\nSuccessfully generated LUT file: {OUTPUT_FILENAME}")
    print(f"Total entries written: {count}")

except IOError as e:
    print(f"Error writing file {OUTPUT_FILENAME}: {e}")
except Exception as e:
    print(f"An unexpected error occurred: {e}")