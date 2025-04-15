function [lut_table, resolutions, domain_min, domain_max, title_str] = parse_cube_lut(lut_filepath)
%PARSE_CUBE_LUT Loads and parses a CUBE 3D LUT file.
%   Corrected version using simple counter for data lines.
%
%   Args:
%       lut_filepath (string): Path to the .cube file.
%
%   Returns:
%       lut_table (double array): 3D LUT data (R_dim x G_dim x B_dim x 3).
%       resolutions (double array): Vector [res_r, res_g, res_b].
%       domain_min (double array): Vector [min_r, min_g, min_b].
%       domain_max (double array): Vector [max_r, max_g, max_b].
%       title_str (string): Title extracted from the LUT file.

    if ~isfile(lut_filepath)
        error('parse_cube_lut:FileNotFound', 'LUT file not found: %s', lut_filepath);
    end

    lut_size = -1;
    domain_min = [0, 0, 0];
    domain_max = [1, 1, 1];
    title_str = '';
    lut_data_linear = []; % Will be preallocated after size is known
    found_size_line = false;
    expected_points = 0; % Initialize expected points
    
    fid = fopen(lut_filepath, 'rt');
    if fid == -1
        error('parse_cube_lut:FileOpenError', 'Cannot open LUT file: %s', lut_filepath);
    end
    cleaner = onCleanup(@() fclose(fid)); 

    line_num = 0;
    current_data_idx = 1; % Initialize the data counter *before* the loop

    while ~feof(fid)
        line = strtrim(fgetl(fid));
        line_num = line_num + 1;
        
        if isempty(line) || startsWith(line, '#')
            continue; 
        end
        
        parts = strsplit(line);
        
        % --- Check if it's a known header keyword ---
        is_header = false;
        if ~found_size_line || numel(parts) ~= 3 % Assume header if not size found OR not 3 numeric parts
            keyword = upper(parts{1});
             if any(strcmp(keyword, {'TITLE', 'LUT_3D_SIZE', 'DOMAIN_MIN', 'DOMAIN_MAX'}))
                 is_header = true;
             end
        end
            
        % --- Header Parsing ---
        if is_header 
            keyword = upper(parts{1}); % Process known headers
            if strcmp(keyword, 'TITLE')
                 if numel(parts) > 1; title_str = strjoin(parts(2:end)); else; title_str = ''; end
            elseif strcmp(keyword, 'LUT_3D_SIZE')
                if numel(parts) == 2
                    val = str2double(parts{2});
                    if ~isnan(val) && val > 1 && round(val)==val
                         if lut_size > 0; warning('parse_cube_lut:DuplicateSize', 'Duplicate LUT_3D_SIZE on line %d. Using first.', line_num); continue; end
                         lut_size = round(val);
                         expected_points = lut_size^3; % Calculate expected points
                         lut_data_linear = zeros(expected_points, 3, 'double'); % Preallocate now
                         fprintf('Found LUT Size: %d\n', lut_size);
                         found_size_line = true;
                    else
                        warning('parse_cube_lut:InvalidSize', 'Invalid LUT_3D_SIZE value on line %d: "%s"', line_num, parts{2});
                    end
                else
                     warning('parse_cube_lut:InvalidSizeFormat', 'Invalid LUT_3D_SIZE format on line %d: "%s"', line_num, line);
                end
            elseif strcmp(keyword, 'DOMAIN_MIN') % Process Domain lines anytime
                 if numel(parts) == 4
                    try domain_min = cellfun(@str2double, parts(2:4)); catch; warning('parse_cube_lut:InvalidDomainMin', 'Invalid DOMAIN_MIN on line %d', line_num); domain_min = [0,0,0]; end
                 else; warning('parse_cube_lut:InvalidDomainMinFormat', 'Invalid DOMAIN_MIN format on line %d', line_num); end
            elseif strcmp(keyword, 'DOMAIN_MAX')
                 if numel(parts) == 4
                    try domain_max = cellfun(@str2double, parts(2:4)); catch; warning('parse_cube_lut:InvalidDomainMax', 'Invalid DOMAIN_MAX on line %d', line_num); domain_max = [1,1,1]; end
                 else; warning('parse_cube_lut:InvalidDomainMaxFormat', 'Invalid DOMAIN_MAX format on line %d', line_num); end
            else % Unknown header format
                  warning('parse_cube_lut:UnknownHeader', 'Ignoring unknown line format in header section on line %d: "%s"', line_num, line);
            end
             
        % --- Data Line Parsing ---
        elseif found_size_line && numel(parts) == 3 % Should be data *after* size found
             % Check if index is still within bounds
             if current_data_idx > expected_points
                 warning('parse_cube_lut:TooMuchData', 'Found more data points than expected on line %d. Stopping data read.', line_num);
                 % Optional: break the loop or just keep reading and issue one error at end
                 continue; % Just ignore extra lines for now
             end
             
             try
                 % Convert and store data using the simple counter
                 lut_data_linear(current_data_idx, :) = cellfun(@str2double, parts);
                 current_data_idx = current_data_idx + 1; % Increment counter *after* success
             catch ME
                 warning('parse_cube_lut:InvalidData', 'Skipping invalid numeric data on line %d: "%s". Error: %s', line_num, line, ME.message);
             end
        elseif ~found_size_line && numel(parts) == 3
            warning('parse_cube_lut:DataBeforeSize', 'Encountered data-like line before LUT_3D_SIZE on line %d. Ignoring: "%s"', line_num, line);
        else % Unknown format line after size supposedly found
             warning('parse_cube_lut:UnknownLine', 'Ignoring unknown line format after header on line %d: "%s"', line_num, line);
        end
    end
    % File is automatically closed by onCleanup object
    
    % Final checks after loop
    if lut_size <= 0
        error('parse_cube_lut:SizeNotFound', 'LUT_3D_SIZE not found or invalid in file.');
    end
    
    % Use final value of counter to check if enough points were parsed
    if current_data_idx - 1 ~= expected_points
         error('parse_cube_lut:DataCountMismatch', 'Number of parsed data points (%d) does not match expected size (%d = %d^3)', ...
               current_data_idx - 1, expected_points, lut_size);
    end
    
    % --- Reshape the data ---
    lut_table = zeros(lut_size, lut_size, lut_size, 3, 'double');
    resolutions = [lut_size, lut_size, lut_size]; 
    
    idx = 1; % Reset index for reshaping
    for b = 1:lut_size 
        for g = 1:lut_size
            for r = 1:lut_size
                % Data read linearly from lut_data_linear
                lut_table(r, g, b, 1:3) = lut_data_linear(idx, :);
                idx = idx + 1;
            end
        end
    end
    
    fprintf('LUT parsing complete for "%s".\n', title_str);

end