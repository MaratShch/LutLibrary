% =============================================
% Main Script to Load LUT and Interpolate
% =============================================
clear; clc;

% --- Configuration ---
lut_filepath = '../../Cube/3D/MagicHour.cube'; 
input_r = 0.4791666666666666;
input_g = 0.40;
input_b = 0.70;

fprintf('--- Starting LUT Interpolation Test ---\n');
fprintf('Input RGB: [%.8f, %.8f, %.8f]\n', input_r, input_g, input_b);

% --- Load LUT using the dedicated function ---
try
    fprintf('Loading LUT: %s...\n', lut_filepath);
    [lut_table, resolutions, domain_min, domain_max, title_str, is3D] = parse_cube_lut(lut_filepath);
    fprintf('Successfully loaded "%s". Resolution: %dx%dx%d\n', ...
            title_str, resolutions(1), resolutions(2), resolutions(3));
    fprintf('Output domain: MIN=[%.4f,%.4f,%.4f], MAX=[%.4f,%.4f,%.4f]\n', ...
             domain_min, domain_max);
    if is3D == true     
        fprintf('This is 3D LUT file');
    else
        fprintf('This is 1D LUT file');
    end
    
    lut_loaded_ok = true;
catch ME % Catch errors from parse_cube_lut
    fprintf('\nError loading LUT file:\n');
    fprintf('%s\n', ME.message);
    % fprintf('%s\n', getReport(ME)); % Uncomment for full stack trace
    lut_loaded_ok = false;
end

% --- Perform Interpolation if LUT loaded ---
if lut_loaded_ok
    try
        fprintf('\nPerforming 1D Slice Linear Interpolation...\n');
        input_point = [input_r, input_g, input_b];

        % Call the specific LERP function
        output_rgb_lerp = linear_interp_3D_Lut(lut_table, resolutions, ...
                                           input_point(1), input_point(2), input_point(3), ...
                                           domain_min, domain_max);

        fprintf('Interpolation Result:\n');
        fprintf('  Output RGB: [%.16f, %.16f, %.16f]\n', ...
                output_rgb_lerp(1), output_rgb_lerp(2), output_rgb_lerp(3));

    catch ME
        fprintf('\nError during interpolation:\n');
        fprintf('%s\n', ME.message);
        % fprintf('%s\n', getReport(ME)); % Uncomment for full stack trace
    end
else
     fprintf('\nSkipping interpolation due to LUT loading error.\n');
end

fprintf('--- Test Finished ---\n');
