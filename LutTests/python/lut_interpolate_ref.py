import numpy as np
import argparse
from itertools import product
import sys
import time

def load_cube_lut(lut_file_path):
    """Loads a 3D CUBE LUT file."""
    try:
        with open(lut_file_path, 'r') as f:
            lines = f.readlines()

        resolution = None
        lut_entries = []
        lut_data = None # Initialize lut_data here
        title = None
        domain_min = [0.0, 0.0, 0.0]
        domain_max = [1.0, 1.0, 1.0]

        reading_data = False # Flag to indicate when we should expect data points

        for line_num, line in enumerate(lines):
            line = line.strip()
            if not line or line.startswith('#'):
                continue # Skip comments and empty lines

            parts = line.split()

            # --- Parse Header Lines ---
            if not reading_data:
                keyword = parts[0].upper() # Check keywords case-insensitively

                if keyword == "TITLE" and len(parts) > 1:
                    title = line.split(maxsplit=1)[1].strip('"') # Extract title string
                    continue # Move to next line

                if keyword == "LUT_3D_SIZE" and len(parts) == 2 and parts[1].isdigit():
                    if resolution is not None:
                         print(f"Warning: Duplicate LUT_3D_SIZE found on line {line_num+1}. Using first value ({resolution}).")
                         continue
                    resolution = int(parts[1])
                    if resolution <= 1:
                         print(f"Error: LUT_3D_SIZE must be greater than 1, found {resolution} on line {line_num+1}.")
                         return None, None, False
                    lut_data = np.zeros((resolution, resolution, resolution, 3), dtype=np.float32)
                    print(f"Found LUT_3D_SIZE: {resolution}")
                    reading_data = True # Start expecting data points after this line
                    continue # Move to next line

                if keyword == "DOMAIN_MIN" and len(parts) == 4:
                    try:
                        domain_min = [float(p) for p in parts[1:]]
                    except ValueError:
                         print(f"Warning: Invalid DOMAIN_MIN values on line {line_num+1}: '{line}'")
                    continue # Move to next line

                if keyword == "DOMAIN_MAX" and len(parts) == 4:
                    try:
                        domain_max = [float(p) for p in parts[1:]]
                    except ValueError:
                         print(f"Warning: Invalid DOMAIN_MAX values on line {line_num+1}: '{line}'")
                    continue # Move to next line

                # If we encounter data-like lines before LUT_3D_SIZE is set
                if resolution is None and len(parts) == 3:
                     print(f"Warning: Encountered data-like line before LUT_3D_SIZE on line {line_num+1}: '{line}'. Ignoring.")
                     continue

            # --- Parse Data Lines ---
            elif reading_data: # Only read data after LUT_3D_SIZE is found
                if len(parts) == 3:
                    try:
                        entry = [float(p) for p in parts]
                        lut_entries.append(entry)
                    except ValueError:
                        print(f"Warning: Skipping line with non-float data values on line {line_num+1}: '{line}'")
                        continue
                else:
                     print(f"Warning: Unexpected line format in data section on line {line_num+1}: '{line}'. Ignoring.")

        # --- Validation After Reading ---
        if resolution is None:
            print(f"Error loading CUBE LUT from '{lut_file_path}'. 'LUT_3D_SIZE' keyword not found or invalid.")
            return None, None, False

        expected_entries = resolution ** 3
        if len(lut_entries) != expected_entries:
             print(f"Error loading CUBE LUT from '{lut_file_path}'. Expected {expected_entries} data points, found {len(lut_entries)}.")
             return None, None, False

        # --- Populate NumPy Array ---
        index = 0
        # CUBE format specifies B changes fastest, then G, then R
        for b_idx, g_idx, r_idx in product(range(resolution), range(resolution), range(resolution)):
            lut_data[r_idx, g_idx, b_idx] = lut_entries[index]
            index += 1

        print(f"Successfully loaded CUBE LUT: '{title if title else 'Untitled'}' (Res: {resolution}) from '{lut_file_path}'")
        # You might want to return domain_min/max if your C++ code uses them
        return lut_data, resolution, True

    except FileNotFoundError:
        print(f"Error: LUT file not found at '{lut_file_path}'")
        return None, None, False
    except Exception as e:
        print(f"An unexpected error occurred while loading LUT from '{lut_file_path}': {e}")
        # import traceback
        # traceback.print_exc() # Uncomment for detailed debugging
        return None, None, False

# --- Helper ---
def _linear_interpolate_1d_slice(lut_slice_1d, resolution_1d, x):
    # ... (no changes needed inside helper itself)
    if not (0.0 <= x <= 1.0): x = np.clip(x, 0.0, 1.0)
    if resolution_1d <= 1: return lut_slice_1d[0] if len(lut_slice_1d) > 0 else np.array([0.0, 0.0, 0.0], dtype=lut_slice_1d.dtype)
    f_index = x * (resolution_1d - 1)
    idx0 = int(np.floor(f_index)); idx1 = int(np.ceil(f_index))
    idx0 = np.clip(idx0, 0, resolution_1d - 1); idx1 = np.clip(idx1, 0, resolution_1d - 1)
    t = f_index - idx0; val0 = lut_slice_1d[idx0]; val1 = lut_slice_1d[idx1]
    return val0 * (1.0 - t) + val1 * t
# ---

def linear_interpolation(lut_data, resolution, r, g, b, domain_min_np, domain_max_np): # Added domain args
    """1D Linear interpolation with output clamping."""
    # ...(slice selection logic as before)...
    if isinstance(resolution, int):
        res_r, res_g, res_b = resolution, resolution, resolution
    elif len(resolution) == 3: res_r, res_g, res_b = resolution
    else: raise ValueError("Invalid resolution format")
    g_coord = np.clip(g, 0.0, 1.0); b_coord = np.clip(b, 0.0, 1.0)
    g_idx = int(round(g_coord * (res_g - 1))) if res_g > 1 else 0
    b_idx = int(round(b_coord * (res_b - 1))) if res_b > 1 else 0
    g_idx = np.clip(g_idx, 0, res_g - 1); b_idx = np.clip(b_idx, 0, res_b - 1)
    lut_slice_1d = lut_data[:, g_idx, b_idx]

    interpolated_val = _linear_interpolate_1d_slice(lut_slice_1d, res_r, r)

    # Clamp output before returning
    clamped_val = np.clip(interpolated_val, domain_min_np, domain_max_np)
    return clamped_val.tolist()


