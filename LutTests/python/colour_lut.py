import colour # The main colour-science library
import numpy as np
import sys
import argparse

def interpolate_with_colour_science(lut_file_path, rgb_point_tuple):
    """
    Loads a CUBE LUT using colour-science and interpolates an RGB point.

    Args:
        lut_file_path (str): Path to the .cube file.
        rgb_point_tuple (tuple): A tuple of (R, G, B) coordinates (0.0-1.0).

    Returns:
        None: Prints the results directly.
    """
    print(f"--- Processing LUT: {lut_file_path} ---")

    # 1. Load the LUT
    try:
        # colour.read_LUT will auto-detect the format based on extension/content
        lut = colour.read_LUT(lut_file_path)
        print(f"Successfully loaded LUT: '{lut.name}'")

        # Verify it's a 3D LUT (read_LUT can read different formats)
        if not isinstance(lut, colour.LUT3D):
            print(f"Error: The loaded file is not a 3D LUT (Type: {type(lut)}).")
            return

        # --- TEMPORARY: Add this to inspect methods ---
        print("\n--- Available attributes/methods on LUT object ---")
        print(dir(lut))
        print("----------------------------------------------\n")
        # --- End Temporary Inspection ---
 
        print(f"LUT Resolution: {lut.table.shape[0]}x{lut.table.shape[1]}x{lut.table.shape[2]}")
        print(f"LUT Domain Min: {lut.domain[0]}, Max: {lut.domain[1]}") # Domain from LUT

    except Exception as e:
        print(f"Error loading LUT file '{lut_file_path}': {e}")
        return

    # 2. Prepare the input point
    # colour-science typically expects NumPy arrays for color values
    rgb_input_np = np.array(rgb_point_tuple, dtype=np.float64)
    print(f"\nInput RGB:        {rgb_input_np.tolist()}")

    # Clamp input point to nominal [0, 1] range if necessary, although
    # colour-science often handles extrapolation or has internal mechanisms.
    # For direct comparison, ensure inputs are the same as your C++ test.
    # rgb_input_np = np.clip(rgb_input_np, 0.0, 1.0) # Optional clamping

    # 3. Perform Interpolations
    print("Interpolation Results:")

    # --- Trilinear Interpolation ---
    try:
        # 'linear' usually maps to trilinear for LUT3D in colour-science
        result_trilinear = lut.apply(rgb_input_np, method='linear')
        print(f"  Trilinear ('linear'): {result_trilinear.tolist()}")
    except Exception as e:
        print(f"  Trilinear ('linear'): Error - {e}")

    # --- Tetrahedral Interpolation ---
    try:
        result_tetra = lut.apply(rgb_input_np, method='tetrahedral')
        print(f"  Tetrahedral:        {result_tetra.tolist()}")
    except Exception as e:
        print(f"  Tetrahedral:        Error - {e}")

    # --- Other potential methods (check library version for support) ---
    # 'nearest' is usually available
    try:
        result_nearest = lut.apply(rgb_input_np, method='nearest')
        print(f"  Nearest Neighbor:   {result_nearest.tolist()}")
    except Exception as e:
        print(f"  Nearest Neighbor:   Error - {e}")

    # Note: colour-science's 'spline' interpolation might differ from simple
    # kernel methods, often involving more complex fitting. Check docs.
    # try:
    #     result_spline = lut.apply(rgb_input_np, method='spline')
    #     print(f"  Spline:             {result_spline}")
    # except Exception as e:
    #     print(f"  Spline:             Error - {e}")

    print("\nNote: colour-science output is typically unclamped unless the LUT itself contains clamped values.")
    print("      You may need to clamp results manually to match DOMAIN_MIN/MAX if needed.")
    print("--- Processing Finished ---")


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description='Load a .cube LUT with colour-science and interpolate a point.',
        formatter_class=argparse.ArgumentDefaultsHelpFormatter
    )
    parser.add_argument(
        'lut_file',
        type=str,
        help='Path to the 3D CUBE LUT file.'
    )
    parser.add_argument(
        'r', type=float, help='Red input coordinate (0.0-1.0)'
    )
    parser.add_argument(
        'g', type=float, help='Green input coordinate (0.0-1.0)'
    )
    parser.add_argument(
        'b', type=float, help='Blue input coordinate (0.0-1.0)'
    )
    args = parser.parse_args()

    input_rgb = (args.r, args.g, args.b)

    interpolate_with_colour_science(args.lut_file, input_rgb)
