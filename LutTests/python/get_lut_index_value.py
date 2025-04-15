import numpy as np
import sys
import argparse
import os
from itertools import product

# Reusing the LUT loading logic we refined earlier
def load_cube_lut_data_and_meta(lut_file_path):
    """
    Loads a 3D CUBE LUT file and returns its data table and metadata.

    Args:
        lut_file_path (str): The path to the .cube LUT file.

    Returns:
        tuple: A tuple containing:
            - lut_data (numpy.ndarray): The 3D LUT data (indexed R,G,B then Channel).
            - resolution (tuple): The actual resolutions (res_r, res_g, res_b).
            - success (bool): True if loading was successful, False otherwise.
    """
    try:
        with open(lut_file_path, 'r') as f:
            lines = f.readlines()

        resolution_val = None # Use explicit resolution variable
        lut_entries = []
        lut_data = None
        title = None

        # Default assumption for domain, CUBE files often omit them
        domain_min = [0.0, 0.0, 0.0]
        domain_max = [1.0, 1.0, 1.0]

        reading_data = False

        for line_num, line in enumerate(lines):
            line = line.strip()
            if not line or line.startswith('#'):
                continue

            parts = line.split()
            keyword = parts[0].upper()

            if not reading_data:
                if keyword == "TITLE" and len(parts) > 1:
                    title = line.split(maxsplit=1)[1].strip('"')
                    continue

                if keyword == "LUT_3D_SIZE" and len(parts) == 2 and parts[1].isdigit():
                    if resolution_val is not None:
                         print(f"Warning: Duplicate LUT_3D_SIZE found on line {line_num+1}.")
                         continue # Use first value
                    resolution_val = int(parts[1])
                    if resolution_val <= 1:
                         print(f"Error: LUT_3D_SIZE must be > 1, found {resolution_val} on line {line_num+1}.")
                         return None, None, False
                    # Create the numpy array *after* finding size
                    lut_data = np.zeros((resolution_val, resolution_val, resolution_val, 3), dtype=np.float64) # Use float64 for precision
                    print(f"Found LUT_3D_SIZE: {resolution_val}")
                    reading_data = True
                    continue

                if keyword == "DOMAIN_MIN" and len(parts) == 4:
                    try: domain_min = [float(p) for p in parts[1:]]
                    except ValueError: print(f"Warning: Invalid DOMAIN_MIN values on line {line_num+1}")
                    continue
                if keyword == "DOMAIN_MAX" and len(parts) == 4:
                    try: domain_max = [float(p) for p in parts[1:]]
                    except ValueError: print(f"Warning: Invalid DOMAIN_MAX values on line {line_num+1}")
                    continue
                if resolution_val is None and len(parts) == 3:
                     print(f"Warning: Data-like line before LUT_3D_SIZE on line {line_num+1}. Ignoring.")
                     continue

            elif reading_data: # Only process as data if size was found
                if len(parts) == 3:
                    try: lut_entries.append([float(p) for p in parts])
                    except ValueError: print(f"Warning: Skipping non-float data on line {line_num+1}")
                else: print(f"Warning: Unexpected line format in data section on line {line_num+1}. Ignoring.")

        if resolution_val is None:
            print(f"Error loading CUBE LUT: 'LUT_3D_SIZE' keyword not found or invalid.")
            return None, None, False

        expected_entries = resolution_val ** 3
        if len(lut_entries) != expected_entries:
             print(f"Error loading CUBE LUT: Expected {expected_entries} data points, found {len(lut_entries)}.")
             return None, None, False

        # Populate NumPy Array according to CUBE standard (B varies slowest, R varies fastest)
        # We will store it in [R, G, B, Channel] order for easier lookup later
        index = 0
        for b_idx in range(resolution_val):
            for g_idx in range(resolution_val):
                for r_idx in range(resolution_val):
                    # Store in [R][G][B] -> Channel layout
                    lut_data[r_idx, g_idx, b_idx] = lut_entries[index]
                    index += 1

        actual_resolution = (resolution_val, resolution_val, resolution_val) # Assuming cubic
        print(f"Successfully parsed {len(lut_entries)} data points for LUT '{title if title else 'Untitled'}' (Res: {resolution_val}x{resolution_val}x{resolution_val}).")
        return lut_data, actual_resolution, True # Return actual shape/resolution

    except FileNotFoundError:
        print(f"Error: LUT file not found at '{lut_file_path}'")
        return None, None, False
    except Exception as e:
        print(f"An unexpected error occurred while loading LUT from '{lut_file_path}': {e}")
        import traceback
        traceback.print_exc()
        return None, None, False


