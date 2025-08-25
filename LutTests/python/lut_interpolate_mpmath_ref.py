#!/usr/bin/env python3

#
# Run with default precision (16 digits)
#   python lut_interpolate_ref.py YourLutFile.cube
#
# Run with 50 digits of precision
#   python lut_interpolate_ref.py YourLutFile.cube --precision 50
#
# Run with 10 digits of precision
#   python lut_interpolate_ref.py YourLutFile.cube --precision 10
#
import numpy as np
import warnings
import argparse
from itertools import product
import sys
import time
import mpmath # <-- ADDED: For high-precision math

# --- UTILITY FUNCTIONS ---

def _numpy_to_mpmath(np_array):
    """Recursively converts a NumPy array to a nested list of mpmath.mpf objects."""
    if np_array.ndim == 1:
        return [mpmath.mpf(str(x)) for x in np_array]
    else:
        return [_numpy_to_mpmath(sub_array) for sub_array in np_array]

def _clip_mpf(value, min_val, max_val):
    """mpmath equivalent of np.clip."""
    return mpmath.fmax(min_val, mpmath.fmin(value, max_val))

def _dot_product_mpf(weights, points):
    """Manual dot product for lists of mpmath.mpf objects."""
    # points is a list of [R,G,B] mpf lists. weights is a list of scalar mpf.
    result = [mpmath.mpf('0.0')] * 3
    for i in range(len(weights)):
        result[0] += weights[i] * points[i][0]
        result[1] += weights[i] * points[i][1]
        result[2] += weights[i] * points[i][2]
    return result

# --- FILE PARSING ---

def load_cube_lut(lut_file_path):
    """
    Loads a 3D CUBE LUT file, ensuring float64 precision during parsing.
    The internal numpy array is created with [R, G, B, Channel] indexing.
    """
    # ... (This function is largely the same but ensures float64 for better base precision) ...
    # --- NOTE: A bug in the original code's comment was fixed. The loading logic
    #     for b..g..r and assignment to lut_data[r,g,b] is correct for R-fastest.
    try:
        with open(lut_file_path, 'r') as f:
            lines = f.readlines()

        resolution = None
        lut_entries = []
        title = "Untitled"
        domain_min = [0.0, 0.0, 0.0]
        domain_max = [1.0, 1.0, 1.0]
        reading_data = False

        for line_num, line in enumerate(lines):
            line = line.strip()
            if not line or line.startswith('#'): continue
            parts = line.split()
            keyword = parts[0].upper()

            if not reading_data or any(x in keyword for x in ['TITLE', 'SIZE', 'DOMAIN']):
                if keyword == "TITLE" and len(parts) > 1:
                    title = line.split(maxsplit=1)[1].strip('"')
                elif keyword == "LUT_3D_SIZE" and len(parts) == 2 and parts[1].isdigit():
                    if resolution is not None: continue # Use first
                    resolution = int(parts[1])
                elif keyword == "DOMAIN_MIN" and len(parts) == 4:
                    try: domain_min = [float(p) for p in parts[1:]]
                    except ValueError: pass
                elif keyword == "DOMAIN_MAX" and len(parts) == 4:
                    try: domain_max = [float(p) for p in parts[1:]]
                    except ValueError: pass
            
            if resolution is not None:
                reading_data = True

            if reading_data and len(parts) == 3:
                try:
                    entry = [float(p) for p in parts]
                    lut_entries.append(entry)
                except ValueError:
                    pass # Skip non-numeric data lines

        if resolution is None:
            raise RuntimeError(f"Could not find 'LUT_3D_SIZE' in '{lut_file_path}'.")

        expected_entries = resolution ** 3
        if len(lut_entries) != expected_entries:
            raise RuntimeError(f"Expected {expected_entries} data points, found {len(lut_entries)}.")

        # Using float64 for best possible precision from file before mpmath conversion
        lut_data = np.zeros((resolution, resolution, resolution, 3), dtype=np.float64)
        index = 0
        # CUBE format specifies R varies fastest, G next, B slowest.
        for b_idx in range(resolution):
            for g_idx in range(resolution):
                for r_idx in range(resolution):
                    lut_data[r_idx, g_idx, b_idx] = lut_entries[index]
                    index += 1
        
        print(f"Successfully loaded CUBE LUT: '{title}' (Res: {resolution})")
        return lut_data, resolution, domain_min, domain_max, True

    except Exception as e:
        print(f"Error loading LUT: {e}", file=sys.stderr)
        return None, None, None, None, False