def bilinear_interpolation(lut_data, resolution, r, g, b, domain_min_np, domain_max_np): # Added domain args
    """2D Bilinear interpolation with output clamping."""
    # ...(slice selection logic as before)...
    if isinstance(resolution, int): res_r, res_g, res_b = resolution, resolution, resolution
    elif len(resolution) == 3: res_r, res_g, res_b = resolution
    else: raise ValueError("Invalid resolution format")
    b_coord = np.clip(b, 0.0, 1.0)
    b_idx = int(round(b_coord * (res_b - 1))) if res_b > 1 else 0
    b_idx = np.clip(b_idx, 0, res_b - 1)
    lut_slice_2d = lut_data[:, :, b_idx]

    x = np.clip(r, 0.0, 1.0); y = np.clip(g, 0.0, 1.0)

    # Handle edge cases for 1D within 2D slice
    if res_r <= 1 and res_g <= 1:
        interpolated_val = lut_slice_2d[0, 0]
    elif res_r <= 1:
        interpolated_val = _linear_interpolate_1d_slice(lut_slice_2d[0, :, :], res_g, y)
    elif res_g <= 1:
        interpolated_val = _linear_interpolate_1d_slice(lut_slice_2d[:, 0, :], res_r, x)
    else: # Standard bilinear interpolation
        fx = x * (res_r - 1); fy = y * (res_g - 1)
        x0 = int(np.floor(fx)); x1 = int(np.ceil(fx))
        y0 = int(np.floor(fy)); y1 = int(np.ceil(fy))
        x0 = np.clip(x0, 0, res_r - 1); x1 = np.clip(x1, 0, res_r - 1)
        y0 = np.clip(y0, 0, res_g - 1); y1 = np.clip(y1, 0, res_g - 1)
        tx = fx - x0; ty = fy - y0
        c00 = lut_slice_2d[x0, y0]; c10 = lut_slice_2d[x1, y0]
        c01 = lut_slice_2d[x0, y1]; c11 = lut_slice_2d[x1, y1]
        c0 = c00 * (1.0 - tx) + c10 * tx; c1 = c01 * (1.0 - tx) + c11 * tx
        interpolated_val = c0 * (1.0 - ty) + c1 * ty

    # Clamp output before returning
    clamped_val = np.clip(interpolated_val, domain_min_np, domain_max_np)
    return clamped_val.tolist()

def nearest_neighbor_interpolation(lut_data, resolution, r, g, b, domain_min_np, domain_max_np): # Added domain args
    """Nearest neighbor interpolation with output clamping."""
    # Resolution here is typically the single int value for cubic LUTs
    if not isinstance(resolution, int):
         print("Warning: Nearest Neighbor expected single resolution value, using first.")
         resolution = resolution[0] # Assume first dim if tuple/list passed

    # ...(input clamping and index calculation as before)...
    r = np.clip(r, 0.0, 1.0); g = np.clip(g, 0.0, 1.0); b = np.clip(b, 0.0, 1.0)
    grid_index_r = int(round(r * (resolution - 1))); grid_index_g = int(round(g * (resolution - 1))); grid_index_b = int(round(b * (resolution - 1)))
    grid_index_r = np.clip(grid_index_r, 0, resolution - 1); grid_index_g = np.clip(grid_index_g, 0, resolution - 1); grid_index_b = np.clip(grid_index_b, 0, resolution - 1)

    result_val = lut_data[grid_index_r, grid_index_g, grid_index_b]

    # Clamp output before returning
    clamped_val = np.clip(result_val, domain_min_np, domain_max_np)
    return clamped_val.tolist()


def trilinear_interpolation(lut_data, resolution, r, g, b, domain_min_np, domain_max_np): # Added domain args
    """Trilinear interpolation with output clamping."""
    if not isinstance(resolution, int):
         print("Warning: Trilinear expected single resolution value, using first.")
         resolution = resolution[0] # Assume first dim if tuple/list passed

    # ...(input clamping and interpolation calculation as before)...
    r = np.clip(r, 0.0, 1.0); g = np.clip(g, 0.0, 1.0); b = np.clip(b, 0.0, 1.0)
    f = float(resolution - 1); x, y, z = r * f, g * f, b * f
    i, j, k = int(x), int(y), int(z)
    fx, fy, fz = x - i, y - j, z - k
    x0 = np.clip(i, 0, resolution - 1); x1 = np.clip(i + 1, 0, resolution - 1)
    y0 = np.clip(j, 0, resolution - 1); y1 = np.clip(j + 1, 0, resolution - 1)
    z0 = np.clip(k, 0, resolution - 1); z1 = np.clip(k + 1, 0, resolution - 1)
    c000 = lut_data[x0, y0, z0]; c100 = lut_data[x1, y0, z0]
    c010 = lut_data[x0, y1, z0]; c110 = lut_data[x1, y1, z0]
    c001 = lut_data[x0, y0, z1]; c101 = lut_data[x1, y0, z1]
    c011 = lut_data[x0, y1, z1]; c111 = lut_data[x1, y1, z1]
    c00 = c000 * (1 - fx) + c100 * fx; c01 = c001 * (1 - fx) + c101 * fx
    c10 = c010 * (1 - fx) + c110 * fx; c11 = c011 * (1 - fx) + c111 * fx
    c0 = c00 * (1 - fy) + c10 * fy; c1 = c01 * (1 - fy) + c11 * fy
    interpolated_val = c0 * (1 - fz) + c1 * fz

    # Clamp output before returning
    clamped_val = np.clip(interpolated_val, domain_min_np, domain_max_np)
    return clamped_val.tolist()

def tetrahedral_interpolation(lut_data, resolution, r, g, b, domain_min_np, domain_max_np): # Added domain args
    """Tetrahedral interpolation with output clamping."""
    if not isinstance(resolution, int):
         print("Warning: Tetrahedral expected single resolution value, using first.")
         resolution = resolution[0] # Assume first dim if tuple/list passed

    # ...(input clamping and interpolation calculation as before)...
    r = np.clip(r, 0.0, 1.0); g = np.clip(g, 0.0, 1.0); b = np.clip(b, 0.0, 1.0)
    f = float(resolution - 1); x, y, z = r * f, g * f, b * f
    i, j, k = int(x), int(y), int(z)
    fx, fy, fz = x - i, y - j, z - k
    x0 = np.clip(i, 0, resolution - 1); x1 = np.clip(i + 1, 0, resolution - 1)
    y0 = np.clip(j, 0, resolution - 1); y1 = np.clip(j + 1, 0, resolution - 1)
    z0 = np.clip(k, 0, resolution - 1); z1 = np.clip(k + 1, 0, resolution - 1)
    v = np.zeros((8, 3), dtype=lut_data.dtype)
    v[0]=lut_data[x0, y0, z0]; v[1]=lut_data[x1, y0, z0]; v[2]=lut_data[x0, y1, z0]
    v[3]=lut_data[x0, y0, z1]; v[4]=lut_data[x1, y1, z0]; v[5]=lut_data[x1, y0, z1]
    v[6]=lut_data[x0, y1, z1]; v[7]=lut_data[x1, y1, z1]

    # Determine tetrahedron and interpolate (example logic from before)
    if fx >= fy and fy >= fz: c0 = v[0]; c1 = v[1] - v[0]; c2 = v[2] - v[0]; c3 = v[4] - v[1] - v[2] + v[0]; result = c0 + c1*fx + c2*fy + c3*fz # T1: v0124
    elif fx >= fz and fz >= fy: c0 = v[0]; c1 = v[1] - v[0]; c2 = v[3] - v[0]; c3 = v[5] - v[1] - v[3] + v[0]; result = c0 + c1*fx + c2*fz + c3*fy # T2: v0135
    elif fz >= fy and fy >= fx: c0 = v[0]; c1 = v[3] - v[0]; c2 = v[2] - v[0]; c3 = v[6] - v[3] - v[2] + v[0]; result = c0 + c1*fz + c2*fy + c3*fx # T3: v0326
    elif fy >= fx and fx >= fz: c0 = v[7]; c1 = v[6] - v[7]; c2 = v[4] - v[7]; c3 = v[2] - v[6] - v[4] + v[7]; result = c0 + c1*(1-fx) + c2*(1-fy) + c3*(1-fz) # T4: v7642
    elif fz >= fx and fx >= fy: c0 = v[7]; c1 = v[6] - v[7]; c2 = v[5] - v[7]; c3 = v[3] - v[6] - v[5] + v[7]; result = c0 + c1*(1-fx) + c2*(1-fz) + c3*(1-fy) # T5: v7653
    else: c0 = v[7]; c1 = v[4] - v[7]; c2 = v[5] - v[7]; c3 = v[1] - v[4] - v[5] + v[7]; result = c0 + c1*(1-fy) + c2*(1-fz) + c3*(1-fx) # T6: v7451

    # Clamp output before returning
    clamped_val = np.clip(result, domain_min_np, domain_max_np)
    return clamped_val.tolist()


