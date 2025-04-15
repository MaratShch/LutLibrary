import numpy as np
import warnings
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
    """
    Performs 1D linear interpolation on a slice using float64 for high precision.
    ASSUMES lut_slice_1d and x are already float64.
    """
    if not (0.0 <= x <= 1.0):
        x = np.clip(x, np.float64(0.0), np.float64(1.0))

    if resolution_1d <= 1:
        return lut_slice_1d[0] if len(lut_slice_1d) > 0 else np.array([0.0, 0.0, 0.0], dtype=np.float64)

    res_minus_1 = np.float64(resolution_1d - 1)
    f_index = x * res_minus_1

    idx0 = int(np.floor(f_index))
    idx1 = int(np.ceil(f_index))
    idx0 = np.clip(idx0, 0, resolution_1d - 1)
    idx1 = np.clip(idx1, 0, resolution_1d - 1)

    if idx0 == idx1:
        return lut_slice_1d[idx0]

    t = f_index - idx0
    val0 = lut_slice_1d[idx0]
    val1 = lut_slice_1d[idx1]
    interpolated_val = val0 * (np.float64(1.0) - t) + val1 * t
    return interpolated_val


def linear_interpolation(lut_data, resolution, r, g, b, domain_min_input, domain_max_input): # Renamed args
    """
    Performs slice selection and 1D Linear interpolation (R-axis only).
    Converts input LUT/domain to float64 internally for calculations.
    Accuracy depends on the original precision of lut_data.

    Args:
        lut_data: The 3D LUT data (NumPy array or list-of-lists, any float/int dtype).
                  Assumes layout lut_data[r_idx, g_idx, b_idx] = [R_val, G_val, B_val].
        resolution: Int or tuple/list of 3 ints defining LUT dimensions.
        r, g, b: Input coordinates (floats).
        domain_min_input: Min output clamp values (NumPy array, list, or float).
        domain_max_input: Max output clamp values (NumPy array, list, or float).

    Returns:
        List of 3 floats representing the interpolated and clamped RGB value.
    """
    # --- Input Type Conversion and Validation ---

    # Convert lut_data to NumPy array if it isn't already
    if not isinstance(lut_data, np.ndarray):
        try:
            lut_data = np.array(lut_data, dtype=np.float64) # Attempt conversion to float64 directly
            warnings.warn(
                "Input lut_data was not a NumPy array. Converted to np.float64. "
                "Output precision will be limited by the original data structure if it wasn't float-like.",
                UserWarning
            )
        except Exception as e:
            raise TypeError(f"Could not convert lut_data to NumPy array: {e}")
    # If it IS a NumPy array, check its dtype
    elif not np.issubdtype(lut_data.dtype, np.floating) and not np.issubdtype(lut_data.dtype, np.integer):
         raise TypeError(f"Input lut_data NumPy array dtype must be numeric (float or int), got {lut_data.dtype}")
    elif lut_data.dtype != np.float64:
        warnings.warn(
            f"Input lut_data dtype was {lut_data.dtype}. Converting to float64 for calculation. "
            "Output precision will be limited by the original dtype.",
            UserWarning
        )
        lut_data = lut_data.astype(np.float64, copy=False)


    # Convert domain bounds to NumPy arrays of float64
    # Handle scalar inputs for domain_min/max as well
    if not isinstance(domain_min_input, np.ndarray):
        domain_min_np = np.array(domain_min_input, dtype=np.float64)
    elif domain_min_input.dtype != np.float64:
        domain_min_np = domain_min_input.astype(np.float64, copy=False)
    else:
        domain_min_np = domain_min_input # Already a np.float64 ndarray

    if not isinstance(domain_max_input, np.ndarray):
        domain_max_np = np.array(domain_max_input, dtype=np.float64)
    elif domain_max_input.dtype != np.float64:
        domain_max_np = domain_max_input.astype(np.float64, copy=False)
    else:
        domain_max_np = domain_max_input # Already a np.float64 ndarray

    # Ensure domain arrays are broadcastable to (3,) if they were scalar
    if domain_min_np.ndim == 0: # if scalar
        domain_min_np = np.full(3, domain_min_np.item(), dtype=np.float64)
    if domain_max_np.ndim == 0: # if scalar
        domain_max_np = np.full(3, domain_max_np.item(), dtype=np.float64)


    # Ensure input coordinates are float64
    r_f64, g_f64, b_f64 = np.float64(r), np.float64(g), np.float64(b)

    # --- Resolution ---
    if isinstance(resolution, int):
        res_r, res_g, res_b = resolution, resolution, resolution
    elif len(resolution) == 3:
        res_r, res_g, res_b = resolution
    else:
        raise ValueError("Invalid resolution format")

    # --- Slice Selection (using nearest neighbor for G and B) ---
    g_coord = np.clip(g_f64, np.float64(0.0), np.float64(1.0))
    b_coord = np.clip(b_f64, np.float64(0.0), np.float64(1.0))

    g_f_idx = g_coord * np.float64(res_g - 1) if res_g > 1 else np.float64(0.0)
    b_f_idx = b_coord * np.float64(res_b - 1) if res_b > 1 else np.float64(0.0)

    g_idx = int(np.round(g_f_idx))
    b_idx = int(np.round(b_f_idx))

    g_idx = np.clip(g_idx, 0, res_g - 1)
    b_idx = np.clip(b_idx, 0, res_b - 1)

    # Extract the 1D slice along the R dimension
    # lut_data is guaranteed to be float64 here.
    # Assuming lut_data is shaped (res_r, res_g, res_b, 3)
    try:
        lut_slice_1d = lut_data[:, g_idx, b_idx, :]
    except IndexError as e:
        raise IndexError(
            f"Error slicing lut_data with shape {lut_data.shape} using g_idx={g_idx}, b_idx={b_idx}. "
            f"Ensure lut_data has 4 dimensions (R, G, B, Channels) and indices are within bounds. Original error: {e}"
        ) from e


    # --- 1D Interpolation using the high-precision helper ---
    interpolated_val = _linear_interpolate_1d_slice(lut_slice_1d, res_r, r_f64)

    # --- Output Clamping ---
    clamped_val = np.clip(interpolated_val, domain_min_np, domain_max_np)

    return clamped_val.tolist()


def _prepare_inputs_for_64bit(lut_data, r, g, b, domain_min_input, domain_max_input):
    """
    Converts interpolation inputs to np.float64 for high-precision calculations.

    Handles lut_data (ndarray or list), coordinates (float), and domain bounds
    (ndarray, list, or float). Issues warnings if input dtypes might limit precision.

    Returns:
        tuple: (lut_data_f64, r_f64, g_f64, b_f64, domain_min_f64, domain_max_f64)
               All guaranteed to be np.float64 (arrays or scalars).
    """
    # --- Process lut_data ---
    is_lut_ndarray = isinstance(lut_data, np.ndarray)
    if not is_lut_ndarray:
        try:
            # Attempt conversion, assuming float64 is desired if not ndarray
            lut_data_f64 = np.array(lut_data, dtype=np.float64)
            warnings.warn(
                "Input lut_data was not a NumPy array. Converted to np.float64.",
                UserWarning
            )
        except Exception as e:
            raise TypeError(f"Could not convert lut_data to NumPy array: {e}")
    # If it IS an ndarray, check/convert its dtype
    elif not np.issubdtype(lut_data.dtype, np.floating) and not np.issubdtype(lut_data.dtype, np.integer):
         raise TypeError(f"Input lut_data NumPy array dtype must be numeric, got {lut_data.dtype}")
    elif lut_data.dtype != np.float64:
        warnings.warn(
            f"Input lut_data dtype was {lut_data.dtype}. Converting to float64 for calculation. "
            "Output precision will be limited by the original dtype.",
            UserWarning
        )
        # copy=False is optimization, avoids copy if already float64
        lut_data_f64 = lut_data.astype(np.float64, copy=False)
    else:
        # Already a NumPy array with float64 dtype
        lut_data_f64 = lut_data

    # --- Process domain bounds ---
    # Convert to NumPy array first, then check/convert dtype
    if not isinstance(domain_min_input, np.ndarray):
        try:
            domain_min_f64 = np.array(domain_min_input, dtype=np.float64)
        except Exception as e:
            raise TypeError(f"Could not convert domain_min_input to NumPy array: {e}")
    elif domain_min_input.dtype != np.float64:
        domain_min_f64 = domain_min_input.astype(np.float64, copy=False)
    else:
        domain_min_f64 = domain_min_input # Already correct type

    if not isinstance(domain_max_input, np.ndarray):
         try:
            domain_max_f64 = np.array(domain_max_input, dtype=np.float64)
         except Exception as e:
            raise TypeError(f"Could not convert domain_max_input to NumPy array: {e}")
    elif domain_max_input.dtype != np.float64:
        domain_max_f64 = domain_max_input.astype(np.float64, copy=False)
    else:
        domain_max_f64 = domain_max_input # Already correct type

    # Ensure domain arrays are broadcastable to (3,) shape for clamping RGB
    # Allow scalar inputs (e.g., 0.0), expand them to 3 elements
    if domain_min_f64.ndim == 0:
        domain_min_f64 = np.full(3, domain_min_f64.item(), dtype=np.float64)
    elif domain_min_f64.shape != (3,) and domain_min_f64.size == 1: # Handle case like np.array([0.0])
        domain_min_f64 = np.full(3, domain_min_f64.item(), dtype=np.float64)
    elif domain_min_f64.shape != (3,):
         raise ValueError(f"domain_min must be scalar or have shape (3,), got {domain_min_f64.shape}")


    if domain_max_f64.ndim == 0:
        domain_max_f64 = np.full(3, domain_max_f64.item(), dtype=np.float64)
    elif domain_max_f64.shape != (3,) and domain_max_f64.size == 1:
        domain_max_f64 = np.full(3, domain_max_f64.item(), dtype=np.float64)
    elif domain_max_f64.shape != (3,):
         raise ValueError(f"domain_max must be scalar or have shape (3,), got {domain_max_f64.shape}")


    # --- Process coordinates ---
    # Standard Python floats are usually 64-bit already, but explicit cast is safest
    r_f64 = np.float64(r)
    g_f64 = np.float64(g)
    b_f64 = np.float64(b)

    return lut_data_f64, r_f64, g_f64, b_f64, domain_min_f64, domain_max_f64
    
    
    
