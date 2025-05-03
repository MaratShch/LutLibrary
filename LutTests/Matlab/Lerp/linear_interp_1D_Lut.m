function result_rgb = linear_interp_1D_Lut(lut_data_1d, lut_size_1d, r_in, g_in, b_in, domain_min_lut, domain_max_lut, output_clamp_min, output_clamp_max)
%APPLY_LUT_1D_SINGLE_PIXEL Applies a 1D LUT to a single RGB pixel using LERP.
%   Performs independent linear interpolation for each channel based on its
%   input value and the corresponding output curve in the LUT.
%
%   Args:
%       lut_data_1d (double array): The Nx3 matrix of 1D LUT output data.
%                                   Column 1=R', Column 2=G', Column 3=B'.
%       lut_size_1d (double):       The size N of the 1D LUT (number of rows).
%       r_in (double):              Input Red component (expected range defined by domain_min/max_lut).
%       g_in (double):              Input Green component (expected range defined by domain_min/max_lut).
%       b_in (double):              Input Blue component (expected range defined by domain_min/max_lut).
%       domain_min_lut (double array): 1x3 vector [Rmin Gmin Bmin] defining the LUT's input domain lower bound (Optional, default [0 0 0]).
%       domain_max_lut (double array): 1x3 vector [Rmax Gmax Bmax] defining the LUT's input domain upper bound (Optional, default [1 1 1]).
%       output_clamp_min (double array): 1x3 vector defining the minimum value for the clamped output (Optional, default [0 0 0]).
%       output_clamp_max (double array): 1x3 vector defining the maximum value for the clamped output (Optional, default [1 1 1]).
%
%   Returns:
%       double array: Interpolated and clamped output RGB value [R, G, B] (1x3 row vector).

    % --- Input Validation and Defaults ---
    if nargin < 5
        error('Usage: apply_lut_1d_single_pixel(lut_data_1d, lut_size_1d, r_in, g_in, b_in, [domain_min_lut], [domain_max_lut], [output_clamp_min], [output_clamp_max])');
    end
    if ~isnumeric(lut_data_1d) || size(lut_data_1d, 2) ~= 3 || ~isscalar(lut_size_1d) || lut_size_1d < 2 || size(lut_data_1d, 1) ~= lut_size_1d
        error('Invalid lut_data_1d or lut_size_1d. LUT data must be Nx3, N >= 2.');
    end
     if ~isscalar(r_in) || ~isscalar(g_in) || ~isscalar(b_in)
         error('Input r_in, g_in, b_in must be scalar values.');
     end
    if nargin < 6 || isempty(domain_min_lut)
        domain_min_lut = [0.0, 0.0, 0.0];
    end
    if nargin < 7 || isempty(domain_max_lut)
        domain_max_lut = [1.0, 1.0, 1.0];
    end
    if nargin < 8 || isempty(output_clamp_min)
        output_clamp_min = [0.0, 0.0, 0.0];
    end
    if nargin < 9 || isempty(output_clamp_max)
        output_clamp_max = [1.0, 1.0, 1.0];
    end
    if ~isequal(size(domain_min_lut), [1, 3]) || ~isequal(size(domain_max_lut), [1, 3]) ...
       || ~isequal(size(output_clamp_min), [1, 3]) || ~isequal(size(output_clamp_max), [1, 3])
         error('Domain and clamp min/max arguments must be 1x3 vectors.');
    end

    input_rgb = [r_in, g_in, b_in];
    result_rgb = zeros(1, 3); % Preallocate output

    % Calculate input domain range, handle division by zero
    domain_range_lut = domain_max_lut - domain_min_lut;
    domain_range_lut(domain_range_lut <= 0) = 1.0; % Avoid division by zero

    % --- Process Each Channel Independently ---
    for channel = 1:3
        % 1. Normalize the specific input channel value based on LUT domain
        norm_input_channel = (input_rgb(channel) - domain_min_lut(channel)) / domain_range_lut(channel);

        % 2. Clamp normalized input to [0, 1] range
        norm_input_clamped = max(0.0, min(1.0, norm_input_channel));

        % 3. Calculate floating point index in the 0 to N-1 range
        float_idx_0based = norm_input_clamped * (lut_size_1d - 1);

        % 4. Find the surrounding integer indices (0-based)
        % Ensure idx0 doesn't exceed N-2 so idx1 is always valid
        idx0_0based = min(floor(float_idx_0based), lut_size_1d - 2);
        idx1_0based = idx0_0based + 1;

        % 5. Calculate the interpolation fraction (weight for idx1)
         % Handle the case where the index lands exactly on a point
        if idx0_0based == float_idx_0based % Avoids potential tiny fraction if floor==value
             fraction = 0.0;
        else
            fraction = float_idx_0based - idx0_0based;
        end
        % Clamp fraction just in case
        fraction = max(0.0, min(1.0, fraction));

        % 6. Convert to 1-based MATLAB indices for table lookup
        idx0_1based = idx0_0based + 1;
        idx1_1based = idx1_0based + 1;

        % 7. Get the specific LUT data column for the current channel's *output*
        lut_output_column = lut_data_1d(:, channel);

        % 8. Look up the output values from the LUT table at the surrounding indices
        output_val_idx0 = lut_output_column(idx0_1based);
        output_val_idx1 = lut_output_column(idx1_1based);

        % 9. Perform Linear Interpolation (LERP) for the current channel
        % output = val0 * (1 - frac) + val1 * frac
        result_rgb(channel) = output_val_idx0 * (1.0 - fraction) + output_val_idx1 * fraction;

    end % End channel loop

    % --- Clamp final output ---
    result_rgb = max(output_clamp_min, min(output_clamp_max, result_rgb));

end