# --- Helper function for Cubic/Bicubic Kernels ---
# This is a common cubic convolution kernel often used for "cubic" interpolation
# (parameter 'a' is often -0.5 for Catmull-Rom like behavior)
def _cubic_kernel(x, a=-0.5):
    """Calculates the weight using a cubic convolution kernel."""
    ax = abs(x)
    if ax <= 1.0:
        return (a + 2.0) * ax**3 - (a + 3.0) * ax**2 + 1.0
    elif ax < 2.0:
        return a * ax**3 - 5.0 * a * ax**2 + 8.0 * a * ax - 4.0 * a
    else:
        return 0.0

# --- Barycentric Tetrahedral Interpolation ---
# Note: The logic here is functionally the same as the previous `tetrahedral_interpolation`
# provided, as that implementation inherently uses barycentric weights within each determined
# tetrahedron. No significant logical change needed if the previous one worked.
# We keep the name change for clarity as requested.
def barycentric_tetrahedral_interpolation(lut_data, resolution, r, g, b, domain_min_np, domain_max_np):
    """Performs Tetrahedral interpolation (effectively Barycentric within sub-tetrahedra)
       with output clamping."""
    # --- Same core logic as the previously provided tetrahedral_interpolation ---
    if not isinstance(resolution, int):
         print("Warning: Barycentric Tetrahedral expected single resolution value, using first.")
         resolution = resolution[0] # Assume first dim if tuple/list passed

    # Clamp input coordinates
    r_clamp = np.clip(r, 0.0, 1.0)
    g_clamp = np.clip(g, 0.0, 1.0)
    b_clamp = np.clip(b, 0.0, 1.0)

    f = float(resolution - 1)
    x, y, z = r_clamp * f, g_clamp * f, b_clamp * f
    i, j, k = int(x), int(y), int(z)
    fx, fy, fz = x - i, y - j, z - k # Fractional parts

    # Ensure base indices are within bounds
    x0_base = np.clip(i, 0, resolution - 1)
    y0_base = np.clip(j, 0, resolution - 1)
    z0_base = np.clip(k, 0, resolution - 1)

    # Fetch the 8 vertices of the surrounding cube, handling boundaries
    v = np.zeros((8, 3), dtype=lut_data.dtype)
    indices = [
        (np.clip(x0_base + dx, 0, resolution-1),
         np.clip(y0_base + dy, 0, resolution-1),
         np.clip(z0_base + dz, 0, resolution-1))
        for dz in [0, 1] for dy in [0, 1] for dx in [0, 1]
    ]
    # Standard vertex order: 000, 100, 010, 001, 110, 101, 011, 111 relative to (i,j,k) floor
    # Reorder indices fetch to match v[0..7] used in logic below
    v[0] = lut_data[indices[0]] # 000
    v[1] = lut_data[indices[1]] # 100
    v[2] = lut_data[indices[2]] # 010
    v[3] = lut_data[indices[4]] # 001
    v[4] = lut_data[indices[3]] # 110
    v[5] = lut_data[indices[5]] # 101
    v[6] = lut_data[indices[6]] # 011
    v[7] = lut_data[indices[7]] # 111


    # Determine tetrahedron and interpolate (same logic as before)
    # This implicitly uses barycentric coordinates for the linear interpolation within the tetra
    if fx >= fy and fy >= fz: c0 = v[0]; c1 = v[1] - v[0]; c2 = v[2] - v[0]; c3 = v[4] - v[1] - v[2] + v[0]; result = c0 + c1*fx + c2*fy + c3*fz # T1: v0124
    elif fx >= fz and fz >= fy: c0 = v[0]; c1 = v[1] - v[0]; c2 = v[3] - v[0]; c3 = v[5] - v[1] - v[3] + v[0]; result = c0 + c1*fx + c2*fz + c3*fy # T2: v0135
    elif fz >= fy and fy >= fx: c0 = v[0]; c1 = v[3] - v[0]; c2 = v[2] - v[0]; c3 = v[6] - v[3] - v[2] + v[0]; result = c0 + c1*fz + c2*fy + c3*fx # T3: v0326
    elif fy >= fx and fx >= fz: c0 = v[7]; c1 = v[6] - v[7]; c2 = v[4] - v[7]; c3 = v[2] - v[6] - v[4] + v[7]; result = c0 + c1*(1-fx) + c2*(1-fy) + c3*(1-fz) # T4: v7642
    elif fz >= fx and fx >= fy: c0 = v[7]; c1 = v[6] - v[7]; c2 = v[5] - v[7]; c3 = v[3] - v[6] - v[5] + v[7]; result = c0 + c1*(1-fx) + c2*(1-fz) + c3*(1-fy) # T5: v7653
    else: c0 = v[7]; c1 = v[4] - v[7]; c2 = v[5] - v[7]; c3 = v[1] - v[4] - v[5] + v[7]; result = c0 + c1*(1-fy) + c2*(1-fz) + c3*(1-fx) # T6: v7451

    # Clamp output before returning
    clamped_val = np.clip(result, domain_min_np, domain_max_np)
    return clamped_val.tolist()