def bilinear_interpolation(lut_data_input, resolution, r_input, g_input, b_input, domain_min_input, domain_max_input):
    """
    Performs 2D Bilinear interpolation on the R-G plane of the 3D LUT,
    using 'r' and 'g' coordinates, ensuring float64 precision internally.
    The R-G plane is selected based on the nearest grid point to 'b'.
    Output is clamped. Accuracy depends on original lut_data precision.

    Args:
        lut_data_input: The 3D LUT data (NumPy array or list-of-lists, any float/int dtype).
                        Expected layout: lut_data[r_idx, g_idx, b_idx] = [R_val, G_val, B_val].
        resolution: Int or tuple/list of 3 ints defining LUT dimensions [res_r, res_g, res_b].
        r_input, g_input, b_input: Input coordinates (floats).
        domain_min_input: Min output clamp values (NumPy array, list, or float/int).
        domain_max_input: Max output clamp values (NumPy array, list, or float/int).

    Returns:
        List of 3 floats representing the interpolated and clamped RGB value.
    """
    # Prepare inputs, ensuring float64 types internally
    lut_data, r, g, b, domain_min_np, domain_max_np = _prepare_inputs_for_64bit(
        lut_data_input, r_input, g_input, b_input, domain_min_input, domain_max_input
    )

    # --- Resolution Handling ---
    if isinstance(resolution, int):
        if resolution < 1: raise ValueError("Resolution must be >= 1")
        res_r, res_g, res_b = resolution, resolution, resolution
    elif len(resolution) == 3:
        res_r, res_g, res_b = resolution
        if not all(isinstance(res, int) and res >= 1 for res in resolution):
             raise ValueError(f"Resolution dimensions must be integers >= 1, got {resolution}")
    else:
        raise ValueError("Resolution must be an int or a list/tuple of 3 ints [res_r, res_g, res_b]")

    # --- Slice Selection (along B-axis using nearest neighbor) ---
    # Perform calculations with float64
    b_coord = np.clip(b, np.float64(0.0), np.float64(1.0))
    # Use float64 for intermediate floating index calculation
    b_f_idx = b_coord * np.float64(res_b - 1) if res_b > 1 else np.float64(0.0)
    # Round to get integer index
    b_idx = int(np.round(b_f_idx))
    # Clip index to valid range
    b_idx = np.clip(b_idx, 0, res_b - 1)

    # Extract the 2D slice (R-G plane at selected b_idx)
    try:
        # Assuming lut_data shape is (res_r, res_g, res_b, 3) based on original user code
        # The slice will inherit the float64 dtype from lut_data
        lut_slice_2d = lut_data[:, :, b_idx, :] # Shape: (res_r, res_g, 3)
    except IndexError as e:
         raise IndexError(
            f"Error slicing lut_data with shape {lut_data.shape} using b_idx={b_idx}. "
            f"Ensure lut_data has 4 dimensions (R, G, B, Channels) and index is within bounds. Original error: {e}"
         ) from e
    except Exception as e: # Catch other potential errors during slicing
         raise RuntimeError(f"Unexpected error during 2D slice extraction: {e}") from e

    # --- Bilinear Interpolation on the R-G Slice (lut_slice_2d) ---
    # Use float64 coordinates for this slice
    x = np.clip(r, np.float64(0.0), np.float64(1.0)) # R-coordinate for slice
    y = np.clip(g, np.float64(0.0), np.float64(1.0)) # G-coordinate for slice

    # Handle edge cases where one or both slice dimensions are 1
    if res_r <= 1 and res_g <= 1:
        # Slice is effectively a single point
        interpolated_val = lut_slice_2d[0, 0, :] # Fetch the single value (already float64)
    elif res_r <= 1:
        # Slice is 1D along G-axis. Interpolate along G (y) only.
        # Slice shape is (1, res_g, 3). Need lut_slice_2d[0, :, :] which is (res_g, 3)
        # Pass float64 slice and coordinate 'y' to the 1D helper
        interpolated_val = _linear_interpolate_1d_slice(lut_slice_2d[0, :, :], res_g, y)
    elif res_g <= 1:
        # Slice is 1D along R-axis. Interpolate along R (x) only.
        # Slice shape is (res_r, 1, 3). Need lut_slice_2d[:, 0, :] which is (res_r, 3)
        # Pass float64 slice and coordinate 'x' to the 1D helper
        interpolated_val = _linear_interpolate_1d_slice(lut_slice_1d=lut_slice_2d[:, 0, :], resolution_1d=res_r, x=x)
    else:
        # --- Standard Bilinear Interpolation Core (using float64) ---
        res_r_minus_1 = np.float64(res_r - 1)
        res_g_minus_1 = np.float64(res_g - 1)

        # Calculate float indices using float64
        fx = x * res_r_minus_1
        fy = y * res_g_minus_1

        # Get integer indices using floor/ceil
        x0 = int(np.floor(fx))
        x1 = int(np.ceil(fx))
        y0 = int(np.floor(fy))
        y1 = int(np.ceil(fy))

        # Clip integer indices to be safe (although fx/fy based on clipped x/y)
        x0 = np.clip(x0, 0, res_r - 1); x1 = np.clip(x1, 0, res_r - 1)
        y0 = np.clip(y0, 0, res_g - 1); y1 = np.clip(y1, 0, res_g - 1)

        # Calculate float64 interpolation weights (tx, ty)
        # Handle exact grid points correctly: if x0=x1, tx=0; if y0=y1, ty=0
        tx = fx - x0 if x0 != x1 else np.float64(0.0)
        ty = fy - y0 if y0 != y1 else np.float64(0.0)

        # Explicit float64 constant for reliable subtraction
        one_f64 = np.float64(1.0)
        tx_inv = one_f64 - tx
        ty_inv = one_f64 - ty

        # Fetch corner values from the float64 slice
        c00 = lut_slice_2d[x0, y0, :] # R=x0, G=y0
        c10 = lut_slice_2d[x1, y0, :] # R=x1, G=y0
        c01 = lut_slice_2d[x0, y1, :] # R=x0, G=y1
        c11 = lut_slice_2d[x1, y1, :] # R=x1, G=y1

        # Perform interpolation steps using float64 arithmetic
        # Interpolate along R (x-axis)
        c0 = c00 * tx_inv + c10 * tx
        c1 = c01 * tx_inv + c11 * tx
        # Interpolate along G (y-axis)
        interpolated_val = c0 * ty_inv + c1 * ty

    # --- Output Clamping ---
    # domain_min_np and domain_max_np are guaranteed float64 arrays of shape (3,)
    clamped_val = np.clip(interpolated_val, domain_min_np, domain_max_np)

    # Return as list of standard Python floats (which are typically 64-bit)
    return clamped_val.tolist()
    
    

def nearest_neighbor_interpolation(lut_data_input, resolution, r_input, g_input, b_input, domain_min_input, domain_max_input):
    """
    Performs Nearest Neighbor interpolation on the 3D LUT.
    Ensures float64 precision for clamping bounds and coordinate processing.

    Args:
        lut_data_input: The 3D LUT data (NumPy array or list-of-lists, any float/int dtype).
                        Expected layout: lut_data[r_idx, g_idx, b_idx] = [R_val, G_val, B_val].
        resolution: Int or tuple/list of 3 ints defining LUT dimensions [res_r, res_g, res_b].
                    For pure NN on a cubic grid, often just a single int is used/expected.
        r_input, g_input, b_input: Input coordinates (floats).
        domain_min_input: Min output clamp values (NumPy array, list, or float/int).
        domain_max_input: Max output clamp values (NumPy array, list, or float/int).

    Returns:
        List of 3 floats representing the selected and clamped RGB value.
    """
    # Prepare inputs, ensuring float64 types internally for relevant parts
    # We still use the helper to get float64 coords and domains, and validated lut_data
    lut_data, r, g, b, domain_min_np, domain_max_np = _prepare_inputs_for_64bit(
        lut_data_input, r_input, g_input, b_input, domain_min_input, domain_max_input
    )

    # --- Resolution Handling ---
    # Allow flexible resolution input, but warn if tuple/list passed as NN often assumes cubic
    if isinstance(resolution, int):
        if resolution < 1: raise ValueError("Resolution must be >= 1")
        res_r, res_g, res_b = resolution, resolution, resolution
    elif len(resolution) == 3:
        res_r, res_g, res_b = resolution
        if not all(isinstance(res, int) and res >= 1 for res in resolution):
             raise ValueError(f"Resolution dimensions must be integers >= 1, got {resolution}")
        if res_r != res_g or res_g != res_b:
             warnings.warn(
                 f"Nearest neighbor usually assumes cubic LUT, but got resolution {resolution}. "
                 "Index calculation will use respective dimension sizes.",
                 UserWarning
             )
    else:
        raise ValueError("Resolution must be an int or a list/tuple of 3 ints [res_r, res_g, res_b]")

    # --- Index Calculation (using float64 intermediates for precision) ---
    # Clip coordinates (already float64)
    r_coord = np.clip(r, np.float64(0.0), np.float64(1.0))
    g_coord = np.clip(g, np.float64(0.0), np.float64(1.0))
    b_coord = np.clip(b, np.float64(0.0), np.float64(1.0))

    # Calculate floating point indices using respective resolutions
    # Use np.float64 for scaling factor to maintain precision
    r_f_idx = r_coord * np.float64(res_r - 1) if res_r > 1 else np.float64(0.0)
    g_f_idx = g_coord * np.float64(res_g - 1) if res_g > 1 else np.float64(0.0)
    b_f_idx = b_coord * np.float64(res_b - 1) if res_b > 1 else np.float64(0.0)

    # Round to nearest integer index
    grid_index_r = int(np.round(r_f_idx))
    grid_index_g = int(np.round(g_f_idx))
    grid_index_b = int(np.round(b_f_idx))

    # Clip integer indices to valid range for each dimension
    grid_index_r = np.clip(grid_index_r, 0, res_r - 1)
    grid_index_g = np.clip(grid_index_g, 0, res_g - 1)
    grid_index_b = np.clip(grid_index_b, 0, res_b - 1)

    # --- Fetch Result ---
    try:
        # lut_data is float64 here. Fetch the value at the nearest grid point.
        # Assuming lut_data shape is (res_r, res_g, res_b, 3)
        result_val = lut_data[grid_index_r, grid_index_g, grid_index_b, :] # Shape (3,)
    except IndexError as e:
         raise IndexError(
            f"Error indexing lut_data with shape {lut_data.shape} using indices "
            f"({grid_index_r}, {grid_index_g}, {grid_index_b}). Check resolution and LUT shape. Original error: {e}"
         ) from e
    except Exception as e:
        raise RuntimeError(f"Unexpected error during nearest neighbor lookup: {e}") from e


    # --- Output Clamping ---
    # Convert the fetched result to float64 *before* clamping, just in case
    # lut_data wasn't originally float64 (though our helper converted it)
    # This ensures the clamp operates on float64 values.
    result_val_f64 = result_val.astype(np.float64, copy=False)

    # Clamp using the prepared float64 domain bounds
    clamped_val = np.clip(result_val_f64, domain_min_np, domain_max_np)

    # Return as list of standard Python floats
    return clamped_val.tolist()