# --- INTERPOLATION ALGORITHMS (Refactored for mpmath) ---
# NOTE: The redundant _prepare_inputs_for_64bit function has been removed.
#       All functions now expect mpmath types as input.

def _linear_interpolate_1d_slice(lut_slice_1d, resolution_1d, x):
    """High-precision 1D linear interpolation helper. Assumes mpmath types."""
    res_minus_1 = mpmath.mpf(str(resolution_1d - 1))
    f_index = x * res_minus_1
    
    idx0 = int(mpmath.floor(f_index))
    idx1 = int(mpmath.ceil(f_index))
    idx0 = max(0, min(resolution_1d - 1, idx0))
    idx1 = max(0, min(resolution_1d - 1, idx1))

    if idx0 == idx1: return lut_slice_1d[idx0]

    t = f_index - idx0
    val0 = lut_slice_1d[idx0]
    val1 = lut_slice_1d[idx1]
    
    one = mpmath.mpf('1.0')
    # Element-wise lerp for [R,G,B]
    res = [v0 * (one - t) + v1 * t for v0, v1 in zip(val0, val1)]
    return res


def nearest_neighbor_interpolation(lut_data, resolution, r, g, b, domain_min, domain_max):
    """Performs Nearest Neighbor interpolation using mpmath."""
    res_r, res_g, res_b = (resolution, resolution, resolution) if isinstance(resolution, int) else resolution
    r_c = _clip_mpf(r, mpmath.mpf('0.0'), mpmath.mpf('1.0'))
    g_c = _clip_mpf(g, mpmath.mpf('0.0'), mpmath.mpf('1.0'))
    b_c = _clip_mpf(b, mpmath.mpf('0.0'), mpmath.mpf('1.0'))

    idx_r = int(mpmath.nint(r_c * (res_r - 1)))
    idx_g = int(mpmath.nint(g_c * (res_g - 1)))
    idx_b = int(mpmath.nint(b_c * (res_b - 1)))

    idx_r = max(0, min(res_r - 1, idx_r))
    idx_g = max(0, min(res_g - 1, idx_g))
    idx_b = max(0, min(res_b - 1, idx_b))
    
    result_val = lut_data[idx_r][idx_g][idx_b]
    
    clamped_val = [
        _clip_mpf(result_val[0], domain_min[0], domain_max[0]),
        _clip_mpf(result_val[1], domain_min[1], domain_max[1]),
        _clip_mpf(result_val[2], domain_min[2], domain_max[2]),
    ]
    return clamped_val


def linear_interpolation(lut_data, resolution, r, g, b, domain_min, domain_max):
    """Performs 1D-Slice Linear interpolation (R-axis) using mpmath."""
    res_r, res_g, res_b = (resolution, resolution, resolution) if isinstance(resolution, int) else resolution
    g_c = _clip_mpf(g, mpmath.mpf('0.0'), mpmath.mpf('1.0'))
    b_c = _clip_mpf(b, mpmath.mpf('0.0'), mpmath.mpf('1.0'))

    g_idx = int(mpmath.nint(g_c * (res_g - 1)))
    b_idx = int(mpmath.nint(b_c * (res_b - 1)))
    g_idx = max(0, min(res_g - 1, g_idx))
    b_idx = max(0, min(res_b - 1, b_idx))

    # Manually create the 1D slice
    lut_slice_1d = [lut_data[i][g_idx][b_idx] for i in range(res_r)]
    
    r_c = _clip_mpf(r, mpmath.mpf('0.0'), mpmath.mpf('1.0'))
    interpolated_val = _linear_interpolate_1d_slice(lut_slice_1d, res_r, r_c)

    clamped_val = [
        _clip_mpf(interpolated_val[0], domain_min[0], domain_max[0]),
        _clip_mpf(interpolated_val[1], domain_min[1], domain_max[1]),
        _clip_mpf(interpolated_val[2], domain_min[2], domain_max[2]),
    ]
    return clamped_val