# --- Bicubic Interpolation (applied to 2D slice) ---
def bicubic_interpolation(lut_data, resolution, r, g, b, domain_min_np, domain_max_np):
    """
    Performs 2D Bicubic interpolation on the R-G plane of the 3D LUT,
    using 'r' and 'g' coordinates. The R-G plane is selected based on
    the nearest grid point to the input 'b' coordinate. Output is clamped.
    Uses a cubic convolution kernel.
    """
    if isinstance(resolution, int):
        res_r, res_g, res_b = resolution, resolution, resolution
    elif len(resolution) == 3:
        res_r, res_g, res_b = resolution
    else:
        raise ValueError("Resolution must be an int or a list/tuple of 3 ints")

    # --- Slice Selection ---
    b_coord = np.clip(b, 0.0, 1.0)
    b_idx = int(round(b_coord * (res_b - 1))) if res_b > 1 else 0
    b_idx = np.clip(b_idx, 0, res_b - 1)
    lut_slice_2d = lut_data[:, :, b_idx] # Shape: (res_r, res_g, 3)

    # --- Bicubic Interpolation on the Slice ---
    x = np.clip(r, 0.0, 1.0)
    y = np.clip(g, 0.0, 1.0)

    # Handle edge cases for dimensions being too small for 4x4 grid
    if res_r < 4 or res_g < 4:
        print(f"Warning: Bicubic requires resolution >= 4 for slice dims, have ({res_r}, {res_g}). Falling back to Bilinear.")
        # Fallback to the bilinear function you already have for this slice
        # Need to pass the slice and its resolution
        # Make sure the bilinear function exists and has the expected signature for a 2D slice
        from_bilinear = bilinear_interpolation(lut_data, resolution, r, g, b, domain_min_np, domain_max_np) # Recalculates slice, suboptimal
        return from_bilinear # Bilinear already clamps

    fx = x * (res_r - 1)
    fy = y * (res_g - 1)
    ix = int(np.floor(fx))
    iy = int(np.floor(fy))
    tx = fx - ix
    ty = fy - iy

    # Get the 4x4 neighboring grid points - Careful with boundaries!
    points = np.zeros((4, 4, 3), dtype=lut_slice_2d.dtype)
    for j in range(4):
        for i in range(4):
            px = np.clip(ix + i - 1, 0, res_r - 1) # Index for the 4x4 grid (-1 to +2)
            py = np.clip(iy + j - 1, 0, res_g - 1) # Index for the 4x4 grid (-1 to +2)
            points[i, j] = lut_slice_2d[px, py]

    # Interpolate along x-axis (R dimension) for each of the 4 rows
    interp_rows = np.zeros((4, 3), dtype=lut_slice_2d.dtype)
    for j in range(4):
        w0 = _cubic_kernel(tx + 1.0) # Weight for point at ix-1
        w1 = _cubic_kernel(tx)       # Weight for point at ix
        w2 = _cubic_kernel(tx - 1.0) # Weight for point at ix+1
        w3 = _cubic_kernel(tx - 2.0) # Weight for point at ix+2
        interp_rows[j] = points[0, j] * w0 + points[1, j] * w1 + points[2, j] * w2 + points[3, j] * w3

    # Interpolate along y-axis (G dimension) using the 4 row results
    w0 = _cubic_kernel(ty + 1.0) # Weight for row at iy-1
    w1 = _cubic_kernel(ty)       # Weight for row at iy
    w2 = _cubic_kernel(ty - 1.0) # Weight for row at iy+1
    w3 = _cubic_kernel(ty - 2.0) # Weight for row at iy+2
    interpolated_val = interp_rows[0] * w0 + interp_rows[1] * w1 + interp_rows[2] * w2 + interp_rows[3] * w3

    # Clamp output before returning
    clamped_val = np.clip(interpolated_val, domain_min_np, domain_max_np)
    return clamped_val.tolist()


# --- 1D Cubic Spline Interpolation (using cubic convolution kernel) ---
def cubic_spline_interpolation(lut_data, resolution, r, g, b, domain_min_np, domain_max_np):
    """
    Performs 1D Cubic interpolation along the R-axis of the 3D LUT,
    using the 'r' coordinate and a cubic convolution kernel. The specific R-axis
    slice is chosen based on the nearest grid point to 'g' and 'b'. Output is clamped.
    """
    if isinstance(resolution, int):
        res_r, res_g, res_b = resolution, resolution, resolution
    elif len(resolution) == 3:
        res_r, res_g, res_b = resolution
    else:
        raise ValueError("Resolution must be an int or a list/tuple of 3 ints")

    # --- Slice Selection ---
    g_coord = np.clip(g, 0.0, 1.0)
    b_coord = np.clip(b, 0.0, 1.0)
    g_idx = int(round(g_coord * (res_g - 1))) if res_g > 1 else 0
    b_idx = int(round(b_coord * (res_b - 1))) if res_b > 1 else 0
    g_idx = np.clip(g_idx, 0, res_g - 1)
    b_idx = np.clip(b_idx, 0, res_b - 1)
    lut_slice_1d = lut_data[:, g_idx, b_idx] # Shape: (res_r, 3)

    # --- 1D Cubic Interpolation on the Slice ---
    x = np.clip(r, 0.0, 1.0)

    # Handle edge cases for dimensions being too small for 4 points
    if res_r < 4:
         print(f"Warning: Cubic interpolation requires resolution >= 4, have ({res_r}). Falling back to Linear.")
         # Fallback to the linear function you already have for this slice
         # Need to pass the slice and its resolution
         from_linear = _linear_interpolate_1d_slice(lut_slice_1d, res_r, x) # Linear helper clamps input
         clamped_val = np.clip(from_linear, domain_min_np, domain_max_np) # Still need output clamp
         return clamped_val.tolist()


    fx = x * (res_r - 1)
    ix = int(np.floor(fx))
    tx = fx - ix # Fractional part

    # Get the 4 neighboring grid points - Careful with boundaries!
    points = np.zeros((4, 3), dtype=lut_slice_1d.dtype)
    for i in range(4):
        px = np.clip(ix + i - 1, 0, res_r - 1) # Index for the 4 points (-1 to +2 relative to ix)
        points[i] = lut_slice_1d[px]

    # Calculate weights using the cubic kernel
    w0 = _cubic_kernel(tx + 1.0) # Weight for point at ix-1
    w1 = _cubic_kernel(tx)       # Weight for point at ix
    w2 = _cubic_kernel(tx - 1.0) # Weight for point at ix+1
    w3 = _cubic_kernel(tx - 2.0) # Weight for point at ix+2

    # Perform the weighted sum
    interpolated_val = points[0] * w0 + points[1] * w1 + points[2] * w2 + points[3] * w3

    # Clamp output before returning
    clamped_val = np.clip(interpolated_val, domain_min_np, domain_max_np)
    return clamped_val.tolist()