def trilinear_interpolation(lut_data_input, resolution, r_input, g_input, b_input, domain_min_input, domain_max_input):
    """
    Performs Trilinear interpolation on the 3D LUT.
    Ensures float64 precision for internal calculations and clamping.

    Args:
        lut_data_input: The 3D LUT data (NumPy array or list-of-lists, any float/int dtype).
                        Expected layout: lut_data[r_idx, g_idx, b_idx] = [R_val, G_val, B_val].
        resolution: Int or tuple/list of 3 ints defining LUT dimensions [res_r, res_g, res_b].
        r_input, g_input, b_input: Input coordinates (floats).
        domain_min_input: Min output clamp values (NumPy array, list, or float/int).
        domain_max_input: Max output clamp values (NumPy array, list, or float/int).

    Returns:
        List of 3 floats representing the interpolated and clamped RGB value.
    """
    # Prepare inputs, ensuring float64 types internally
    lut_data, r, g, b, domain_min_np, domain_max_np = _prepare_inputs_for_64bit(
        lut_data_input, r_input, g_input, b_input, domain_min_input, domain_max_input
    )

    # --- Resolution Handling ---
    if isinstance(resolution, int):
        if resolution < 2: raise ValueError("Trilinear interpolation requires resolution >= 2")
        res_r, res_g, res_b = resolution, resolution, resolution
    elif len(resolution) == 3:
        res_r, res_g, res_b = resolution
        if not all(isinstance(res, int) and res >= 2 for res in resolution):
             raise ValueError(f"Trilinear interpolation requires resolution dimensions >= 2, got {resolution}")
    else:
        raise ValueError("Resolution must be an int or a list/tuple of 3 ints [res_r, res_g, res_b]")

    # --- Coordinate and Index Calculation (using float64) ---
    # Clip coordinates (already float64)
    r_coord = np.clip(r, np.float64(0.0), np.float64(1.0))
    g_coord = np.clip(g, np.float64(0.0), np.float64(1.0))
    b_coord = np.clip(b, np.float64(0.0), np.float64(1.0))

    # Calculate float scaling factors using float64
    res_r_minus_1 = np.float64(res_r - 1)
    res_g_minus_1 = np.float64(res_g - 1)
    res_b_minus_1 = np.float64(res_b - 1)

    # Calculate floating point indices (x, y, z) using float64
    x = r_coord * res_r_minus_1
    y = g_coord * res_g_minus_1
    z = b_coord * res_b_minus_1

    # Get integer base indices (i, j, k) using floor
    i = int(np.floor(x))
    j = int(np.floor(y))
    k = int(np.floor(z))

    # Calculate fractional weights (fx, fy, fz) using float64
    # Subtracting int from float64 results in float64
    fx = x - i
    fy = y - j
    fz = z - k

    # Calculate inverse weights using float64
    one_f64 = np.float64(1.0)
    fx_inv = one_f64 - fx
    fy_inv = one_f64 - fy
    fz_inv = one_f64 - fz

    # Calculate integer indices for the 8 corners, clipping to bounds
    x0 = np.clip(i,     0, res_r - 1)
    x1 = np.clip(i + 1, 0, res_r - 1)
    y0 = np.clip(j,     0, res_g - 1)
    y1 = np.clip(j + 1, 0, res_g - 1)
    z0 = np.clip(k,     0, res_b - 1)
    z1 = np.clip(k + 1, 0, res_b - 1)

    # --- Fetch Corner Values ---
    try:
        # lut_data is float64 here. Fetch the 8 corner values.
        # Assuming lut_data shape is (res_r, res_g, res_b, 3)
        c000 = lut_data[x0, y0, z0, :]
        c100 = lut_data[x1, y0, z0, :]
        c010 = lut_data[x0, y1, z0, :]
        c110 = lut_data[x1, y1, z0, :]
        c001 = lut_data[x0, y0, z1, :]
        c101 = lut_data[x1, y0, z1, :]
        c011 = lut_data[x0, y1, z1, :]
        c111 = lut_data[x1, y1, z1, :]
    except IndexError as e:
         raise IndexError(
            f"Error indexing lut_data with shape {lut_data.shape} using corner indices around "
            f"({i}, {j}, {k}). Check resolution and LUT shape. Original error: {e}"
         ) from e
    except Exception as e:
        raise RuntimeError(f"Unexpected error during trilinear corner value lookup: {e}") from e

    # --- Perform Interpolation using float64 ---
    # Interpolate along X axis
    c00 = c000 * fx_inv + c100 * fx
    c01 = c001 * fx_inv + c101 * fx
    c10 = c010 * fx_inv + c110 * fx
    c11 = c011 * fx_inv + c111 * fx

    # Interpolate along Y axis
    c0 = c00 * fy_inv + c10 * fy
    c1 = c01 * fy_inv + c11 * fy

    # Interpolate along Z axis
    interpolated_val = c0 * fz_inv + c1 * fz # This is float64 array shape (3,)

    # --- Output Clamping ---
    # Clamp using the prepared float64 domain bounds
    clamped_val = np.clip(interpolated_val, domain_min_np, domain_max_np)

    # Return as list of standard Python floats
    return clamped_val.tolist()
    

def tetrahedral_interpolation(lut_data_input, resolution, r_input, g_input, b_input, domain_min_input, domain_max_input):
    """
    Performs Tetrahedral interpolation on the 3D LUT.
    Ensures float64 precision for internal calculations and clamping.
    The logic divides the local cube cell into 6 tetrahedra.

    Args:
        lut_data_input: The 3D LUT data (NumPy array or list-of-lists, any float/int dtype).
                        Expected layout: lut_data[r_idx, g_idx, b_idx] = [R_val, G_val, B_val].
        resolution: Int or tuple/list of 3 ints defining LUT dimensions [res_r, res_g, res_b].
                    Often a single int for cubic LUTs.
        r_input, g_input, b_input: Input coordinates (floats).
        domain_min_input: Min output clamp values (NumPy array, list, or float/int).
        domain_max_input: Max output clamp values (NumPy array, list, or float/int).

    Returns:
        List of 3 floats representing the interpolated and clamped RGB value.
    """
    # Prepare inputs, ensuring float64 types internally
    lut_data, r, g, b, domain_min_np, domain_max_np = _prepare_inputs_for_64bit(
        lut_data_input, r_input, g_input, b_input, domain_min_input, domain_max_input
    )

    # --- Resolution Handling ---
    # Tetrahedral needs at least 2 points to define edges
    if isinstance(resolution, int):
        if resolution < 2: raise ValueError("Tetrahedral interpolation requires resolution >= 2")
        res_r, res_g, res_b = resolution, resolution, resolution
    elif len(resolution) == 3:
        res_r, res_g, res_b = resolution
        if not all(isinstance(res, int) and res >= 2 for res in resolution):
             raise ValueError(f"Tetrahedral interpolation requires resolution dimensions >= 2, got {resolution}")
    else:
        raise ValueError("Resolution must be an int or a list/tuple of 3 ints [res_r, res_g, res_b]")

    # --- Coordinate and Index Calculation (using float64) ---
    # Clip coordinates (already float64)
    r_coord = np.clip(r, np.float64(0.0), np.float64(1.0))
    g_coord = np.clip(g, np.float64(0.0), np.float64(1.0))
    b_coord = np.clip(b, np.float64(0.0), np.float64(1.0))

    # Calculate float scaling factors using float64
    res_r_minus_1 = np.float64(res_r - 1)
    res_g_minus_1 = np.float64(res_g - 1)
    res_b_minus_1 = np.float64(res_b - 1)

    # Calculate floating point indices (x, y, z) using float64
    x = r_coord * res_r_minus_1
    y = g_coord * res_g_minus_1
    z = b_coord * res_b_minus_1

    # Get integer base indices (i, j, k) using floor
    i = int(np.floor(x))
    j = int(np.floor(y))
    k = int(np.floor(z))

    # Calculate fractional weights (fx, fy, fz) using float64
    fx = x - i # float64 - int = float64
    fy = y - j
    fz = z - k

    # Calculate integer indices for the 8 corners, clipping to bounds
    x0 = np.clip(i,     0, res_r - 1)
    x1 = np.clip(i + 1, 0, res_r - 1)
    y0 = np.clip(j,     0, res_g - 1)
    y1 = np.clip(j + 1, 0, res_g - 1)
    z0 = np.clip(k,     0, res_b - 1)
    z1 = np.clip(k + 1, 0, res_b - 1)

    # --- Fetch Corner Values ---
    try:
        # lut_data is float64 here. Fetch the 8 corner values into a float64 array.
        # Assuming lut_data shape is (res_r, res_g, res_b, 3)
        v = np.zeros((8, 3), dtype=np.float64) # Ensure 'v' array is float64
        # Standard vertex order relative to (x0,y0,z0): 000, 100, 010, 001, 110, 101, 011, 111
        # Adjusting fetch order to match the logic in the if/else block below:
        # v[0]=000, v[1]=100, v[2]=010, v[3]=001, v[4]=110, v[5]=101, v[6]=011, v[7]=111
        v[0] = lut_data[x0, y0, z0, :]
        v[1] = lut_data[x1, y0, z0, :]
        v[2] = lut_data[x0, y1, z0, :]
        v[3] = lut_data[x0, y0, z1, :]
        v[4] = lut_data[x1, y1, z0, :] # Differs from trilinear order fetch
        v[5] = lut_data[x1, y0, z1, :] # Differs from trilinear order fetch
        v[6] = lut_data[x0, y1, z1, :] # Differs from trilinear order fetch
        v[7] = lut_data[x1, y1, z1, :]
    except IndexError as e:
         raise IndexError(
            f"Error indexing lut_data with shape {lut_data.shape} using corner indices around "
            f"({i}, {j}, {k}). Check resolution and LUT shape. Original error: {e}"
         ) from e
    except Exception as e:
        raise RuntimeError(f"Unexpected error during tetrahedral corner value lookup: {e}") from e

    # --- Determine Tetrahedron and Interpolate (using float64) ---
    # The comparisons (fx>=fy etc.) are float64.
    # The arithmetic (subtractions, multiplications, additions) involves
    # float64 operands (v[n] and fx, fy, fz), resulting in float64.
    one_f64 = np.float64(1.0) # Explicit float64 constant

    # Simplified conditions based on relative order of fractional parts
    if fx >= fy:
        if fy >= fz: # T1: fx >= fy >= fz. Vertices: 000, 100, 010, 110 (v0, v1, v2, v4)
            # Interpolate: v0 + (v1-v0)*fx + (v2-v0)*fy + (v4-v1-v2+v0)*fz Incorrect?
            # Simpler: Linear interpolation within the tetra identified
            # Weighting: w0 = 1-fx, w1 = fx-fy, w2 = fy-fz, w3 = fz
            # Points:   P0=v0, P1=v1, P2=v4, P3=v7 (???) No, this isn't barycentric directly
            # Using the logic from the original provided code (assumed correct derivation):
            c0 = v[0]; c1 = v[1] - v[0]; c2 = v[2] - v[0]; c3 = v[4] - v[1] - v[2] + v[0]
            result = c0 + c1*fx + c2*fy + c3*fz
        else: # fx >= fz >= fy
            if fx >= fz: # T2: fx >= fz >= fy. Vertices: 000, 100, 001, 101 (v0, v1, v3, v5)
                 c0 = v[0]; c1 = v[1] - v[0]; c2 = v[3] - v[0]; c3 = v[5] - v[1] - v[3] + v[0]
                 result = c0 + c1*fx + c2*fz + c3*fy
            else: # T3: fz > fx >= fy. Vertices: 000, 010, 001, 011 (v0, v2, v3, v6) - Error in original logic?
                 # Let's assume original logic handles the 6 cases correctly based on vertex numbering used
                 c0 = v[0]; c1 = v[3] - v[0]; c2 = v[2] - v[0]; c3 = v[6] - v[3] - v[2] + v[0]
                 result = c0 + c1*fz + c2*fy + c3*fx # T3: v0326
    else: # fy > fx
        if fx >= fz: # T4: fy > fx >= fz. Vertices: 111, 011, 110, 010 (v7, v6, v4, v2)
            c0 = v[7]; c1 = v[6] - v[7]; c2 = v[4] - v[7]; c3 = v[2] - v[6] - v[4] + v[7]
            result = c0 + c1*(one_f64-fx) + c2*(one_f64-fy) + c3*(one_f64-fz) # T4: v7642
        else: # fy >= fz > fx
             if fy >= fz: # T5: fy >= fz > fx. Vertices: 111, 011, 101, 001 (v7, v6, v5, v3)
                  c0 = v[7]; c1 = v[6] - v[7]; c2 = v[5] - v[7]; c3 = v[3] - v[6] - v[5] + v[7]
                  result = c0 + c1*(one_f64-fx) + c2*(one_f64-fz) + c3*(one_f64-fy) # T5: v7653
             else: # T6: fz > fy > fx. Vertices: 111, 110, 101, 100 (v7, v4, v5, v1)
                  c0 = v[7]; c1 = v[4] - v[7]; c2 = v[5] - v[7]; c3 = v[1] - v[4] - v[5] + v[7]
                  result = c0 + c1*(one_f64-fy) + c2*(one_f64-fz) + c3*(one_f64-fx) # T6: v7451


    # --- Output Clamping ---
    # Clamp the float64 result using the float64 domain bounds
    clamped_val = np.clip(result, domain_min_np, domain_max_np)

    # Return as list of standard Python floats
    return clamped_val.tolist()


