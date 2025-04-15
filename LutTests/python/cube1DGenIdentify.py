import argparse
import sys
import os

def generate_identity_1d_lut(lut_size: int, filename: str):
    """
    Generates a 1D identity CUBE LUT and saves it to a file.

    Args:
        lut_size: The size of the LUT (number of entries). Must be >= 2.
        filename: The name of the .cube file to create.
    """
    if lut_size < 2:
        print(f"Error: LUT_1D_SIZE must be 2 or greater.", file=sys.stderr)
        sys.exit(1)

    if not filename.lower().endswith(".cube"):
        print(f"Warning: Output filename '{filename}' does not end with .cube", file=sys.stderr)
        # Optionally, you could enforce adding .cube here:
        # filename += ".cube"

    print(f"Generating {lut_size}-entry 1D identity LUT...")

    try:
        with open(filename, 'w') as f:
            # --- Write Header ---
            f.write(f'TITLE "Identity 1D LUT {lut_size}"\n')
            f.write('\n')
            f.write('# Created by Python script\n')
            f.write('\n')
            # Key change for 1D LUT
            f.write(f'LUT_1D_SIZE {lut_size}\n')
            f.write('\n')
            # Define the domain (input range) - default 0.0 to 1.0
            # Even for 1D, specifying RGB domains is common and safe
            f.write('DOMAIN_MIN 0.0 0.0 0.0\n')
            f.write('DOMAIN_MAX 1.0 1.0 1.0\n')
            f.write('\n')

            # --- Write LUT Data ---
            # For 1D LUT, we just loop through the number of entries
            for i in range(lut_size):
                # For identity LUT, output equals normalized input position
                # Normalize index to 0.0 - 1.0 range
                # Since lut_size >= 2, lut_size - 1 will not be zero
                value = i / (lut_size - 1)

                # Write the R G B triplet, space-separated
                # For identity 1D, R=G=B=value
                f.write(f"{value:.12f} {value:.12f} {value:.12f}\n")

        print(f"Successfully generated 1D identity LUT: '{os.path.abspath(filename)}'")

    except IOError as e:
        print(f"Error writing file '{filename}': {e}", file=sys.stderr)
        sys.exit(1) # Exit with error code
    except Exception as e:
        print(f"An unexpected error occurred: {e}", file=sys.stderr)
        sys.exit(1)


def main():
    """Parses command-line arguments and initiates 1D LUT generation."""
    parser = argparse.ArgumentParser(
        description="Generate a 1D identity CUBE LUT file.",
        formatter_class=argparse.ArgumentDefaultsHelpFormatter
    )

    parser.add_argument(
        "lut_size",
        type=int,
        help="The size of the 1D LUT (number of entries, e.g., 1024, 4096). Must be >= 2."
    )
    parser.add_argument(
        "filename",
        type=str,
        help="The desired output filename for the .cube LUT file."
    )

    if len(sys.argv) == 1:
        parser.print_help(sys.stderr)
        sys.exit(1)

    args = parser.parse_args()

    # The check for lut_size < 2 is now inside generate_identity_1d_lut

    generate_identity_1d_lut(args.lut_size, args.filename)


if __name__ == "__main__":
    main()