def get_lut_value_at_index(lut_data, resolution_tuple, r_idx, g_idx, b_idx):
    """
    Retrieves the RGB value from the LUT data at the specified integer indices.

    Args:
        lut_data (numpy.ndarray): The 3D LUT data (indexed R, G, B, Channel).
        resolution_tuple (tuple): Tuple of (res_r, res_g, res_b).
        r_idx (int): The Red index (0 to res_r - 1).
        g_idx (int): The Green index (0 to res_g - 1).
        b_idx (int): The Blue index (0 to res_b - 1).

    Returns:
        list: The [R, G, B] list at that index, or None if indices are out of bounds.
    """
    res_r, res_g, res_b = resolution_tuple

    # Check if indices are within the valid range
    if not (0 <= r_idx < res_r and 0 <= g_idx < res_g and 0 <= b_idx < res_b):
        print(f"Error: Indices ({r_idx}, {g_idx}, {b_idx}) are out of bounds for LUT with resolution ({res_r}x{res_g}x{res_b}).")
        return None

    # Access the data using direct indexing
    try:
        # Accessing assuming R,G,B index order established during loading
        rgb_value = lut_data[r_idx, g_idx, b_idx]
        return rgb_value.tolist() # Convert NumPy array slice to Python list
    except IndexError:
        # This shouldn't happen if the bounds check above works, but included for safety
        print(f"Error: Internal indexing error accessing ({r_idx}, {g_idx}, {b_idx}).")
        return None
    except Exception as e:
        print(f"Error retrieving LUT value at index ({r_idx}, {g_idx}, {b_idx}): {e}")
        return None


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description='Load a .cube LUT and retrieve the value at specific integer grid indices (R, G, B).',
        formatter_class=argparse.ArgumentDefaultsHelpFormatter
    )
    parser.add_argument('lut_file', type=str, help='Path to the 3D CUBE LUT file.')
    parser.add_argument('r_index', type=int, help='RED integer index (0 to LUT_SIZE-1)')
    parser.add_argument('g_index', type=int, help='GREEN integer index (0 to LUT_SIZE-1)')
    parser.add_argument('b_index', type=int, help='BLUE integer index (0 to LUT_SIZE-1)')
    args = parser.parse_args()

    print(f"--- Attempting to load LUT: {args.lut_file} ---")

    lut_data, resolution, loaded_successfully = load_cube_lut_data_and_meta(args.lut_file)

    if loaded_successfully:
        print(f"\n--- Retrieving value at index R={args.r_index}, G={args.g_index}, B={args.b_index} ---")
        rgb_value = get_lut_value_at_index(lut_data, resolution, args.r_index, args.g_index, args.b_index)

        if rgb_value is not None and isinstance(rgb_value, list) and len(rgb_value) == 3:
             # Print with high precision
            np.set_printoptions(precision=16, suppress=True) # Affects numpy array printing
            # Explicit formatting for list elements:
            print(f"Value at LUT[{args.r_index}][{args.g_index}][{args.b_index}]: "
                  f"[{rgb_value[0]:.17g}, {rgb_value[1]:.17g}, {rgb_value[2]:.17g}]")
            print("(Using R, G, B indexing)")
        else:
            print("Failed to retrieve value at the specified index.")

    else:
        print(f"\nFailed to load LUT '{args.lut_file}'.")
        sys.exit(1)