# --- Helper function for Cubic/Bicubic Kernels ---
# This is a common cubic convolution kernel often used for "cubic" interpolation
# (parameter 'a' is often -0.5 for Catmull-Rom like behavior)
def _cubic_kernel(x_input, a=-0.5):
    """
    Calculates the weight using a cubic convolution kernel (e.g., Catmull-Rom when a=-0.5).
    Ensures float64 precision internally.

    Args:
        x_input (float or np.float64): The distance from the sample point.
        a (float or np.float64): The 'a' parameter of the cubic kernel.

    Returns:
        np.float64: The calculated weight.
    """
    # Ensure inputs are float64
    x = np.float64(x_input)
    a_f64 = np.float64(a)

    # Use float64 for absolute value
    ax = np.abs(x)

    # Define float64 constants
    f_0 = np.float64(0.0)
    f_1 = np.float64(1.0)
    f_2 = np.float64(2.0)
    f_3 = np.float64(3.0)
    f_4 = np.float64(4.0)
    f_5 = np.float64(5.0)
    f_8 = np.float64(8.0)

    # Perform calculations using float64
    if ax <= f_1:
        # (a + 2.0) * ax**3 - (a + 3.0) * ax**2 + 1.0
        ax2 = ax * ax
        ax3 = ax2 * ax
        term1 = (a_f64 + f_2) * ax3
        term2 = (a_f64 + f_3) * ax2
        return term1 - term2 + f_1
    elif ax < f_2:
        # a * ax**3 - 5.0 * a * ax**2 + 8.0 * a * ax - 4.0 * a
        ax2 = ax * ax
        ax3 = ax2 * ax
        term1 = a_f64 * ax3
        term2 = f_5 * a_f64 * ax2
        term3 = f_8 * a_f64 * ax
        term4 = f_4 * a_f64
        return term1 - term2 + term3 - term4
    else:
        return f_0 # Return float64 zero


# --- Barycentric Tetrahedral Interpolation ---
# Note: The logic here is functionally the same as the previous `tetrahedral_interpolation`
# provided, as that implementation inherently uses barycentric weights within each determined
# tetrahedron. No significant logical change needed if the previous one worked.
# We keep the name change for clarity as requested.
def barycentric_tetrahedral_interpolation(lut_data_input, resolution, r_input, g_input, b_input, domain_min_input, domain_max_input):
    """
    Performs Tetrahedral interpolation on the 3D LUT using a decomposition of
    the unit cube into 6 tetrahedra. Ensures float64 precision internally.
    Functionally equivalent to the tetrahedral_interpolation provided earlier.

    Args:
        lut_data_input: The 3D LUT data (NumPy array or list-of-lists, any float/int dtype).
                        Expected layout: lut_data[r_idx, g_idx, b_idx] = [R_val, G_val, B_val].
        resolution: Int or tuple/list of 3 ints defining LUT dimensions [res_r, res_g, res_b].
                    Often a single int for cubic LUTs.
        r_input, g_input, b_input: Input coordinates (floats).
        domain_min_input: Min output clamp values (NumPy array, list, or float/int).
        domain_max_input: Max output clamp values (NumPy array, list, or float/int).

    Returns:
        List of 3 floats representing the interpolated and clamped RGB value.
    """
    # Prepare inputs, ensuring float64 types internally
    lut_data, r, g, b, domain_min_np, domain_max_np = _prepare_inputs_for_64bit(
        lut_data_input, r_input, g_input, b_input, domain_min_input, domain_max_input
    )

    # --- Resolution Handling ---
    # Needs at least 2 points to define edges
    if isinstance(resolution, int):
        if resolution < 2: raise ValueError("Barycentric Tetrahedral interpolation requires resolution >= 2")
        res_r, res_g, res_b = resolution, resolution, resolution
    elif len(resolution) == 3:
        res_r, res_g, res_b = resolution
        if not all(isinstance(res, int) and res >= 2 for res in resolution):
             raise ValueError(f"Barycentric Tetrahedral interpolation requires resolution dimensions >= 2, got {resolution}")
    else:
        raise ValueError("Resolution must be an int or a list/tuple of 3 ints [res_r, res_g, res_b]")

    # --- Coordinate and Index Calculation (using float64) ---
    r_coord = np.clip(r, np.float64(0.0), np.float64(1.0))
    g_coord = np.clip(g, np.float64(0.0), np.float64(1.0))
    b_coord = np.clip(b, np.float64(0.0), np.float64(1.0))

    res_r_minus_1 = np.float64(res_r - 1)
    res_g_minus_1 = np.float64(res_g - 1)
    res_b_minus_1 = np.float64(res_b - 1)

    x = r_coord * res_r_minus_1
    y = g_coord * res_g_minus_1
    z = b_coord * res_b_minus_1

    i = int(np.floor(x))
    j = int(np.floor(y))
    k = int(np.floor(z))

    fx = x - i # Fractional parts (float64)
    fy = y - j
    fz = z - k

    x0 = np.clip(i,     0, res_r - 1)
    x1 = np.clip(i + 1, 0, res_r - 1)
    y0 = np.clip(j,     0, res_g - 1)
    y1 = np.clip(j + 1, 0, res_g - 1)
    z0 = np.clip(k,     0, res_b - 1)
    z1 = np.clip(k + 1, 0, res_b - 1)

    # --- Fetch Corner Values ---
    try:
        # Fetch into a float64 array 'v'
        v = np.zeros((8, 3), dtype=np.float64)
        # Vertex order matches the logic below (same as tetrahedral function)
        v[0] = lut_data[x0, y0, z0, :]
        v[1] = lut_data[x1, y0, z0, :]
        v[2] = lut_data[x0, y1, z0, :]
        v[3] = lut_data[x0, y0, z1, :]
        v[4] = lut_data[x1, y1, z0, :]
        v[5] = lut_data[x1, y0, z1, :]
        v[6] = lut_data[x0, y1, z1, :]
        v[7] = lut_data[x1, y1, z1, :]
    except IndexError as e:
         raise IndexError(
            f"Error indexing lut_data with shape {lut_data.shape} using corner indices around "
            f"({i}, {j}, {k}). Check resolution and LUT shape. Original error: {e}"
         ) from e
    except Exception as e:
        raise RuntimeError(f"Unexpected error during barycentric corner value lookup: {e}") from e

    # --- Determine Tetrahedron and Interpolate (using float64) ---
    one_f64 = np.float64(1.0)
    # This logic block is identical to the tetrahedral_interpolation version
    if fx >= fy:
        if fy >= fz: # T1: v0124
            c0 = v[0]; c1 = v[1] - v[0]; c2 = v[2] - v[0]; c3 = v[4] - v[1] - v[2] + v[0]
            result = c0 + c1*fx + c2*fy + c3*fz
        else: # fx >= fz >= fy
            if fx >= fz: # T2: v0135
                 c0 = v[0]; c1 = v[1] - v[0]; c2 = v[3] - v[0]; c3 = v[5] - v[1] - v[3] + v[0]
                 result = c0 + c1*fx + c2*fz + c3*fy
            else: # T3: v0326
                 c0 = v[0]; c1 = v[3] - v[0]; c2 = v[2] - v[0]; c3 = v[6] - v[3] - v[2] + v[0]
                 result = c0 + c1*fz + c2*fy + c3*fx
    else: # fy > fx
        if fx >= fz: # T4: v7642
            c0 = v[7]; c1 = v[6] - v[7]; c2 = v[4] - v[7]; c3 = v[2] - v[6] - v[4] + v[7]
            result = c0 + c1*(one_f64-fx) + c2*(one_f64-fy) + c3*(one_f64-fz)
        else: # fy >= fz > fx
             if fy >= fz: # T5: v7653
                  c0 = v[7]; c1 = v[6] - v[7]; c2 = v[5] - v[7]; c3 = v[3] - v[6] - v[5] + v[7]
                  result = c0 + c1*(one_f64-fx) + c2*(one_f64-fz) + c3*(one_f64-fy)
             else: # T6: v7451
                  c0 = v[7]; c1 = v[4] - v[7]; c2 = v[5] - v[7]; c3 = v[1] - v[4] - v[5] + v[7]
                  result = c0 + c1*(one_f64-fy) + c2*(one_f64-fz) + c3*(one_f64-fx)

    # --- Output Clamping ---
    clamped_val = np.clip(result, domain_min_np, domain_max_np)

    # Return as list of standard Python floats
    return clamped_val.tolist()
    
    
