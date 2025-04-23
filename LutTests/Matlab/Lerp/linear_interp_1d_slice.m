function result_rgb = linear_interp_1d_slice(lut_table, resolutions, r, g, b, domain_min, domain_max)
%LINEAR_INTERP_1D_SLICE Performs 1D linear interpolation along R-axis of 3D LUT.
%   Selects slice based on nearest G, B index. Clamps output.
%   Assumes lut_table indexing is [R_idx, G_idx, B_idx, Channel].
%
%   Args:
%       lut_table (double array): 3D LUT data (R_dim x G_dim x B_dim x 3).
%       resolutions (double array): Vector [res_r, res_g, res_b].
%       r (double): Normalized R coordinate (0.0 to 1.0) for interpolation.
%       g (double): Normalized G coordinate (0.0 to 1.0) for slice selection.
%       b (double): Normalized B coordinate (0.0 to 1.0) for slice selection.
%       domain_min (double array): Lower bound for output clamping [min_r, min_g, min_b].
%       domain_max (double array): Upper bound for output clamping [max_r, max_g, max_b].
%
%   Returns:
%       double array: Interpolated and clamped RGB value [R, G, B].

    res_r = resolutions(1);
    res_g = resolutions(2);
    res_b = resolutions(3);

    % --- Slice Selection (Indices are 1-based in MATLAB) ---
    g_coord_norm = max(0.0, min(1.0, g));
    b_coord_norm = max(0.0, min(1.0, b));
    
    g_idx = 1; 
    if res_g > 1; g_idx = round(g_coord_norm * (res_g - 1)) + 1; end
    
    b_idx = 1;
    if res_b > 1; b_idx = round(b_coord_norm * (res_b - 1)) + 1; end

    g_idx = max(1, min(res_g, g_idx));
    b_idx = max(1, min(res_b, b_idx));

    % --- 1D Linear Interpolation along R axis ---
    r_coord_norm = max(0.0, min(1.0, r));

    if res_r <= 1
        result_rgb = squeeze(lut_table(1, g_idx, b_idx, :)); % Use R=1 index
    else
        % Calculate floating point 1-based index along R
        r_f_index = r_coord_norm * (res_r - 1) + 1;

        r_idx0 = floor(r_f_index);
        r_idx1 = ceil(r_f_index);
        r_idx0 = max(1, min(res_r, r_idx0));
        r_idx1 = max(1, min(res_r, r_idx1));

        % Calculate interpolation factor 't' (0 to 1)
        % Handle the case where the index lands exactly on a point
        if r_idx0 == r_idx1
             t = 0.0;
        else
            % Fractional distance between index 0 and target index
            t = r_f_index - r_idx0; 
        end
        % Clamp t just in case (shouldn't be needed if logic above is perfect)
        t = max(0.0, min(1.0, t)); 


        % Get the two RGB points (use squeeze to get 3x1 vectors)
        p0 = squeeze(lut_table(r_idx0, g_idx, b_idx, :)); 
        p1 = squeeze(lut_table(r_idx1, g_idx, b_idx, :)); 

        % Perform the linear interpolation component-wise
        result_rgb = p0 * (1.0 - t) + p1 * t;
    end

    % --- Clamp final output ---
    % Ensure domain arrays are treated as row or column consistent with result_rgb
    if size(domain_min,2)>1; domain_min = domain_min'; end % Make column
    if size(domain_max,2)>1; domain_max = domain_max'; end % Make column
    if size(result_rgb,2)>1; result_rgb = result_rgb'; end % Make column for consistency

    result_rgb = max(domain_min, min(domain_max, result_rgb));

    % Ensure result is a 1x3 row vector for standard output
    if size(result_rgb, 1) > 1
        result_rgb = result_rgb'; 
    end
end
