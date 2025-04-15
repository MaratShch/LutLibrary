function print_lut_entry(lut_table, r_idx_0based, g_idx_0based, b_idx_0based)
%PRINT_LUT_ENTRY Retrieves and prints the RGB value at specified 0-based indices.
%   Handles index conversion and boundary checks. Assumes lut_table data is
%   indexed as lut_table(R_index, G_index, B_index, Channel_index).
%
%   Args:
%       lut_table (double array): The loaded 3D LUT data (R x G x B x 3).
%       r_idx_0based (integer): 0-based Red index.
%       g_idx_0based (integer): 0-based Green index.
%       b_idx_0based (integer): 0-based Blue index.

    % Get dimensions from the LUT table
    if ndims(lut_table) ~= 4 || size(lut_table, 4) ~= 3
        error('print_lut_entry:InvalidInput', 'Input lut_table must be a 4D array with 3 channels in the last dimension.');
    end
    res_r = size(lut_table, 1);
    res_g = size(lut_table, 2);
    res_b = size(lut_table, 3);

    % Convert 0-based C++ style indices to 1-based MATLAB indices
    matlab_r_idx = r_idx_0based + 1;
    matlab_g_idx = g_idx_0based + 1;
    matlab_b_idx = b_idx_0based + 1;

    fprintf('--- Querying LUT Entry ---\n');
    fprintf('Target (0-based): R=%d, G=%d, B=%d\n', r_idx_0based, g_idx_0based, b_idx_0based);
    fprintf('MATLAB Index (1-based): (%d, %d, %d)\n', matlab_r_idx, matlab_g_idx, matlab_b_idx);

    % Check if calculated 1-based indices are within MATLAB's valid bounds
    if matlab_r_idx >= 1 && matlab_r_idx <= res_r && ...
       matlab_g_idx >= 1 && matlab_g_idx <= res_g && ...
       matlab_b_idx >= 1 && matlab_b_idx <= res_b

        % Access the RGB triplet using MATLAB indexing
        % lut_table(R_index, G_index, B_index, all_channels)
        rgb_value_at_index = squeeze(lut_table(matlab_r_idx, matlab_g_idx, matlab_b_idx, :)); 
        
        % Ensure it's a row vector (1x3) for consistent formatting below
        if size(rgb_value_at_index, 1) > 1
            rgb_value_at_index = rgb_value_at_index'; 
        end
        
        % --- Print with High Precision ---
        % Use format specifier '%.16f' for fixed-point or '%.17g' for general format
        % 'g' often represents doubles more accurately without excessive trailing zeros
        fprintf('Value Found:      [%.17g, %.17g, %.17g]\n', ...
                 rgb_value_at_index(1), rgb_value_at_index(2), rgb_value_at_index(3));

    else
        fprintf('Error: Indices (0-based: %d,%d,%d -> 1-based: %d,%d,%d) are OUT OF BOUNDS for LUT size (%d x %d x %d).\n', ...
                 r_idx_0based, g_idx_0based, b_idx_0based, ...
                 matlab_r_idx, matlab_g_idx, matlab_b_idx, ...
                 res_r, res_g, res_b);
    end
    fprintf('------------------------\n');

end % function