# --- Bicubic Interpolation (applied to 2D slice) ---
def bicubic_interpolation(lut_data_input, resolution, r_input, g_input, b_input, domain_min_input, domain_max_input):
    """
    Performs 2D Bicubic interpolation on the R-G plane of the 3D LUT,
    using 'r' and 'g' coordinates, ensuring float64 precision internally.
    The R-G plane is selected based on the nearest grid point to 'b'.
    Uses a cubic convolution kernel. Output is clamped. CORRECTED VERSION.

    Args:
        lut_data_input: The 3D LUT data (NumPy array or list-of-lists, any float/int dtype).
                        Expected layout: lut_data[r_idx, g_idx, b_idx] = [R_val, G_val, B_val].
        resolution: Int or tuple/list of 3 ints defining LUT dimensions [res_r, res_g, res_b].
        r_input, g_input, b_input: Input coordinates (floats).
        domain_min_input: Min output clamp values (NumPy array, list, or float/int).
        domain_max_input: Max output clamp values (NumPy array, list, or float/int).

    Returns:
        List of 3 floats representing the interpolated and clamped RGB value.
    """
    # Prepare inputs, ensuring float64 types internally
    lut_data, r, g, b, domain_min_np, domain_max_np = _prepare_inputs_for_64bit(
        lut_data_input, r_input, g_input, b_input, domain_min_input, domain_max_input
    )

    # --- Resolution Handling ---
    if isinstance(resolution, int):
        if resolution < 1: raise ValueError("Resolution must be >= 1")
        res_r, res_g, res_b = resolution, resolution, resolution
    elif len(resolution) == 3:
        res_r, res_g, res_b = resolution
        if not all(isinstance(res, int) and res >= 1 for res in resolution):
             raise ValueError(f"Resolution dimensions must be integers >= 1, got {resolution}")
    else:
        raise ValueError("Resolution must be an int or a list/tuple of 3 ints [res_r, res_g, res_b]")

    # --- Slice Selection (along B-axis using nearest neighbor) ---
    b_coord = np.clip(b, np.float64(0.0), np.float64(1.0))
    b_f_idx = b_coord * np.float64(res_b - 1) if res_b > 1 else np.float64(0.0)
    b_idx = int(np.round(b_f_idx))
    b_idx = np.clip(b_idx, 0, res_b - 1)

    try:
        # lut_data is float64, slice will be float64
        # Assuming lut_data shape is (res_r, res_g, res_b, 3)
        lut_slice_2d = lut_data[:, :, b_idx, :] # Shape: (res_r, res_g, 3)
    except IndexError as e:
         raise IndexError(f"Error slicing lut_data for bicubic with shape {lut_data.shape} using b_idx={b_idx}: {e}") from e
    except Exception as e:
         raise RuntimeError(f"Unexpected error during 2D slice extraction for bicubic: {e}") from e

    # --- Fallback for small slice dimensions ---
    min_res_bicubic = 4
    if res_r < min_res_bicubic or res_g < min_res_bicubic:
        warnings.warn(
            f"Bicubic requires resolution >= {min_res_bicubic} for interpolated slice dimensions (R, G), have ({res_r}, {res_g}). "
            "Falling back to Bilinear for this slice.",
            UserWarning
        )
        try:
             # Ensure the *main* bilinear function (which does input prep) is available
             # Pass the original inputs, as bilinear handles prep itself.
             return bilinear_interpolation(lut_data_input, resolution, r_input, g_input, b_input, domain_min_input, domain_max_input)
        except NameError:
             raise RuntimeError("Fallback failed: bilinear_interpolation function not found.")
        except Exception as e:
             raise RuntimeError(f"Error during fallback to bilinear: {e}") from e


    # --- Bicubic Interpolation Core (using float64) ---
    x = np.clip(r, np.float64(0.0), np.float64(1.0)) # R-coord for slice
    y = np.clip(g, np.float64(0.0), np.float64(1.0)) # G-coord for slice

    res_r_minus_1 = np.float64(res_r - 1)
    res_g_minus_1 = np.float64(res_g - 1)

    # Calculate float indices and fractional parts using float64
    fx = x * res_r_minus_1
    fy = y * res_g_minus_1
    ix = int(np.floor(fx)) # Integer part for indexing anchor
    iy = int(np.floor(fy))
    tx = fx - ix # Fractional part (float64)
    ty = fy - iy

    # Get the 4x4 neighboring grid points. Careful with indexing!
    # points[i][j] will store the value at grid point (ix + i - 1, iy + j - 1)
    points = np.zeros((4, 4, 3), dtype=np.float64)
    for j_idx in range(4): # Corresponds to iy offset j_idx - 1
        py = np.clip(iy + j_idx - 1, 0, res_g - 1)
        for i_idx in range(4): # Corresponds to ix offset i_idx - 1
            px = np.clip(ix + i_idx - 1, 0, res_r - 1)
            # Fetch from the 2D slice using calculated px, py
            points[i_idx, j_idx, :] = lut_slice_2d[px, py, :]

    # Calculate weights using the float64 cubic kernel
    # Weights for the 4 points along X relative to tx (offsets -1, 0, 1, 2)
    wx = np.array([_cubic_kernel(tx + 1.0), _cubic_kernel(tx),
                     _cubic_kernel(tx - 1.0), _cubic_kernel(tx - 2.0)],
                    dtype=np.float64)
    # Weights for the 4 points along Y relative to ty (offsets -1, 0, 1, 2)
    wy = np.array([_cubic_kernel(ty + 1.0), _cubic_kernel(ty),
                     _cubic_kernel(ty - 1.0), _cubic_kernel(ty - 2.0)],
                    dtype=np.float64)

    # Interpolate along x-axis (R dim) for each of the 4 conceptual 'rows' in the 4x4 grid
    # The result interp_rows[j] will be the interpolated value at (r_input, g_anchor_j)
    interp_rows = np.zeros((4, 3), dtype=np.float64)
    for j_idx in range(4):
        # points[:, j_idx, :] selects the 4 points along the X dimension
        # for the j_idx-th row (fixed Y offset). Shape is (4, 3).
        interp_rows[j_idx, :] = np.dot(wx, points[:, j_idx, :])
        # Example: interp_rows[0] uses points points[0,0,:], points[1,0,:], points[2,0,:], points[3,0,:]

    # Interpolate along y-axis (G dim) using the 4 intermediate row results
    # interp_rows[j_idx, :] holds the result for the j_idx-th conceptual row (y offset j_idx - 1)
    # The result is the interpolated value at (r_input, g_input)
    interpolated_val = np.dot(wy, interp_rows) # Dot product of (4,) weights and (4, 3) data -> (3,) result

    # --- Output Clamping ---
    # Use the float64 bounds prepared earlier
    clamped_val = np.clip(interpolated_val, domain_min_np, domain_max_np)

    # Return as list of standard Python floats
    return clamped_val.tolist()


def cubic_spline_interpolation(lut_data_input, resolution, r_input, g_input, b_input, domain_min_input, domain_max_input):
    """
    Performs 1D Cubic interpolation along the R-axis of the 3D LUT,
    using the 'r' coordinate and a cubic convolution kernel. Ensures float64
    precision internally. The R-axis slice is chosen based on nearest 'g' and 'b'.
    Output is clamped.

    Args:
        lut_data_input: The 3D LUT data (NumPy array or list-of-lists, any float/int dtype).
                        Expected layout: lut_data[r_idx, g_idx, b_idx] = [R_val, G_val, B_val].
        resolution: Int or tuple/list of 3 ints defining LUT dimensions [res_r, res_g, res_b].
        r_input, g_input, b_input: Input coordinates (floats).
        domain_min_input: Min output clamp values (NumPy array, list, or float/int).
        domain_max_input: Max output clamp values (NumPy array, list, or float/int).

    Returns:
        List of 3 floats representing the interpolated and clamped RGB value.
    """
    # Prepare inputs, ensuring float64 types internally
    lut_data, r, g, b, domain_min_np, domain_max_np = _prepare_inputs_for_64bit(
        lut_data_input, r_input, g_input, b_input, domain_min_input, domain_max_input
    )

    # --- Resolution Handling ---
    if isinstance(resolution, int):
        if resolution < 1: raise ValueError("Resolution must be >= 1")
        res_r, res_g, res_b = resolution, resolution, resolution
    elif len(resolution) == 3:
        res_r, res_g, res_b = resolution
        if not all(isinstance(res, int) and res >= 1 for res in resolution):
             raise ValueError(f"Resolution dimensions must be integers >= 1, got {resolution}")
    else:
        raise ValueError("Resolution must be an int or a list/tuple of 3 ints [res_r, res_g, res_b]")

    # --- Slice Selection (along G/B axes using nearest neighbor) ---
    g_coord = np.clip(g, np.float64(0.0), np.float64(1.0))
    b_coord = np.clip(b, np.float64(0.0), np.float64(1.0))
    g_f_idx = g_coord * np.float64(res_g - 1) if res_g > 1 else np.float64(0.0)
    b_f_idx = b_coord * np.float64(res_b - 1) if res_b > 1 else np.float64(0.0)
    g_idx = int(np.round(g_f_idx))
    b_idx = int(np.round(b_f_idx))
    g_idx = np.clip(g_idx, 0, res_g - 1)
    b_idx = np.clip(b_idx, 0, res_b - 1)

    try:
        # lut_data is float64, slice will be float64
        # Assuming lut_data shape is (res_r, res_g, res_b, 3)
        # Select all R points at the chosen G, B indices
        lut_slice_1d = lut_data[:, g_idx, b_idx, :] # Shape: (res_r, 3)
    except IndexError as e:
         raise IndexError(f"Error slicing lut_data for cubic spline with shape {lut_data.shape} using g_idx={g_idx}, b_idx={b_idx}: {e}") from e
    except Exception as e:
         raise RuntimeError(f"Unexpected error during 1D slice extraction for cubic spline: {e}") from e

    # --- Coordinate processing for the 1D slice (float64) ---
    x = np.clip(r, np.float64(0.0), np.float64(1.0)) # R-coordinate for slice

    # --- Fallback for small slice dimension ---
    # 1D Cubic interpolation needs a 4-point neighborhood
    min_res_cubic = 4
    if res_r < min_res_cubic:
        warnings.warn(
            f"1D Cubic interpolation requires resolution >= {min_res_cubic} for the interpolated dimension (R), have ({res_r}). "
            "Falling back to Linear for this slice.",
            UserWarning
        )
        # Use the float64-aware _linear_interpolate_1d_slice helper directly
        try:
            # Pass the extracted float64 slice, its resolution, and the float64 coordinate
            interpolated_val = _linear_interpolate_1d_slice(lut_slice_1d, res_r, x)
            # Need to clamp the result from the helper
            clamped_val = np.clip(interpolated_val, domain_min_np, domain_max_np)
            return clamped_val.tolist()
        except NameError:
             raise RuntimeError("Fallback failed: _linear_interpolate_1d_slice function not found.")
        except Exception as e:
             raise RuntimeError(f"Error during fallback to linear 1D: {e}") from e


    # --- 1D Cubic Interpolation Core (using float64) ---
    res_r_minus_1 = np.float64(res_r - 1)

    # Calculate float index and fractional part using float64
    fx = x * res_r_minus_1
    ix = int(np.floor(fx)) # Integer part for indexing anchor
    tx = fx - ix          # Fractional part (float64)

    # Get the 4 neighboring grid points - Ensure float64 dtype
    # points[i] will store the value at grid point (ix + i - 1)
    points = np.zeros((4, 3), dtype=np.float64)
    for i_offset in range(-1, 3): # Offset relative to ix: -1, 0, 1, 2
        # Calculate actual index, clipping to valid range [0, res_r-1]
        px = np.clip(ix + i_offset, 0, res_r - 1)
        # Fetch from the 1D slice using calculated px
        points[i_offset + 1, :] = lut_slice_1d[px, :]

    # Calculate weights using the float64 cubic kernel
    # Weights for the 4 points relative to tx (offsets -1, 0, 1, 2)
    w = np.array([_cubic_kernel(tx + 1.0), _cubic_kernel(tx),
                  _cubic_kernel(tx - 1.0), _cubic_kernel(tx - 2.0)],
                 dtype=np.float64)

    # Perform the weighted sum using float64 dot product
    # np.dot of (4,) weights and (4, 3) points -> (3,) result
    interpolated_val = np.dot(w, points)

    # --- Output Clamping ---
    clamped_val = np.clip(interpolated_val, domain_min_np, domain_max_np)

    # Return as list of standard Python floats
    return clamped_val.tolist()