def tricubic_interpolation(lut_data, resolution, r, g, b, domain_min_np, domain_max_np):
    """
    Performs 3D Tricubic interpolation on the LUT using a cubic convolution kernel.
    Output is clamped to the domain bounds.

    Args:
        lut_data (numpy.ndarray): The 3D LUT data (res_r x res_g x res_b x 3).
        resolution (int or list/tuple): The resolution (can be single int if cube, or list [res_r, res_g, res_b]).
        r (float): Normalized R coordinate (0.0 to 1.0).
        g (float): Normalized G coordinate (0.0 to 1.0).
        b (float): Normalized B coordinate (0.0 to 1.0).
        domain_min_np (numpy.ndarray): Lower bounds for output clamping ([min_r, min_g, min_b]).
        domain_max_np (numpy.ndarray): Upper bounds for output clamping ([max_r, max_g, max_b]).

    Returns:
        list: The interpolated RGB value.
    """
    if isinstance(resolution, int):
        res_r, res_g, res_b = resolution, resolution, resolution
    elif len(resolution) == 3:
        res_r, res_g, res_b = resolution
    else:
        raise ValueError("Resolution must be an int or a list/tuple of 3 ints")

    # Clamp input coordinates
    x = np.clip(r, 0.0, 1.0)
    y = np.clip(g, 0.0, 1.0)
    z = np.clip(b, 0.0, 1.0)

    # --- Fallback for small resolutions ---
    if res_r < 4 or res_g < 4 or res_b < 4:
        print(f"Warning: Tricubic requires resolution >= 4 in all dims, have ({res_r}, {res_g}, {res_b}). Falling back to Trilinear.")
        # Fallback to the trilinear function (ensure it exists and has the correct signature)
        # Assuming trilinear function clamps output internally
        from_trilinear = trilinear_interpolation(lut_data, resolution, r, g, b, domain_min_np, domain_max_np)
        return from_trilinear # Trilinear should clamp

    # Calculate floating point indices and integer base indices
    fx = x * (res_r - 1)
    fy = y * (res_g - 1)
    fz = z * (res_b - 1)

    ix = int(np.floor(fx))
    iy = int(np.floor(fy))
    iz = int(np.floor(fz))

    # Calculate fractional parts
    tx = fx - ix
    ty = fy - iy
    tz = fz - iz

    # Get the 4x4x4 neighborhood grid points - Ensure boundaries are handled
    points_4x4x4 = np.zeros((4, 4, 4, 3), dtype=lut_data.dtype)
    for k in range(4):
        for j in range(4):
            for i in range(4):
                px = np.clip(ix + i - 1, 0, res_r - 1) # Index for the 4 points along X
                py = np.clip(iy + j - 1, 0, res_g - 1) # Index for the 4 points along Y
                pz = np.clip(iz + k - 1, 0, res_b - 1) # Index for the 4 points along Z
                points_4x4x4[i, j, k] = lut_data[px, py, pz]

    # Interpolate along X-axis to get a 1x4x4 grid
    interp_y_z = np.zeros((4, 4, 3), dtype=lut_data.dtype)
    wx = np.array([_cubic_kernel(tx + 1.0), _cubic_kernel(tx), _cubic_kernel(tx - 1.0), _cubic_kernel(tx - 2.0)])
    for k in range(4):
        for j in range(4):
            interp_y_z[j, k] = np.dot(wx, points_4x4x4[:, j, k]) # Efficiently sum weighted points along X

    # Interpolate along Y-axis to get a 1x1x4 grid (effectively 4 points along Z)
    interp_z = np.zeros((4, 3), dtype=lut_data.dtype)
    wy = np.array([_cubic_kernel(ty + 1.0), _cubic_kernel(ty), _cubic_kernel(ty - 1.0), _cubic_kernel(ty - 2.0)])
    for k in range(4):
        interp_z[k] = np.dot(wy, interp_y_z[:, k]) # Efficiently sum weighted points along Y

    # Interpolate along Z-axis to get the final 1x1x1 value
    wz = np.array([_cubic_kernel(tz + 1.0), _cubic_kernel(tz), _cubic_kernel(tz - 1.0), _cubic_kernel(tz - 2.0)])
    interpolated_val = np.dot(wz, interp_z) # Efficiently sum weighted points along Z

    # Clamp output before returning
    clamped_val = np.clip(interpolated_val, domain_min_np, domain_max_np)
    return clamped_val.tolist()


# --- Helper Function for Quintic Kernel ---
# A simplified conceptual quintic kernel based on extending the idea.
# NOTE: Deriving and using *true* C² continuous quintic splines is more complex.
# This aims for a plausible shape for demonstration. Needs support [-3, 3] for 6x6x6.
# (For simplicity, let's just use the _cubic_kernel for demo - a true Quintic adds much complexity)
# *** Correction: Let's define a placeholder quintic or explicitly fallback to cubic
# *** For simplicity in this example, we will reuse the _cubic_kernel logic
# *** and just use a larger 6x6x6 grid, effectively creating a different type
# *** of high-order interpolation but not strictly "Tri-Quintic" in the pure mathematical sense.
# *** A true quintic implementation is significantly more involved.
# *** If you need true quintic, a library like SciPy's RegularGridInterpolator might be needed.
# *** UPDATE: To make it *demonstrate* the 6x6x6 idea, we *will* implement it using
# *** the existing cubic kernel over the larger grid. It's not technically "Tri-Quintic"
# *** but shows the principle of higher-order sequential application.

