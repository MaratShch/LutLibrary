import argparse
import sys
import os

def generate_identity_lut(lut_size: int, filename: str):
    """
    Generates a 3D identity CUBE LUT and saves it to a file.

    Args:
        lut_size: The size of the cube (number of points along each dimension).
        filename: The name of the .cube file to create.
    """
    if not filename.lower().endswith(".cube"):
        print(f"Warning: Output filename '{filename}' does not end with .cube", file=sys.stderr)
        # Optionally, you could enforce adding .cube here:
        # filename += ".cube"

    print(f"Generating {lut_size}x{lut_size}x{lut_size} identity LUT...")

    try:
        with open(filename, 'w') as f:
            # --- Write Header ---
            f.write(f'TITLE "Identity LUT {lut_size}x{lut_size}x{lut_size}"\n')
            f.write('\n') # Optional blank line, common practice
            f.write('# Created by Python script\n') # Optional comment
            f.write('\n')
            f.write(f'LUT_3D_SIZE {lut_size}\n')
            f.write('\n')
            # Define the domain (input range) - default 0.0 to 1.0
            f.write('DOMAIN_MIN 0.0 0.0 0.0\n')
            f.write('DOMAIN_MAX 1.0 1.0 1.0\n')
            f.write('\n')

            # --- Write LUT Data ---
            # The order is Blue fastest, then Green, then Red
            for r_idx in range(lut_size):
                for g_idx in range(lut_size):
                    for b_idx in range(lut_size):
                        # For identity LUT, output equals normalized input position
                        # Normalize index to 0.0 - 1.0 range
                        # Handle lut_size=1 case to avoid division by zero
                        if lut_size == 1:
                            r_val = 0.0
                            g_val = 0.0
                            b_val = 0.0
                        else:
                            r_val = r_idx / (lut_size - 1)
                            g_val = g_idx / (lut_size - 1)
                            b_val = b_idx / (lut_size - 1)

                        # Write the R G B triplet, space-separated
                        # Use sufficient precision (e.g., 6 decimal places)
                        f.write(f"{r_val:.12f} {g_val:.12f} {b_val:.12f}\n")

        print(f"Successfully generated identity LUT: '{os.path.abspath(filename)}'")

    except IOError as e:
        print(f"Error writing file '{filename}': {e}", file=sys.stderr)
        sys.exit(1) # Exit with error code
    except Exception as e:
        print(f"An unexpected error occurred: {e}", file=sys.stderr)
        sys.exit(1)


def main():
    """Parses command-line arguments and initiates LUT generation."""
    parser = argparse.ArgumentParser(
        description="Generate a 3D identity CUBE LUT file.",
        formatter_class=argparse.ArgumentDefaultsHelpFormatter # Shows defaults in help
    )

    parser.add_argument(
        "lut_size",
        type=int,
        help="The size of the LUT cube (number of points per dimension, e.g., 17, 33, 65)."
    )
    parser.add_argument(
        "filename",
        type=str,
        help="The desired output filename for the .cube LUT file."
    )

    # Although domain is fixed to 0-1, you could add arguments for it later:
    # parser.add_argument("--domain_min", type=float, default=0.0, help="Minimum domain value")
    # parser.add_argument("--domain_max", type=float, default=1.0, help="Maximum domain value")

    if len(sys.argv) == 1:
        parser.print_help(sys.stderr)
        sys.exit(1)

    args = parser.parse_args()

    if args.lut_size < 1:
         print(f"Error: LUT size must be at least 1.", file=sys.stderr)
         sys.exit(1)

    generate_identity_lut(args.lut_size, args.filename)


if __name__ == "__main__":
    main()