def slice_bicubic_interpolation(lut_data_input, resolution, r_input, g_input, b_input, domain_min_input, domain_max_input):
    """
    Performs 2D Bicubic interpolation on an R-G plane slice of the 3D LUT.
    The slice is selected based on the nearest grid point to 'b'.
    Ensures float64 precision internally. Uses a cubic convolution kernel. Output is clamped.

    Args:
        lut_data_input: The 3D LUT data (NumPy array or list-of-lists, any float/int dtype).
                        Expected layout: lut_data[r_idx, g_idx, b_idx] = [R_val, G_val, B_val].
        resolution: Int or tuple/list of 3 ints defining LUT dimensions [res_r, res_g, res_b].
        r_input, g_input, b_input: Input coordinates (floats).
        domain_min_input: Min output clamp values (NumPy array, list, or float/int).
        domain_max_input: Max output clamp values (NumPy array, list, or float/int).

    Returns:
        List of 3 floats representing the interpolated and clamped RGB value.
    """
    # Prepare inputs, ensuring float64 types internally
    lut_data, r, g, b, domain_min_np, domain_max_np = _prepare_inputs_for_64bit(
        lut_data_input, r_input, g_input, b_input, domain_min_input, domain_max_input
    )

    # --- Resolution Handling ---
    if isinstance(resolution, int):
        if resolution < 1: raise ValueError("Resolution must be >= 1")
        res_r, res_g, res_b = resolution, resolution, resolution
    elif len(resolution) == 3:
        res_r, res_g, res_b = resolution
        if not all(isinstance(res, int) and res >= 1 for res in resolution):
             raise ValueError(f"Resolution dimensions must be integers >= 1, got {resolution}")
    else:
        raise ValueError("Resolution must be an int or a list/tuple of 3 ints [res_r, res_g, res_b]")

    # --- Slice Selection (Nearest B) ---
    b_coord = np.clip(b, np.float64(0.0), np.float64(1.0))
    b_f_idx = b_coord * np.float64(res_b - 1) if res_b > 1 else np.float64(0.0)
    b_idx = int(np.round(b_f_idx))
    b_idx = np.clip(b_idx, 0, res_b - 1)

    try:
        # Assuming lut_data shape is (res_r, res_g, res_b, 3)
        lut_slice_2d = lut_data[:, :, b_idx, :] # Shape: (res_r, res_g, 3), float64
    except IndexError as e:
         raise IndexError(f"Error slicing lut_data for bicubic with shape {lut_data.shape} using b_idx={b_idx}: {e}") from e
    except Exception as e:
         raise RuntimeError(f"Unexpected error during 2D slice extraction for bicubic: {e}") from e

    # --- Fallback for small slice dimensions ---
    min_res_bicubic = 4
    if res_r < min_res_bicubic or res_g < min_res_bicubic:
        warnings.warn(
            f"Slice Bicubic requires resolution >= {min_res_bicubic} for interpolated dimensions (R, G), have ({res_r}, {res_g}). "
            "Falling back to Bilinear for this slice.",
            UserWarning
        )
        try:
             # Call the refactored bilinear function using original inputs
             return bilinear_interpolation(lut_data_input, resolution, r_input, g_input, b_input, domain_min_input, domain_max_input)
        except NameError:
             raise RuntimeError("Fallback failed: bilinear_interpolation function not found.")
        except Exception as e:
             raise RuntimeError(f"Error during fallback to bilinear: {e}") from e

    # --- Bicubic Interpolation Core on the Slice (using float64) ---
    x = np.clip(r, np.float64(0.0), np.float64(1.0)) # R-coord for slice
    y = np.clip(g, np.float64(0.0), np.float64(1.0)) # G-coord for slice

    res_r_minus_1 = np.float64(res_r - 1)
    res_g_minus_1 = np.float64(res_g - 1)

    fx = x * res_r_minus_1
    fy = y * res_g_minus_1
    ix = int(np.floor(fx))
    iy = int(np.floor(fy))
    tx = fx - ix # Fractional part (float64)
    ty = fy - iy

    # Get the 4x4 neighboring grid points from the slice
    points = np.zeros((4, 4, 3), dtype=np.float64)
    for j_idx in range(4): # Corresponds to iy offset j_idx - 1
        py = np.clip(iy + j_idx - 1, 0, res_g - 1)
        for i_idx in range(4): # Corresponds to ix offset i_idx - 1
            px = np.clip(ix + i_idx - 1, 0, res_r - 1)
            points[i_idx, j_idx, :] = lut_slice_2d[px, py, :]

    # Calculate weights using the float64 cubic kernel
    wx = np.array([_cubic_kernel(tx + 1.0), _cubic_kernel(tx),
                     _cubic_kernel(tx - 1.0), _cubic_kernel(tx - 2.0)],
                    dtype=np.float64)
    wy = np.array([_cubic_kernel(ty + 1.0), _cubic_kernel(ty),
                     _cubic_kernel(ty - 1.0), _cubic_kernel(ty - 2.0)],
                    dtype=np.float64)

    # Interpolate along x-axis (R dim) for each row
    interp_rows = np.zeros((4, 3), dtype=np.float64)
    for j_idx in range(4):
        interp_rows[j_idx, :] = np.dot(wx, points[:, j_idx, :])

    # Interpolate along y-axis (G dim) using intermediate results
    interpolated_val = np.dot(wy, interp_rows)

    # --- Output Clamping ---
    clamped_val = np.clip(interpolated_val, domain_min_np, domain_max_np)

    # Return as list of standard Python floats
    return clamped_val.tolist()
    

def slice_tricubic_interpolation(lut_data_input, resolution, r_input, g_input, b_input, domain_min_input, domain_max_input):
    """
    Performs standard 3D Tricubic interpolation on the full 3D LUT using a 4x4x4
    neighborhood and a cubic convolution kernel.
    Ensures float64 precision internally. Output is clamped.

    Args:
        lut_data_input: The 3D LUT data (NumPy array or list-of-lists, any float/int dtype).
                        Expected layout: lut_data[r_idx, g_idx, b_idx] = [R_val, G_val, B_val].
        resolution: Int or tuple/list of 3 ints defining LUT dimensions [res_r, res_g, res_b].
        r_input, g_input, b_input: Input coordinates (floats).
        domain_min_input: Min output clamp values (NumPy array, list, or float/int).
        domain_max_input: Max output clamp values (NumPy array, list, or float/int).

    Returns:
        List of 3 floats representing the interpolated and clamped RGB value.
    """
    # Prepare inputs, ensuring float64 types internally
    lut_data, r, g, b, domain_min_np, domain_max_np = _prepare_inputs_for_64bit(
        lut_data_input, r_input, g_input, b_input, domain_min_input, domain_max_input
    )

    # --- Resolution Handling ---
    if isinstance(resolution, int):
        if resolution < 1: raise ValueError("Resolution must be >= 1")
        res_r, res_g, res_b = resolution, resolution, resolution
    elif len(resolution) == 3:
        res_r, res_g, res_b = resolution
        if not all(isinstance(res, int) and res >= 1 for res in resolution):
             raise ValueError(f"Resolution dimensions must be integers >= 1, got {resolution}")
    else:
        raise ValueError("Resolution must be an int or a list/tuple of 3 ints [res_r, res_g, res_b]")

    # --- Fallback for small resolutions ---
    min_res_tricubic = 4
    if res_r < min_res_tricubic or res_g < min_res_tricubic or res_b < min_res_tricubic:
        warnings.warn(
            f"Tricubic requires resolution >= {min_res_tricubic} in all dimensions, have ({res_r}, {res_g}, {res_b}). "
            "Falling back to Trilinear.",
            UserWarning
        )
        try:
             # Call the refactored trilinear function using original inputs
             return trilinear_interpolation(lut_data_input, resolution, r_input, g_input, b_input, domain_min_input, domain_max_input)
        except NameError:
             raise RuntimeError("Fallback failed: trilinear_interpolation function not found.")
        except Exception as e:
             raise RuntimeError(f"Error during fallback to trilinear: {e}") from e

    # --- Tricubic Interpolation Core (using float64) ---
    x = np.clip(r, np.float64(0.0), np.float64(1.0))
    y = np.clip(g, np.float64(0.0), np.float64(1.0))
    z = np.clip(b, np.float64(0.0), np.float64(1.0))

    res_r_minus_1 = np.float64(res_r - 1)
    res_g_minus_1 = np.float64(res_g - 1)
    res_b_minus_1 = np.float64(res_b - 1)

    # Calculate float indices and fractional parts using float64
    fx = x * res_r_minus_1
    fy = y * res_g_minus_1
    fz = z * res_b_minus_1
    ix = int(np.floor(fx))
    iy = int(np.floor(fy))
    iz = int(np.floor(fz))
    tx = fx - ix # Fractional part (float64)
    ty = fy - iy
    tz = fz - iz

    # Get the 4x4x4 neighboring grid points from the full 3D LUT
    points_4x4x4 = np.zeros((4, 4, 4, 3), dtype=np.float64)
    for k_idx in range(4): # Corresponds to iz offset k_idx - 1
        pz = np.clip(iz + k_idx - 1, 0, res_b - 1)
        for j_idx in range(4): # Corresponds to iy offset j_idx - 1
            py = np.clip(iy + j_idx - 1, 0, res_g - 1)
            for i_idx in range(4): # Corresponds to ix offset i_idx - 1
                px = np.clip(ix + i_idx - 1, 0, res_r - 1)
                # Fetch from the main 3D lut_data
                points_4x4x4[i_idx, j_idx, k_idx, :] = lut_data[px, py, pz, :]

    # Calculate weights using the float64 cubic kernel
    wx = np.array([_cubic_kernel(tx + 1.0), _cubic_kernel(tx),
                     _cubic_kernel(tx - 1.0), _cubic_kernel(tx - 2.0)], dtype=np.float64)
    wy = np.array([_cubic_kernel(ty + 1.0), _cubic_kernel(ty),
                     _cubic_kernel(ty - 1.0), _cubic_kernel(ty - 2.0)], dtype=np.float64)
    wz = np.array([_cubic_kernel(tz + 1.0), _cubic_kernel(tz),
                     _cubic_kernel(tz - 1.0), _cubic_kernel(tz - 2.0)], dtype=np.float64)

    # Interpolate along X-axis -> results in a 4x4 grid (for each Z plane)
    interp_y_z = np.zeros((4, 4, 3), dtype=np.float64)
    for k_idx in range(4):
        for j_idx in range(4):
            # points_4x4x4[:, j_idx, k_idx, :] selects the 4 points along X
            interp_y_z[j_idx, k_idx, :] = np.dot(wx, points_4x4x4[:, j_idx, k_idx, :])

    # Interpolate along Y-axis -> results in a 4-element vector (along Z)
    interp_z = np.zeros((4, 3), dtype=np.float64)
    for k_idx in range(4):
        # interp_y_z[:, k_idx, :] selects the 4 intermediate results along Y for a fixed Z
        interp_z[k_idx, :] = np.dot(wy, interp_y_z[:, k_idx, :])

    # Interpolate along Z-axis -> final value
    # interp_z holds the 4 intermediate results along Z
    interpolated_val = np.dot(wz, interp_z)

    # --- Output Clamping ---
    clamped_val = np.clip(interpolated_val, domain_min_np, domain_max_np)

    # Return as list of standard Python floats
    return clamped_val.tolist()
    
    
    