def _get_interpolation_weights(kernel_func, fractional_pos, num_points):
    """Helper to get interpolation weights based on a kernel."""
    weights = np.zeros(num_points)
    offset = -(num_points // 2 - 1) # Center the kernel relative to floor index
    for i in range(num_points):
        weights[i] = kernel_func(fractional_pos + offset - i + 1) # Shift kernel correctly
        # Example for 4 points (Cubic):
        # w0 = kernel_func(tx + 1.0)  => i=0, offset=-1; kernel(tx - 1 - 0 + 1) = kernel(tx) INCORRECT
        # Correct kernel positions relative to fractional coordinate `t`:
        # indices relative to floor(fx): -1, 0, 1, 2
        # kernel inputs should be: t - (-1), t - 0, t - 1, t - 2 => t+1, t, t-1, t-2
    if num_points == 4: # Adjust kernel input for standard 4-point cubic
        weights = np.array([kernel_func(fractional_pos + 1.0), kernel_func(fractional_pos),
                            kernel_func(fractional_pos - 1.0), kernel_func(fractional_pos - 2.0)])
    elif num_points == 6: # Adjust kernel input for conceptual 6-point method
        # Using the _cubic_kernel for this 6-point demo - not true quintic
         weights = np.array([kernel_func(fractional_pos + 2.0), kernel_func(fractional_pos + 1.0),
                             kernel_func(fractional_pos), kernel_func(fractional_pos - 1.0),
                             kernel_func(fractional_pos - 2.0), kernel_func(fractional_pos - 3.0)])
    else:
        raise ValueError("Unsupported number of points for weights")
    return weights

# --- "Conceptual" Tri-Quintic (using Cubic Kernel over 6x6x6 Grid) ---
def conceptual_triquintic_interpolation(lut_data, resolution, r, g, b, domain_min_np, domain_max_np):
    """
    Performs 3D interpolation using a 6x6x6 neighborhood and the _cubic_kernel
    applied sequentially. This demonstrates higher-order principles but isn't a
    true mathematical Tri-Quintic spline. Requires resolution >= 6. Output is clamped.

    NOTE: Falls back to Tricubic if resolution is too small.
    """
    kernel_func = _cubic_kernel # Using the existing cubic kernel for this demo
    kernel_size = 6 # 6x6x6 neighborhood

    if isinstance(resolution, int):
        res_r, res_g, res_b = resolution, resolution, resolution
    elif len(resolution) == 3:
        res_r, res_g, res_b = resolution
    else:
        raise ValueError("Resolution must be an int or a list/tuple of 3 ints")

    # Clamp input coordinates
    x = np.clip(r, 0.0, 1.0)
    y = np.clip(g, 0.0, 1.0)
    z = np.clip(b, 0.0, 1.0)

    # --- Fallback for small resolutions ---
    if res_r < kernel_size or res_g < kernel_size or res_b < kernel_size:
        print(f"Warning: Conceptual {kernel_size}x{kernel_size}x{kernel_size} requires res >= {kernel_size}. Falling back to Tricubic.")
        # Fallback to the tricubic function
        from_tricubic = tricubic_interpolation(lut_data, resolution, r, g, b, domain_min_np, domain_max_np)
        return from_tricubic # Assume tricubic clamps

    # Calculate floating point indices and integer base indices
    fx = x * (res_r - 1)
    fy = y * (res_g - 1)
    fz = z * (res_b - 1)
    ix = int(np.floor(fx))
    iy = int(np.floor(fy))
    iz = int(np.floor(fz))
    tx = fx - ix # Fractional parts
    ty = fy - iy
    tz = fz - iz

    # Get the KxKxK neighborhood grid points (K=kernel_size)
    grid_offset = -(kernel_size // 2 - 1) # e.g., -2 for K=6; indices -2 to +3
    points_KxKxK = np.zeros((kernel_size, kernel_size, kernel_size, 3), dtype=lut_data.dtype)
    for k_i in range(kernel_size):
        for j_i in range(kernel_size):
            for i_i in range(kernel_size):
                px = np.clip(ix + i_i + grid_offset, 0, res_r - 1)
                py = np.clip(iy + j_i + grid_offset, 0, res_g - 1)
                pz = np.clip(iz + k_i + grid_offset, 0, res_b - 1)
                points_KxKxK[i_i, j_i, k_i] = lut_data[px, py, pz]

    # Calculate interpolation weights
    wx = _get_interpolation_weights(kernel_func, tx, kernel_size)
    wy = _get_interpolation_weights(kernel_func, ty, kernel_size)
    wz = _get_interpolation_weights(kernel_func, tz, kernel_size)

    # Interpolate along X-axis -> KxK grid
    interp_y_z = np.zeros((kernel_size, kernel_size, 3), dtype=lut_data.dtype)
    for k in range(kernel_size):
        for j in range(kernel_size):
            interp_y_z[j, k] = np.dot(wx, points_KxKxK[:, j, k])

    # Interpolate along Y-axis -> K vector along Z
    interp_z = np.zeros((kernel_size, 3), dtype=lut_data.dtype)
    for k in range(kernel_size):
        interp_z[k] = np.dot(wy, interp_y_z[:, k])

    # Interpolate along Z-axis -> final value
    interpolated_val = np.dot(wz, interp_z)

    # Clamp output before returning
    clamped_val = np.clip(interpolated_val, domain_min_np, domain_max_np)
    return clamped_val.tolist()


# --- RBF Interpolation (Gaussian Kernel - VERY SLOW DEMO) ---
def _gaussian_rbf(r, sigma):
    """Gaussian RBF kernel."""
    return np.exp(- (r**2) / (2 * sigma**2))

def rbf_interpolation(lut_data, resolution, r, g, b, domain_min_np, domain_max_np):
    """
    Performs 3D RBF interpolation using a Gaussian kernel.
    WARNING: Extremely slow for non-trivial LUT sizes as it recalculates
             weights for every call. Provided for mathematical interest only.
             Practical use requires pre-calculation of weights. Output is clamped.
    """
    print("Warning: RBF interpolation called. This will be very slow!")

    if isinstance(resolution, int):
        res_r, res_g, res_b = resolution, resolution, resolution
    elif len(resolution) == 3:
        res_r, res_g, res_b = resolution
    else:
        raise ValueError("Resolution must be an int or a list/tuple of 3 ints")

    num_points_total = res_r * res_g * res_b

    if num_points_total > 1000: # Arbitrary limit to prevent hanging
        print(f"Warning: RBF LUT size ({num_points_total} points) too large for demo. Skipping calculation.")
        # Return a fallback or indicate failure
        return [domain_min_np[0], domain_min_np[1], domain_min_np[2]] # e.g., return min domain value


    # 1. Create coordinates of all LUT grid points (centers)
    r_coords = np.linspace(0.0, 1.0, res_r)
    g_coords = np.linspace(0.0, 1.0, res_g)
    b_coords = np.linspace(0.0, 1.0, res_b)
    centers_list = list(product(r_coords, g_coords, b_coords))
    centers = np.array(centers_list) # Shape: (N, 3)

    # 2. Get the target values at the centers
    # Reshape LUT data to match centers: (N, 3)
    # Note: Be careful with reshape order if using different indexing!
    # Assuming lut_data[r_idx, g_idx, b_idx] corresponds to centers order from product
    target_values = lut_data.reshape(num_points_total, 3)

    # 3. Choose RBF kernel and parameter (sigma)
    kernel = _gaussian_rbf
    # Estimate sigma based on average grid spacing (heuristic)
    avg_spacing_r = 1.0 / (res_r - 1) if res_r > 1 else 1.0
    avg_spacing_g = 1.0 / (res_g - 1) if res_g > 1 else 1.0
    avg_spacing_b = 1.0 / (res_b - 1) if res_b > 1 else 1.0
    sigma = (avg_spacing_r + avg_spacing_g + avg_spacing_b) / 3.0 # Example sigma

    # 4. Calculate the interpolation matrix A
    # Calculate pairwise distances between all centers
    # Broadcasting can be complex here, using loops for clarity in demo:
    A = np.zeros((num_points_total, num_points_total))
    for i in range(num_points_total):
        for j in range(num_points_total):
            dist = np.linalg.norm(centers[i] - centers[j])
            A[i, j] = kernel(dist, sigma)

    # 5. Solve for the weights w (A * w = f)
    try:
        # Solve for each color channel independently or as a block
        weights = np.linalg.solve(A, target_values) # weights shape: (N, 3)
    except np.linalg.LinAlgError:
        print("Error: RBF matrix is singular. Cannot solve for weights. Check sigma or RBF type.")
        # Fallback (e.g., trilinear) or return default
        fallback_res = trilinear_interpolation(lut_data, resolution, r, g, b, domain_min_np, domain_max_np)
        return fallback_res

    # 6. Interpolate the input point (r, g, b)
    input_point = np.array([r, g, b])
    phi_values = np.zeros(num_points_total)
    for i in range(num_points_total):
        dist = np.linalg.norm(input_point - centers[i])
        phi_values[i] = kernel(dist, sigma)

    # Calculate final value: sum(weights * phi_values) for each channel
    interpolated_val = np.dot(phi_values, weights) # Result shape (3,)

    # 7. Clamp output
    clamped_val = np.clip(interpolated_val, domain_min_np, domain_max_np)
    return clamped_val.tolist()


# Assuming _gaussian_rbf kernel function is defined as before:
# def _gaussian_rbf(r, sigma):
#     """Gaussian RBF kernel."""
#     return np.exp(- (r**2) / (2 * sigma**2))

# --- RBF Interpolation (Gaussian Kernel - UNPROTECTED, EXTREMELY SLOW) ---
def rbf_interpolation_unprotected(lut_data, resolution, r, g, b, domain_min_np, domain_max_np):
    """
    Performs 3D RBF interpolation using a Gaussian kernel.
    *** WARNING: UNPROTECTED VERSION! ***
    This version REMOVES the size check. It WILL attempt the full calculation
    regardless of LUT size. This is extremely computationally expensive and
    memory-intensive (O(N^2) memory, O(N^3) time where N=res^3) and is
    highly likely to fail (crash, hang, take days) for LUTs larger than ~8x8x8.
    Provided for mathematical interest ONLY. Use with extreme caution.
    """
    print("\n!!! WARNING: Running UNPROTECTED RBF interpolation !!!")
    print("    This may take an extremely long time or exhaust system memory.")

    if isinstance(resolution, int):
        res_r, res_g, res_b = resolution, resolution, resolution
    elif len(resolution) == 3:
        res_r, res_g, res_b = resolution
    else:
        raise ValueError("Resolution must be an int or a list/tuple of 3 ints")

    num_points_total = res_r * res_g * res_b
    print(f"    LUT dimensions: {res_r}x{res_g}x{res_b} ({num_points_total} points)")

    # 1. Create coordinates of all LUT grid points (centers)
    print("    Step 1/7: Creating center coordinates...")
    r_coords = np.linspace(0.0, 1.0, res_r)
    g_coords = np.linspace(0.0, 1.0, res_g)
    b_coords = np.linspace(0.0, 1.0, res_b)
    # Using product ensures correct ordering matching reshape later
    centers_list = list(product(r_coords, g_coords, b_coords))
    centers = np.array(centers_list, dtype=lut_data.dtype) # Shape: (N, 3)

    # 2. Get the target values at the centers
    print("    Step 2/7: Reshaping target LUT values...")
    # Ensure reshape order matches center creation (C order usually default)
    target_values = lut_data.reshape(num_points_total, 3)

    # 3. Choose RBF kernel and parameter (sigma)
    print("    Step 3/7: Selecting RBF kernel and parameters...")
    kernel = _gaussian_rbf
    avg_spacing_r = 1.0 / (res_r - 1) if res_r > 1 else 1.0
    avg_spacing_g = 1.0 / (res_g - 1) if res_g > 1 else 1.0
    avg_spacing_b = 1.0 / (res_b - 1) if res_b > 1 else 1.0
    sigma = (avg_spacing_r + avg_spacing_g + avg_spacing_b) / 3.0
    print(f"        Using Gaussian kernel with estimated sigma: {sigma:.4f}")

    # 4. Calculate the interpolation matrix A
    print(f"    Step 4/7: Calculating interpolation matrix A ({num_points_total}x{num_points_total})...")
    # Pre-calculate squared distances for efficiency
    print("        Calculating pairwise distances...")
    centers_sq_sum = np.sum(centers**2, axis=1)
    # Dist^2 = ||a-b||^2 = ||a||^2 + ||b||^2 - 2*a.b
    dist_sq = centers_sq_sum[:, np.newaxis] + centers_sq_sum[np.newaxis, :] - 2 * np.dot(centers, centers.T)
    dist_sq = np.maximum(dist_sq, 0) # Prevent small negatives from precision issues
    distances = np.sqrt(dist_sq)
    print("        Applying RBF kernel to distances...")
    try:
        A = kernel(distances, sigma)
    except MemoryError:
         print("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!")
         print("MEMORY ERROR: Failed to allocate matrix A.")
         print("LUT size is too large for available memory.")
         print("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!")
         # Cannot proceed, return a default value
         return [domain_min_np[0], domain_min_np[1], domain_min_np[2]]


    print(f"    Step 5/7: Solving linear system (A * w = f) for weights ({num_points_total} equations)...")
    weights = np.zeros_like(target_values) # Initialize weights array
    start_solve_time = time.time()
    try:
        # Solve for weights (can take extremely long)
        weights = np.linalg.solve(A, target_values) # weights shape: (N, 3)
        end_solve_time = time.time()
        print(f"        System solve took: {end_solve_time - start_solve_time:.2f} seconds.")
    except np.linalg.LinAlgError:
        print("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!")
        print("ERROR: RBF matrix is singular or ill-conditioned.")
        print("Cannot solve for weights. Check sigma or RBF type.")
        print("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!")
        # Cannot proceed, return default
        return [domain_min_np[0], domain_min_np[1], domain_min_np[2]]
    except MemoryError:
        print("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!")
        print("MEMORY ERROR: Failed during linear system solve.")
        print("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!")
        return [domain_min_np[0], domain_min_np[1], domain_min_np[2]]


    print("    Step 6/7: Interpolating input point...")
    input_point = np.array([r, g, b])
    phi_values = np.zeros(num_points_total)
    # Calculate distance from input point to all centers
    input_dist = np.linalg.norm(input_point - centers, axis=1)
    phi_values = kernel(input_dist, sigma)

    # Calculate final value: sum(weights * phi_values) for each channel
    interpolated_val = np.dot(phi_values, weights) # Result shape (3,)

    print("    Step 7/7: Clamping output...")
    # Clamp output
    clamped_val = np.clip(interpolated_val, domain_min_np, domain_max_np)
    print("--- RBF UNPROTECTED calculation finished. ---")
    return clamped_val.tolist()




if __name__ == "__main__":
    # --- Setup Argument Parser ---
    parser = argparse.ArgumentParser(
        description='Load a 3D CUBE LUT file and perform various interpolations.',
        formatter_class=argparse.ArgumentDefaultsHelpFormatter # Optional: improves help message
    )
    # Add an argument for the LUT file path. It's a 'positional' argument, meaning it's required.
    parser.add_argument(
        'lut_file_path',             # Name of the argument variable (args.lut_file_path)
        type=str,                    # Expected data type
        help='Path to the 3D CUBE LUT file to process.' # Help text displayed with -h
    )

    domain_min = 0.0;
    domain_max = 1.0;

    # --- Parse Arguments ---
    args = parser.parse_args() # Read arguments from the command line

    # --- Use the Parsed Argument ---
    lut_file = args.lut_file_path  # Get the file path provided by the user

    # Print the filename being used (as requested)
    print(f"--- Using LUT file: {lut_file} ---")

    # --- Load the LUT ---
    lut_data, resolution, loaded_successfully = load_cube_lut(lut_file)

    # --- Proceed if LUT loaded ---
    if loaded_successfully:
        # Array of RGB values to interpolate (keep or modify as needed)
        rgb_values_to_interpolate = [
            (0.0, 0.0, 0.0),   # Corner
            (1.0, 1.0, 1.0),   # Opposite corner
            (1.0, 0.0, 0.0),   # R
            (0.0, 1.0, 0.0),   # G
            (0.0, 0.0, 1.0),   # B
            (1.0, 1.0, 0.0),   # Yellow
            (1.0, 0.0, 1.0),   # Purple
            (0.1, 0.5, 0.9),   # Random interior point 1
            (0.15, 0.15, 0.15),# Dark Gray
            (0.155, 0.15, 0.15),# Dark Gray (variation)
            (0.15, 0.155, 0.15),# Dark Gray (variation)
            (0.15, 0.15, 0.155),# Dark Gray (variation)
            (0.5, 0.5, 0.5),   # Center point
            (0.75, 0.75, 0.75),# Brigth Gray
            (0.8, 0.2, 0.4),   # Random interior point 2
            (0.8, 0.21, 0.4),  # Random interior point 3
            (0.335, 0.127, 0.023),   # Random interior point 4
            (0.997, 0.782, 0.901),   # Random interior point 5
            (1/3, 1/7, 1/6),   # Random interior point 6
            (0.25, 0.5, 0.75), # Point between grid lines
            (0.75, 0.5, 0.25), # Point between grid lines
            (0.251, 0.51, 0.751), # Point closely to grid lines
            (-0.1, 0.5, 1.1),  # Point 1 outside bounds (will be clamped)
            (-0.01, 0.62, 1.01)# Point 2 outside bounds (will be clamped)
        ]

        print("\nInterpolation Results:")
        # Ensure resolution is usable by interpolation functions if it's a single int
        if isinstance(resolution, int):
            lut_resolution_for_funcs = resolution
        elif isinstance(resolution, (list, tuple)) and len(resolution) == 3:
             # Assuming cubic, some functions might just need the single value
             if resolution[0] == resolution[1] == resolution[2]:
                 lut_resolution_for_funcs = resolution[0]
             else:
                 # If non-cubic, maybe pass the list/tuple if functions support it,
                 # or handle differently based on function needs. Here we use the list.
                 lut_resolution_for_funcs = resolution
        else:
            print("Error: Invalid resolution format obtained from LUT.")
            sys.exit(1)


        for r_in, g_in, b_in in rgb_values_to_interpolate:
            print(f"Input RGB: ({r_in:.3f}, {g_in:.3f}, {b_in:.3f})")

            # Call Linear Interpolation
            try:
                lin_result = linear_interpolation(lut_data, lut_resolution_for_funcs, r_in, g_in, b_in, domain_min, domain_max)
                print(f"  Linear (1D R-axis): [{lin_result[0]:.10f}, {lin_result[1]:.10f}, {lin_result[2]:.10f}]")
            except Exception as e:
                 print(f"  Linear (1D R-axis): Error - {e}")

            # Call Bilinear Interpolation
            try:
                bilin_result = bilinear_interpolation(lut_data, lut_resolution_for_funcs, r_in, g_in, b_in, domain_min, domain_max)
                print(f"  Bilinear (2D RG):   [{bilin_result[0]:.10f}, {bilin_result[1]:.10f}, {bilin_result[2]:.10f}]")
            except Exception as e:
                 print(f"  Bilinear (2D RG):   Error - {e}")

            # Call Nearest Neighbor Interpolation
            try:
                # NN usually just needs the single resolution value if cubic
                nn_res = resolution if isinstance(resolution, int) else resolution[0]
                nn_result = nearest_neighbor_interpolation(lut_data, nn_res, r_in, g_in, b_in, domain_min, domain_max)
                print(f"  Nearest Neighbor:   [{nn_result[0]:.10f}, {nn_result[1]:.10f}, {nn_result[2]:.10f}]")
            except Exception as e:
                 print(f"  Nearest Neighbor:   Error - {e}")

            # Call Trilinear Interpolation
            try:
                # Pass the prepared resolution and numpy domain arrays
                tl_result = trilinear_interpolation(lut_data, lut_resolution_for_funcs, r_in, g_in, b_in, domain_min, domain_max)
                print(f"  Trilinear:          [{tl_result[0]:.10f}, {tl_result[1]:.10f}, {tl_result[2]:.10f}]")
            except Exception as e:
                 print(f"  Trilinear:          Error - {e}")

            # Call Tetrahedral Interpolation
            try:
                # Pass the prepared resolution and numpy domain arrays
                tt_result = tetrahedral_interpolation(lut_data, lut_resolution_for_funcs, r_in, g_in, b_in, domain_min, domain_max)
                print(f"  Tetrahedral:        [{tt_result[0]:.10f}, {tt_result[1]:.10f}, {tt_result[2]:.10f}]")
            except Exception as e:
                 print(f"  Tetrahedral:        Error - {e}")

            # Call Barycentric Tetrahedral Interpolation
            try:
                # Pass the prepared resolution and numpy domain arrays
                bary_result = barycentric_tetrahedral_interpolation(lut_data, lut_resolution_for_funcs, r_in, g_in, b_in, domain_min, domain_max)
                print(f"  Barycentric:        [{bary_result[0]:.10f}, {bary_result[1]:.10f}, {bary_result[2]:.10f}]") # Used different result var name
            except Exception as e:
                 print(f"  Barycentric:        Error - {e}")

            # Call Bicubic Interpolation
            try:
                # Pass the prepared resolution and numpy domain arrays
                bicubic_result = bicubic_interpolation(lut_data, lut_resolution_for_funcs, r_in, g_in, b_in, domain_min, domain_max)
                print(f"  Bicubic:            [{bicubic_result[0]:.10f}, {bicubic_result[1]:.10f}, {bicubic_result[2]:.10f}]") # Used different result var name
            except Exception as e:
                 print(f"  Bicubic:            Error - {e}")

            # Call Cubic Spline Interpolation
            try:
                # Pass the prepared resolution and numpy domain arrays
                cubic_result = cubic_spline_interpolation(lut_data, lut_resolution_for_funcs, r_in, g_in, b_in, domain_min, domain_max)
                print(f"  Cubic Spline (1D):  [{cubic_result[0]:.10f}, {cubic_result[1]:.10f}, {cubic_result[2]:.10f}]") # Used different result var name & clarified title
            except Exception as e:
                 print(f"  Cubic Spline (1D):  Error - {e}")
 
            # Call Tricubic Interpolation
            try:
                # Pass the prepared resolution and numpy domain arrays
                tricubic_result = tricubic_interpolation(lut_data, lut_resolution_for_funcs, r_in, g_in, b_in, domain_min, domain_max)
                print(f"  Tricubic:           [{tricubic_result[0]:.10f}, {tricubic_result[1]:.10f}, {tricubic_result[2]:.10f}]")
            except Exception as e:
                 print(f"  Tricubic:           Error - {e}")

            # Call "Conceptual" Tri-Quintic (Cubic kernel on 6x6x6 grid)
            try:
                 # Pass the prepared resolution and numpy domain arrays
                 quintic_result = conceptual_triquintic_interpolation(lut_data, lut_resolution_for_funcs, r_in, g_in, b_in, domain_min, domain_max)
                 print(f"  ConceptualQuintic:  [{quintic_result[0]:.10f}, {quintic_result[1]:.10f}, {quintic_result[2]:.10f}]")
            except Exception as e:
                 print(f"  ConceptualQuintic:  Error - {e}")

            # Call RBF Interpolation (Use with extreme caution!)
#            try:
#                 # Pass the prepared resolution and numpy domain arrays
#                 rbf_result = rbf_interpolation(lut_data, lut_resolution_for_funcs, r_in, g_in, b_in, domain_min, domain_max)
#                 print(f"  RBF (Slow Demo):    [{rbf_result[0]:.10f}, {rbf_result[1]:.10f}, {rbf_result[2]:.10f}]")
#            except Exception as e:
#                 print(f"  RBF (Slow Demo):    Error - {e}")
#
#            # Call UNPROTECTED RBF Interpolation (Use with extreme caution!)
#            try:
#                 # Pass the prepared resolution and numpy domain arrays
#                 rbf_unprot_result = rbf_interpolation_unprotected(lut_data, lut_resolution_for_funcs, r_in, g_in, b_in, domain_min, domain_max)
#
#                 # Check if the result is valid before printing
#                 if isinstance(rbf_unprot_result, (list, np.ndarray)) and len(rbf_unprot_result) == 3:
#                     print(f"  RBF (Unprotected):  [{rbf_unprot_result[0]:.10f}, {rbf_unprot_result[1]:.10f}, {rbf_unprot_result[2]:.10f}]")
#                 else:
#                     print(f"  RBF (Unprotected):  Invalid result (received: {rbf_unprot_result})")
#
#            except Exception as e:
#                 # This catches other potential errors
#                 print(f"  RBF (Unprotected):  Exception during execution - {e}")

            print("-" * 20) # Separator

    else:
        # Exit if LUT loading failed
        print("Failed to load LUT. Exiting.")
        sys.exit(1) # Exit with a non-zero status code to indicate failure