def bilinear_interpolation(lut_data, resolution, r, g, b, domain_min, domain_max):
    """Performs 2D-Slice Bilinear interpolation (RG-plane) using mpmath."""
    res_r, res_g, res_b = (resolution, resolution, resolution) if isinstance(resolution, int) else resolution
    b_c = _clip_mpf(b, mpmath.mpf('0.0'), mpmath.mpf('1.0'))
    b_idx = int(mpmath.nint(b_c * (res_b - 1)))
    b_idx = max(0, min(res_b - 1, b_idx))

    r_c = _clip_mpf(r, mpmath.mpf('0.0'), mpmath.mpf('1.0'))
    g_c = _clip_mpf(g, mpmath.mpf('0.0'), mpmath.mpf('1.0'))
    
    if res_r <= 1 or res_g <= 1: # Fallback to 1D linear
        return linear_interpolation(lut_data, resolution, r, g, b, domain_min, domain_max)

    fx = r_c * (res_r - 1)
    fy = g_c * (res_g - 1)
    x0, y0 = int(mpmath.floor(fx)), int(mpmath.floor(fy))
    x1, y1 = int(mpmath.ceil(fx)), int(mpmath.ceil(fy))
    x0, x1 = max(0, min(res_r-1, x0)), max(0, min(res_r-1, x1))
    y0, y1 = max(0, min(res_g-1, y0)), max(0, min(res_g-1, y1))

    tx = fx - x0
    ty = fy - y0
    
    c00 = lut_data[x0][y0][b_idx]
    c10 = lut_data[x1][y0][b_idx]
    c01 = lut_data[x0][y1][b_idx]
    c11 = lut_data[x1][y1][b_idx]

    c0 = _linear_interpolate_1d_slice([c00, c10], 2, tx) if x0 != x1 else c00
    c1 = _linear_interpolate_1d_slice([c01, c11], 2, tx) if x0 != x1 else c01
    interpolated_val = _linear_interpolate_1d_slice([c0, c1], 2, ty) if y0 != y1 else c0

    clamped_val = [
        _clip_mpf(interpolated_val[0], domain_min[0], domain_max[0]),
        _clip_mpf(interpolated_val[1], domain_min[1], domain_max[1]),
        _clip_mpf(interpolated_val[2], domain_min[2], domain_max[2]),
    ]
    return clamped_val


def trilinear_interpolation(lut_data, resolution, r, g, b, domain_min, domain_max):
    """Performs Trilinear interpolation using mpmath."""
    res_r, res_g, res_b = (resolution, resolution, resolution) if isinstance(resolution, int) else resolution
    r_c = _clip_mpf(r, mpmath.mpf('0.0'), mpmath.mpf('1.0'))
    g_c = _clip_mpf(g, mpmath.mpf('0.0'), mpmath.mpf('1.0'))
    b_c = _clip_mpf(b, mpmath.mpf('0.0'), mpmath.mpf('1.0'))
    
    if res_r < 2 or res_g < 2 or res_b < 2:
        return nearest_neighbor_interpolation(lut_data, resolution, r, g, b, domain_min, domain_max)

    fx = r_c * (res_r - 1)
    fy = g_c * (res_g - 1)
    fz = b_c * (res_b - 1)
    
    x0, y0, z0 = int(mpmath.floor(fx)), int(mpmath.floor(fy)), int(mpmath.floor(fz))
    x1, y1, z1 = int(mpmath.ceil(fx)), int(mpmath.ceil(fy)), int(mpmath.ceil(fz))
    
    x0, x1 = max(0, min(res_r-1, x0)), max(0, min(res_r-1, x1))
    y0, y1 = max(0, min(res_g-1, y0)), max(0, min(res_g-1, y1))
    z0, z1 = max(0, min(res_b-1, z0)), max(0, min(res_b-1, z1))

    tx = fx - x0
    ty = fy - y0
    tz = fz - z0

    c000 = lut_data[x0][y0][z0]; c100 = lut_data[x1][y0][z0]
    c010 = lut_data[x0][y1][z0]; c110 = lut_data[x1][y1][z0]
    c001 = lut_data[x0][y0][z1]; c101 = lut_data[x1][y0][z1]
    c011 = lut_data[x0][y1][z1]; c111 = lut_data[x1][y1][z1]

    c00 = _linear_interpolate_1d_slice([c000, c100], 2, tx) if x0 != x1 else c000
    c10 = _linear_interpolate_1d_slice([c010, c110], 2, tx) if x0 != x1 else c010
    c01 = _linear_interpolate_1d_slice([c001, c101], 2, tx) if x0 != x1 else c001
    c11 = _linear_interpolate_1d_slice([c011, c111], 2, tx) if x0 != x1 else c011

    c0 = _linear_interpolate_1d_slice([c00, c10], 2, ty) if y0 != y1 else c00
    c1 = _linear_interpolate_1d_slice([c01, c11], 2, ty) if y0 != y1 else c01
    
    interpolated_val = _linear_interpolate_1d_slice([c0, c1], 2, tz) if z0 != z1 else c0

    clamped_val = [
        _clip_mpf(interpolated_val[0], domain_min[0], domain_max[0]),
        _clip_mpf(interpolated_val[1], domain_min[1], domain_max[1]),
        _clip_mpf(interpolated_val[2], domain_min[2], domain_max[2]),
    ]
    return clamped_val