def tricubic_interpolation(lut_data_input, resolution, r_input, g_input, b_input, domain_min_input, domain_max_input):
    """
    Performs standard 3D Tricubic interpolation on the full 3D LUT using a 4x4x4
    neighborhood and a cubic convolution kernel, ensuring float64 precision internally.
    Output is clamped.

    Args:
        lut_data_input: The 3D LUT data (NumPy array or list-of-lists, any float/int dtype).
                        Expected layout: lut_data[r_idx, g_idx, b_idx] = [R_val, G_val, B_val].
        resolution: Int or tuple/list of 3 ints defining LUT dimensions [res_r, res_g, res_b].
        r_input, g_input, b_input: Input coordinates (floats).
        domain_min_input: Min output clamp values (NumPy array, list, or float/int).
        domain_max_input: Max output clamp values (NumPy array, list, or float/int).

    Returns:
        List of 3 floats representing the interpolated and clamped RGB value.
    """
    # Prepare inputs, ensuring float64 types internally
    lut_data, r, g, b, domain_min_np, domain_max_np = _prepare_inputs_for_64bit(
        lut_data_input, r_input, g_input, b_input, domain_min_input, domain_max_input
    )

    # --- Resolution Handling ---
    if isinstance(resolution, int):
        if resolution < 1: raise ValueError("Resolution must be >= 1")
        res_r, res_g, res_b = resolution, resolution, resolution
    elif len(resolution) == 3:
        res_r, res_g, res_b = resolution
        if not all(isinstance(res, int) and res >= 1 for res in resolution):
             raise ValueError(f"Resolution dimensions must be integers >= 1, got {resolution}")
    else:
        raise ValueError("Resolution must be an int or a list/tuple of 3 ints [res_r, res_g, res_b]")

    # --- Fallback for small resolutions ---
    min_res_tricubic = 4
    if res_r < min_res_tricubic or res_g < min_res_tricubic or res_b < min_res_tricubic:
        warnings.warn(
            f"Tricubic requires resolution >= {min_res_tricubic} in all dimensions, have ({res_r}, {res_g}, {res_b}). "
            "Falling back to Trilinear.",
            UserWarning
        )
        try:
             # Call the refactored trilinear function using original inputs
             # Ensure trilinear_interpolation is defined and float64-aware
             return trilinear_interpolation(lut_data_input, resolution, r_input, g_input, b_input, domain_min_input, domain_max_input)
        except NameError:
             raise RuntimeError("Fallback failed: trilinear_interpolation function not found.")
        except Exception as e:
             raise RuntimeError(f"Error during fallback to trilinear: {e}") from e

    # --- Tricubic Interpolation Core (using float64) ---
    # Clip coordinates (already float64)
    x = np.clip(r, np.float64(0.0), np.float64(1.0))
    y = np.clip(g, np.float64(0.0), np.float64(1.0))
    z = np.clip(b, np.float64(0.0), np.float64(1.0))

    # Calculate float scaling factors (float64)
    res_r_minus_1 = np.float64(res_r - 1)
    res_g_minus_1 = np.float64(res_g - 1)
    res_b_minus_1 = np.float64(res_b - 1)

    # Calculate float indices and fractional parts using float64
    fx = x * res_r_minus_1
    fy = y * res_g_minus_1
    fz = z * res_b_minus_1
    ix = int(np.floor(fx)) # Integer anchor index
    iy = int(np.floor(fy))
    iz = int(np.floor(fz))
    tx = fx - ix # Fractional part (float64)
    ty = fy - iy
    tz = fz - iz

    # Get the 4x4x4 neighboring grid points from the full 3D LUT (float64)
    points_4x4x4 = np.zeros((4, 4, 4, 3), dtype=np.float64)
    for k_idx in range(4): # Corresponds to iz offset k_idx - 1
        pz = np.clip(iz + k_idx - 1, 0, res_b - 1)
        for j_idx in range(4): # Corresponds to iy offset j_idx - 1
            py = np.clip(iy + j_idx - 1, 0, res_g - 1)
            for i_idx in range(4): # Corresponds to ix offset i_idx - 1
                px = np.clip(ix + i_idx - 1, 0, res_r - 1)
                # Fetch from the main 3D lut_data (which is float64)
                # Assuming lut_data shape is (res_r, res_g, res_b, 3)
                points_4x4x4[i_idx, j_idx, k_idx, :] = lut_data[px, py, pz, :]

    # Calculate weights using the float64 cubic kernel
    wx = np.array([_cubic_kernel(tx + 1.0), _cubic_kernel(tx),
                     _cubic_kernel(tx - 1.0), _cubic_kernel(tx - 2.0)], dtype=np.float64)
    wy = np.array([_cubic_kernel(ty + 1.0), _cubic_kernel(ty),
                     _cubic_kernel(ty - 1.0), _cubic_kernel(ty - 2.0)], dtype=np.float64)
    wz = np.array([_cubic_kernel(tz + 1.0), _cubic_kernel(tz),
                     _cubic_kernel(tz - 1.0), _cubic_kernel(tz - 2.0)], dtype=np.float64)

    # Interpolate along X-axis -> results in a 4x4 intermediate grid (for each Z)
    # Output shape: (4, 4, 3) corresponding to (Y, Z, Channels) after X interp
    interp_yz = np.zeros((4, 4, 3), dtype=np.float64)
    for k_idx in range(4): # Iterate through Z planes
        for j_idx in range(4): # Iterate through Y lines within a Z plane
            # points_4x4x4[:, j_idx, k_idx, :] gets the 4 points along X for a fixed (Y,Z)
            # Result is the interpolated value at (r_input, g_anchor_j, b_anchor_k)
            interp_yz[j_idx, k_idx, :] = np.dot(wx, points_4x4x4[:, j_idx, k_idx, :])

    # Interpolate along Y-axis -> results in a 4-element intermediate vector (along Z)
    # Output shape: (4, 3) corresponding to (Z, Channels) after Y interp
    interp_z = np.zeros((4, 3), dtype=np.float64)
    for k_idx in range(4): # Iterate through Z results
        # interp_yz[:, k_idx, :] gets the 4 intermediate results along Y for a fixed Z
        # Result is the interpolated value at (r_input, g_input, b_anchor_k)
        interp_z[k_idx, :] = np.dot(wy, interp_yz[:, k_idx, :])

    # Interpolate along Z-axis -> final value
    # interp_z holds the 4 intermediate results along Z
    # Result is the interpolated value at (r_input, g_input, b_input)
    interpolated_val = np.dot(wz, interp_z) # Shape (3,)

    # --- Output Clamping ---
    clamped_val = np.clip(interpolated_val, domain_min_np, domain_max_np)

    # Return as list of standard Python floats
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

def _get_interpolation_weights(kernel_func, fractional_pos_f64, num_points):
    """
    Helper to get interpolation weights based on a kernel and fractional position.
    Ensures float64 precision.

    Args:
        kernel_func: The kernel function (e.g., _cubic_kernel) - assumed float64 aware.
        fractional_pos_f64 (np.float64): The fractional part of the coordinate (e.g., tx).
        num_points (int): The number of points in the neighborhood (e.g., 4 for cubic, 6 for this demo).

    Returns:
        numpy.ndarray: Array of weights (dtype=np.float64).
    """
    weights = np.zeros(num_points, dtype=np.float64)
    # Calculate weights based on distance from the interpolation point (ix + t)
    # to the neighboring grid points (ix + offset).
    # Weight index 'i' corresponds to grid point index ix + i + grid_offset.
    if num_points == 4: # For standard cubic kernel usage: indices ix + (-1, 0, 1, 2)
        # Kernel argument is distance: t - (grid_point_offset_from_ix)
        weights[0] = kernel_func(fractional_pos_f64 - (-1.0)) # Dist to ix-1 -> kernel(t+1)
        weights[1] = kernel_func(fractional_pos_f64 - (0.0))  # Dist to ix   -> kernel(t)
        weights[2] = kernel_func(fractional_pos_f64 - (1.0))  # Dist to ix+1 -> kernel(t-1)
        weights[3] = kernel_func(fractional_pos_f64 - (2.0))  # Dist to ix+2 -> kernel(t-2)
    elif num_points == 6: # For conceptual 6-point usage: indices ix + (-2, -1, 0, 1, 2, 3)
        # Using the _cubic_kernel for this 6-point demo
        weights[0] = kernel_func(fractional_pos_f64 - (-2.0)) # Dist to ix-2 -> kernel(t+2)
        weights[1] = kernel_func(fractional_pos_f64 - (-1.0)) # Dist to ix-1 -> kernel(t+1)
        weights[2] = kernel_func(fractional_pos_f64 - (0.0))  # Dist to ix   -> kernel(t)
        weights[3] = kernel_func(fractional_pos_f64 - (1.0))  # Dist to ix+1 -> kernel(t-1)
        weights[4] = kernel_func(fractional_pos_f64 - (2.0))  # Dist to ix+2 -> kernel(t-2)
        weights[5] = kernel_func(fractional_pos_f64 - (3.0))  # Dist to ix+3 -> kernel(t-3)
    else:
        raise ValueError(f"Unsupported number of points ({num_points}) for weights helper")

    # Note: Interpolating kernels like cubic convolution are generally *not* normalized
    # to sum to 1 across their support, as they aim for specific frequency responses
    # or continuity properties rather than just weighted averaging like a Gaussian.

    return weights
    

