import math
import sys
import argparse # Import the argparse library

def generate_negative_1d_lut(size: int, filename: str):
    """
    Generates a 1D CUBE LUT file that applies a 'negative' effect.

    Args:
        size: The number of entries in the LUT (e.g., 1024, 4096). Must be >= 2.
        filename: The path to the output .cube file.
    """
    if size < 2:
        print(f"Error: LUT size must be at least 2. Received: {size}")
        sys.exit(1) # Exit script if size is invalid

    print(f"Generating a {size}-entry 1D Negative LUT: {filename}")

    try:
        with open(filename, 'w') as f:
            # --- Write Header ---
            f.write(f'TITLE "Generated Negative 1D LUT ({size})"\n')
            f.write(f'LUT_1D_SIZE {size}\n')
            f.write('DOMAIN_MIN 0.0 0.0 0.0\n')
            f.write('DOMAIN_MAX 1.0 1.0 1.0\n')
            f.write('\n# LUT Data Points (R G B)\n')

            # --- Write Data Points ---
            for i in range(size):
                input_val = i / (size - 1.0)
                output_val = 1.0 - input_val
                # Clamping is good practice, though not strictly needed here
                output_val = max(0.0, min(1.0, output_val))
                f.write(f"{output_val:.10f} {output_val:.10f} {output_val:.10f}\n")

        print("LUT generation complete.")

    except IOError as e:
        print(f"Error writing file {filename}: {e}")
        sys.exit(1)
    except Exception as e:
        print(f"An unexpected error occurred: {e}")
        sys.exit(1)

# --- Main execution block ---
if __name__ == "__main__":
    # Check Python version (optional)
    print(f"Running on Python version: {sys.version}")

    # --- Setup Command Line Argument Parsing ---
    parser = argparse.ArgumentParser(
        description="Generate a 1D Negative CUBE LUT file.",
        formatter_class=argparse.ArgumentDefaultsHelpFormatter # Nicer help message format
    )

    # Add the required argument for LUT size
    parser.add_argument(
        "size", # Name of the argument
        type=int, # Expected data type (automatically validates input)
        help="The number of entries (size) for the 1D LUT. Must be an integer >= 2."
    )

    # Add an optional argument for the output filename
    parser.add_argument(
        "-o", "--output", # Flags for the argument
        type=str, # Expected data type
        default=None, # Default value if not provided
        help="Optional: Specify the output filename. If omitted, defaults to 'negative_1D_<size>.cube'."
    )

    # --- Parse the arguments provided by the user ---
    args = parser.parse_args()

    # --- Get the values from the parsed arguments ---
    lut_size = args.size

    # Determine the output filename
    if args.output:
        output_filename = args.output
    else:
        # Create default filename if not specified
        output_filename = f"negative_1D_{lut_size}.cube"

    # Ensure the filename ends with .cube (optional, but good practice)
    if not output_filename.lower().endswith(".cube"):
        print(f"Warning: Output filename '{output_filename}' does not end with .cube. Appending it.")
        output_filename += ".cube"


    # --- Call the LUT generation function ---
    generate_negative_1d_lut(lut_size, output_filename)