def tetrahedral_interpolation(lut_data, resolution, r, g, b, domain_min, domain_max):
    """Performs Tetrahedral interpolation using mpmath. CORRECTED LOGIC."""
    res_r, res_g, res_b = (resolution, resolution, resolution) if isinstance(resolution, int) else resolution
    r_c = _clip_mpf(r, mpmath.mpf('0.0'), mpmath.mpf('1.0'))
    g_c = _clip_mpf(g, mpmath.mpf('0.0'), mpmath.mpf('1.0'))
    b_c = _clip_mpf(b, mpmath.mpf('0.0'), mpmath.mpf('1.0'))

    if res_r < 2 or res_g < 2 or res_b < 2:
        return trilinear_interpolation(lut_data, resolution, r, g, b, domain_min, domain_max)

    fx = r_c * (res_r - 1)
    fy = g_c * (res_g - 1)
    fz = b_c * (res_b - 1)
    
    x0, y0, z0 = int(mpmath.floor(fx)), int(mpmath.floor(fy)), int(mpmath.floor(fz))
    x1, y1, z1 = max(0, min(res_r-1, x0+1)), max(0, min(res_g-1, y0+1)), max(0, min(res_b-1, z0+1))
    x0, y0, z0 = max(0, min(res_r-1, x0)), max(0, min(res_g-1, y0)), max(0, min(res_b-1, z0))

    tx, ty, tz = fx - x0, fy - y0, fz - z0

    c000 = lut_data[x0][y0][z0]; c100 = lut_data[x1][y0][z0]
    c010 = lut_data[x0][y1][z0]; c001 = lut_data[x0][y0][z1]
    c110 = lut_data[x1][y1][z0]; c101 = lut_data[x1][y0][z1]
    c011 = lut_data[x0][y1][z1]; c111 = lut_data[x1][y1][z1]
    
    # Standard decomposition based on fractional part comparison
    if tx > ty:
        if ty > tz: # R > G > B
            p1 = [c100[i] - c000[i] for i in range(3)]
            p2 = [c110[i] - c100[i] for i in range(3)]
            p3 = [c111[i] - c110[i] for i in range(3)]
            interpolated_val = [c000[i] + p1[i] * tx + p2[i] * ty + p3[i] * tz for i in range(3)]
        elif tx > tz: # R > B > G
            p1 = [c100[i] - c000[i] for i in range(3)]
            p2 = [c101[i] - c100[i] for i in range(3)]
            p3 = [c111[i] - c101[i] for i in range(3)]
            interpolated_val = [c000[i] + p1[i] * tx + p2[i] * tz + p3[i] * ty for i in range(3)]
        else: # B > R > G
            p1 = [c001[i] - c000[i] for i in range(3)]
            p2 = [c101[i] - c001[i] for i in range(3)]
            p3 = [c111[i] - c101[i] for i in range(3)]
            interpolated_val = [c000[i] + p1[i] * tz + p2[i] * tx + p3[i] * ty for i in range(3)]
    else: # G >= R
        if tz > ty: # B > G > R
            p1 = [c001[i] - c000[i] for i in range(3)]
            p2 = [c011[i] - c001[i] for i in range(3)]
            p3 = [c111[i] - c011[i] for i in range(3)]
            interpolated_val = [c000[i] + p1[i] * tz + p2[i] * ty + p3[i] * tx for i in range(3)]
        elif tz > tx: # G > B > R
            p1 = [c010[i] - c000[i] for i in range(3)]
            p2 = [c011[i] - c010[i] for i in range(3)]
            p3 = [c111[i] - c011[i] for i in range(3)]
            interpolated_val = [c000[i] + p1[i] * ty + p2[i] * tz + p3[i] * tx for i in range(3)]
        else: # G > R > B
            p1 = [c010[i] - c000[i] for i in range(3)]
            p2 = [c110[i] - c010[i] for i in range(3)]
            p3 = [c111[i] - c110[i] for i in range(3)]
            interpolated_val = [c000[i] + p1[i] * ty + p2[i] * tx + p3[i] * tz for i in range(3)]

    clamped_val = [
        _clip_mpf(interpolated_val[0], domain_min[0], domain_max[0]),
        _clip_mpf(interpolated_val[1], domain_min[1], domain_max[1]),
        _clip_mpf(interpolated_val[2], domain_min[2], domain_max[2]),
    ]
    return clamped_val