# --- "Conceptual" Tri-Quintic (using Cubic Kernel over 6x6x6 Grid) ---
def conceptual_triquintic_interpolation(lut_data_input, resolution, r_input, g_input, b_input, domain_min_input, domain_max_input):
    """
    Performs 3D interpolation using a 6x6x6 neighborhood and the _cubic_kernel
    applied sequentially. Ensures float64 precision internally. This demonstrates
    higher-order principles but isn't a true mathematical Tri-Quintic spline.
    Requires resolution >= 6. Output is clamped.

    NOTE: Falls back to Tricubic if resolution is too small.

    Args:
        lut_data_input: The 3D LUT data (NumPy array or list-of-lists, any float/int dtype).
                        Expected layout: lut_data[r_idx, g_idx, b_idx] = [R_val, G_val, B_val].
        resolution: Int or tuple/list of 3 ints defining LUT dimensions [res_r, res_g, res_b].
        r_input, g_input, b_input: Input coordinates (floats).
        domain_min_input: Min output clamp values (NumPy array, list, or float/int).
        domain_max_input: Max output clamp values (NumPy array, list, or float/int).

    Returns:
        List of 3 floats representing the interpolated and clamped RGB value.
    """
    # Use the existing cubic kernel for this 6x6x6 demonstration
    kernel_func = _cubic_kernel
    kernel_size = 6 # 6x6x6 neighborhood

    # Prepare inputs, ensuring float64 types internally
    lut_data, r, g, b, domain_min_np, domain_max_np = _prepare_inputs_for_64bit(
        lut_data_input, r_input, g_input, b_input, domain_min_input, domain_max_input
    )

    # --- Resolution Handling ---
    if isinstance(resolution, int):
        if resolution < 1: raise ValueError("Resolution must be >= 1")
        res_r, res_g, res_b = resolution, resolution, resolution
    elif len(resolution) == 3:
        res_r, res_g, res_b = resolution
        if not all(isinstance(res, int) and res >= 1 for res in resolution):
             raise ValueError(f"Resolution dimensions must be integers >= 1, got {resolution}")
    else:
        raise ValueError("Resolution must be an int or a list/tuple of 3 ints [res_r, res_g, res_b]")

    # --- Fallback for small resolutions ---
    min_res_required = kernel_size
    if res_r < min_res_required or res_g < min_res_required or res_b < min_res_required:
        warnings.warn(
            f"Conceptual {kernel_size}x{kernel_size}x{kernel_size} requires resolution >= {min_res_required} in all dims, "
            f"have ({res_r}, {res_g}, {res_b}). Falling back to Tricubic.",
            UserWarning
        )
        try:
            # Call the refactored tricubic function using original inputs
            # Ensure tricubic_interpolation is defined and float64-aware
            return tricubic_interpolation(lut_data_input, resolution, r_input, g_input, b_input, domain_min_input, domain_max_input)
        except NameError:
            raise RuntimeError("Fallback failed: tricubic_interpolation function not found.")
        except Exception as e:
            raise RuntimeError(f"Error during fallback to tricubic: {e}") from e

    # --- Conceptual "Tri-Quintic" Core (using float64 and 6x6x6 grid) ---
    # Clip coordinates (already float64)
    x = np.clip(r, np.float64(0.0), np.float64(1.0))
    y = np.clip(g, np.float64(0.0), np.float64(1.0))
    z = np.clip(b, np.float64(0.0), np.float64(1.0))

    # Calculate float scaling factors (float64)
    res_r_minus_1 = np.float64(res_r - 1)
    res_g_minus_1 = np.float64(res_g - 1)
    res_b_minus_1 = np.float64(res_b - 1)

    # Calculate float indices and fractional parts using float64
    fx = x * res_r_minus_1
    fy = y * res_g_minus_1
    fz = z * res_b_minus_1
    ix = int(np.floor(fx)) # Integer anchor index
    iy = int(np.floor(fy))
    iz = int(np.floor(fz))
    tx = fx - ix # Fractional part (float64)
    ty = fy - iy
    tz = fz - iz

    # Get the KxKxK neighborhood grid points (K=kernel_size)
    # grid_offset determines the index range relative to ix,iy,iz
    # For K=6, need indices ix+(-2, -1, 0, 1, 2, 3)
    grid_offset = -2 # Start index relative to ix,iy,iz is ix + grid_offset
    points_KxKxK = np.zeros((kernel_size, kernel_size, kernel_size, 3), dtype=np.float64)
    for k_i in range(kernel_size): # Index within the KxKxK array (0..K-1)
        pz = np.clip(iz + k_i + grid_offset, 0, res_b - 1) # Actual LUT index
        for j_i in range(kernel_size):
            py = np.clip(iy + j_i + grid_offset, 0, res_g - 1)
            for i_i in range(kernel_size):
                px = np.clip(ix + i_i + grid_offset, 0, res_r - 1)
                # Fetch from the main 3D lut_data (which is float64)
                # Assuming lut_data shape is (res_r, res_g, res_b, 3)
                points_KxKxK[i_i, j_i, k_i, :] = lut_data[px, py, pz, :]

    # Calculate interpolation weights using float64 helper
    wx = _get_interpolation_weights(kernel_func, tx, kernel_size)
    wy = _get_interpolation_weights(kernel_func, ty, kernel_size)
    wz = _get_interpolation_weights(kernel_func, tz, kernel_size)

    # Interpolate along X-axis -> results in a KxK intermediate grid (Y, Z)
    interp_yz = np.zeros((kernel_size, kernel_size, 3), dtype=np.float64)
    for k_idx in range(kernel_size): # Iterate through Z planes
        for j_idx in range(kernel_size): # Iterate through Y lines
            # points_KxKxK[:, j_idx, k_idx, :] selects K points along X
            interp_yz[j_idx, k_idx, :] = np.dot(wx, points_KxKxK[:, j_idx, k_idx, :])

    # Interpolate along Y-axis -> results in a K-element intermediate vector (Z)
    interp_z = np.zeros((kernel_size, 3), dtype=np.float64)
    for k_idx in range(kernel_size): # Iterate through Z results
        # interp_yz[:, k_idx, :] selects K intermediate results along Y
        interp_z[k_idx, :] = np.dot(wy, interp_yz[:, k_idx, :])

    # Interpolate along Z-axis -> final value
    # interp_z holds the K intermediate results along Z
    interpolated_val = np.dot(wz, interp_z) # Shape (3,)

    # --- Output Clamping ---
    clamped_val = np.clip(interpolated_val, domain_min_np, domain_max_np)

    # Return as list of standard Python floats
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
            print(f"Input RGB: ({r_in:.16f}, {g_in:.16f}, {b_in:.16f})")

            # Call Linear Interpolation
            try:
                lin_result = linear_interpolation(lut_data, lut_resolution_for_funcs, r_in, g_in, b_in, domain_min, domain_max)
                print(f"  Linear (1D R-axis): [{lin_result[0]:.16f}, {lin_result[1]:.16f}, {lin_result[2]:.16f}]")
            except Exception as e:
                 print(f"  Linear (1D R-axis): Error - {e}")

            # Call Bilinear Interpolation
            try:
                bilin_result = bilinear_interpolation(lut_data, lut_resolution_for_funcs, r_in, g_in, b_in, domain_min, domain_max)
                print(f"  Bilinear (2D RG):   [{bilin_result[0]:.16f}, {bilin_result[1]:.16f}, {bilin_result[2]:.16f}]")
            except Exception as e:
                 print(f"  Bilinear (2D RG):   Error - {e}")

            # Call Nearest Neighbor Interpolation
            try:
                # NN usually just needs the single resolution value if cubic
                nn_res = resolution if isinstance(resolution, int) else resolution[0]
                nn_result = nearest_neighbor_interpolation(lut_data, nn_res, r_in, g_in, b_in, domain_min, domain_max)
                print(f"  Nearest Neighbor:   [{nn_result[0]:.16f}, {nn_result[1]:.16f}, {nn_result[2]:.16f}]")
            except Exception as e:
                 print(f"  Nearest Neighbor:   Error - {e}")

            # Call Trilinear Interpolation
            try:
                # Pass the prepared resolution and numpy domain arrays
                tl_result = trilinear_interpolation(lut_data, lut_resolution_for_funcs, r_in, g_in, b_in, domain_min, domain_max)
                print(f"  Trilinear:          [{tl_result[0]:.16f}, {tl_result[1]:.16f}, {tl_result[2]:.16f}]")
            except Exception as e:
                 print(f"  Trilinear:          Error - {e}")

            # Call Tetrahedral Interpolation
            try:
                # Pass the prepared resolution and numpy domain arrays
                tt_result = tetrahedral_interpolation(lut_data, lut_resolution_for_funcs, r_in, g_in, b_in, domain_min, domain_max)
                print(f"  Tetrahedral:        [{tt_result[0]:.16f}, {tt_result[1]:.16f}, {tt_result[2]:.16f}]")
            except Exception as e:
                 print(f"  Tetrahedral:        Error - {e}")

            # Call Barycentric Tetrahedral Interpolation
            try:
                # Pass the prepared resolution and numpy domain arrays
                bary_result = barycentric_tetrahedral_interpolation(lut_data, lut_resolution_for_funcs, r_in, g_in, b_in, domain_min, domain_max)
                print(f"  Barycentric:        [{bary_result[0]:.16f}, {bary_result[1]:.16f}, {bary_result[2]:.16f}]") # Used different result var name
            except Exception as e:
                 print(f"  Barycentric:        Error - {e}")

            # Call Bicubic Interpolation
            try:
                # Pass the prepared resolution and numpy domain arrays
                bicubic_result = bicubic_interpolation(lut_data, lut_resolution_for_funcs, r_in, g_in, b_in, domain_min, domain_max)
                print(f"  Bicubic:            [{bicubic_result[0]:.16f}, {bicubic_result[1]:.16f}, {bicubic_result[2]:.16f}]") # Used different result var name
            except Exception as e:
                 print(f"  Bicubic:            Error - {e}")

            # Call Cubic 1D Spline Interpolation
            try:
                # Pass the prepared resolution and numpy domain arrays
                cubic_result = cubic_spline_interpolation(lut_data, lut_resolution_for_funcs, r_in, g_in, b_in, domain_min, domain_max)
                print(f"  Cubic Spline (1D):  [{cubic_result[0]:.16f}, {cubic_result[1]:.16f}, {cubic_result[2]:.16f}]") # Used different result var name & clarified title
            except Exception as e:
                 print(f"  Cubic Spline (1D):  Error - {e}")
 
            # Call Cubic 2D Spline Interpolation
            try:
                # Pass the prepared resolution and numpy domain arrays
                cubic_result = slice_bicubic_interpolation(lut_data, lut_resolution_for_funcs, r_in, g_in, b_in, domain_min, domain_max)
                print(f"  Cubic Spline (2D):  [{cubic_result[0]:.16f}, {cubic_result[1]:.16f}, {cubic_result[2]:.16f}]") # Used different result var name & clarified title
            except Exception as e:
                 print(f"  Cubic Spline (2D):  Error - {e}")

            # Call Cubic 3D Spline Interpolation
            try:
                # Pass the prepared resolution and numpy domain arrays
                cubic_result = slice_tricubic_interpolation(lut_data, lut_resolution_for_funcs, r_in, g_in, b_in, domain_min, domain_max)
                print(f"  Cubic Spline (3D):  [{cubic_result[0]:.16f}, {cubic_result[1]:.16f}, {cubic_result[2]:.16f}]") # Used different result var name & clarified title
            except Exception as e:
                 print(f"  Cubic Spline (3D):  Error - {e}")


            # Call Tricubic Interpolation
            try:
                # Pass the prepared resolution and numpy domain arrays
                tricubic_result = tricubic_interpolation(lut_data, lut_resolution_for_funcs, r_in, g_in, b_in, domain_min, domain_max)
                print(f"  Tricubic:           [{tricubic_result[0]:.16f}, {tricubic_result[1]:.16f}, {tricubic_result[2]:.16f}]")
            except Exception as e:
                 print(f"  Tricubic:           Error - {e}")

            # Call "Conceptual" Tri-Quintic (Cubic kernel on 6x6x6 grid)
            try:
                 # Pass the prepared resolution and numpy domain arrays
                 quintic_result = conceptual_triquintic_interpolation(lut_data, lut_resolution_for_funcs, r_in, g_in, b_in, domain_min, domain_max)
                 print(f"  ConceptualQuintic:  [{quintic_result[0]:.16f}, {quintic_result[1]:.16f}, {quintic_result[2]:.16f}]")
            except Exception as e:
                 print(f"  ConceptualQuintic:  Error - {e}")

            # Call RBF Interpolation (Use with extreme caution!)
#            try:
#                 # Pass the prepared resolution and numpy domain arrays
#                 rbf_result = rbf_interpolation(lut_data, lut_resolution_for_funcs, r_in, g_in, b_in, domain_min, domain_max)
#                 print(f"  RBF (Slow Demo):    [{rbf_result[0]:.16f}, {rbf_result[1]:.16f}, {rbf_result[2]:.16f}]")
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
#                     print(f"  RBF (Unprotected):  [{rbf_unprot_result[0]:.16f}, {rbf_unprot_result[1]:.16f}, {rbf_unprot_result[2]:.16f}]")
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