# --- Main Execution Block ---

if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description='Load a 3D CUBE LUT file and perform high-precision interpolations using mpmath.',
        formatter_class=argparse.ArgumentDefaultsHelpFormatter
    )
    parser.add_argument('lut_file_path', type=str, help='Path to the 3D CUBE LUT file.')
    parser.add_argument('--precision', type=int, default=16, choices=range(4, 51),
                        help='Number of decimal digits for calculation and output (4-50).')
    args = parser.parse_args()

    # --- Set mpmath Precision ---
    try:
        mpmath.mp.dps = args.precision
        print(f"--- Using mpmath with {mpmath.mp.dps} decimal digits of precision ---")
    except Exception as e:
        print(f"Error setting mpmath precision: {e}", file=sys.stderr)
        sys.exit(1)

    # --- Load LUT Data ---
    lut_data_np, resolution, domain_min_list, domain_max_list, loaded = load_cube_lut(args.lut_file_path)

    if not loaded:
        sys.exit(1)

    # --- Convert all data to mpmath high-precision types ONCE ---
    print("Converting loaded LUT data to high-precision mpmath format...")
    lut_data_mp = _numpy_to_mpmath(lut_data_np)
    domain_min_mp = [mpmath.mpf(str(d)) for d in domain_min_list]
    domain_max_mp = [mpmath.mpf(str(d)) for d in domain_max_list]
    print("Conversion complete.")

    # List of test points to interpolate
    rgb_values_to_interpolate = [
        (0.0, 0.0, 0.0), (1.0, 1.0, 1.0), (0.1, 0.5, 0.9),
        (0.5, 0.5, 0.5), (0.75, 0.75, 0.75), (0.8, 0.2, 0.4),
        (1/3, 1/7, 1/6), (0.25, 0.5, 0.75), (-0.1, 0.5, 1.1)
    ]

    print("\n--- Starting Interpolation Tests ---")
    # Define the format string for printing based on user-requested precision
    print_format = f"  {{:<20}} : [{{:.{args.precision}f}}, {{:.{args.precision}f}}, {{:.{args.precision}f}}]"

    for r_in, g_in, b_in in rgb_values_to_interpolate:
        # Convert input point to mpmath for this iteration
        r_mp = mpmath.mpf(str(r_in))
        g_mp = mpmath.mpf(str(g_in))
        b_mp = mpmath.mpf(str(b_in))

        print(f"\nInput RGB: ({r_in:.8f}, {g_in:.8f}, {b_in:.8f})")
        
        try:
            res_nn = nearest_neighbor_interpolation(lut_data_mp, resolution, r_mp, g_mp, b_mp, domain_min_mp, domain_max_mp)
            print(print_format.format("Nearest Neighbor", *res_nn))
        except Exception as e: print(f"  Nearest Neighbor     : Error - {e}")
        
        try:
            res_lin = linear_interpolation(lut_data_mp, resolution, r_mp, g_mp, b_mp, domain_min_mp, domain_max_mp)
            print(print_format.format("Linear (1D Slice)", *res_lin))
        except Exception as e: print(f"  Linear (1D Slice)    : Error - {e}")

        try:
            res_bilin = bilinear_interpolation(lut_data_mp, resolution, r_mp, g_mp, b_mp, domain_min_mp, domain_max_mp)
            print(print_format.format("Bilinear (2D Slice)", *res_bilin))
        except Exception as e: print(f"  Bilinear (2D Slice)  : Error - {e}")

        try:
            res_trilin = trilinear_interpolation(lut_data_mp, resolution, r_mp, g_mp, b_mp, domain_min_mp, domain_max_mp)
            print(print_format.format("Trilinear", *res_trilin))
        except Exception as e: print(f"  Trilinear            : Error - {e}")
        
        try:
            res_tetra = tetrahedral_interpolation(lut_data_mp, resolution, r_mp, g_mp, b_mp, domain_min_mp, domain_max_mp)
            print(print_format.format("Tetrahedral", *res_tetra))
        except Exception as e: print(f"  Tetrahedral          : Error - {e}")