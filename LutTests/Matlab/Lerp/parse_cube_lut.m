function [lut_table, resolutions, domain_min, domain_max, title_str, is3D] = parse_cube_lut(filename)
%PARSE_CUBE_LUT Parses a 1D or 3D CUBE (.cube) LUT file.
%
% Args:
%   filename (string): Path to the .cube file.
%
% Returns:
%   lut_table (double array): LUT data.
%                               If 3D: NxNxNx3 array.
%                               If 1D: Nx3 array.
%   resolutions (double array): Vector of LUT dimensions.
%                               If 3D: [N, N, N].
%                               If 1D: [N].
%   domain_min (double array): 1x3 vector [Rmin Gmin Bmin] for input domain lower bound.
%   domain_max (double array): 1x3 vector [Rmax Gmax Bmax] for input domain upper bound.
%   title_str (string):        Contents of the TITLE field, if present.
%   is3D (logical):            True if the parsed LUT is 3D, False if 1D.

    % --- Initialize Outputs ---
    lut_table = [];
    resolutions = [];
    domain_min = [0.0, 0.0, 0.0]; % Default
    domain_max = [1.0, 1.0, 1.0]; % Default
    title_str = '';
    is3D = []; % Initialize as empty to detect if size keyword was found

    lut_size = []; % Will store N from LUT_xD_SIZE

    fid = fopen(filename, 'rt'); % Open in text mode, handle line endings
    if fid < 0
        error('Cannot open file: %s', filename);
    end
    % Use onCleanup to ensure file is closed even if errors occur
    cleanupObj = onCleanup(@() fclose(fid));

    in_header = true;
    line_num = 0;
    temp_lut_data = []; % Temporary storage for flat data
    expected_data_points = 0;
    data_points_read = 0;

    while ~feof(fid)
        line = fgetl(fid);
        line_num = line_num + 1;

        if line == -1 % End of file check within loop as well
            break;
        end

        line = strtrim(line); % Remove leading/trailing whitespace

        if isempty(line) || line(1) == '#' % Skip empty lines and comments
             % Check for special DaVinci Resolve Blob marker WITHIN comments
             if startsWith(line, '#BEGIN METADATA BLOB', 'IgnoreCase', true) && ~in_header
                 % If we found blob marker after header, stop reading data points
                 fprintf('Info: Found metadata blob marker at line %d. Stopping data read.\n', line_num);
                 break; % Exit the while loop
             end
             continue;
        end

        % --- Parse Header ---
        if in_header
            if startsWith(line, 'TITLE', 'IgnoreCase', true)
                match = regexp(line, 'TITLE\s*"(.*?)"', 'tokens', 'ignorecase');
                if ~isempty(match)
                    title_str = match{1}{1};
                else
                     warning('Line %d: Malformed TITLE line: %s', line_num, line);
                end

            elseif startsWith(line, 'LUT_1D_SIZE', 'IgnoreCase', true)
                if ~isempty(is3D) % Already found a size keyword
                     error('Line %d: Found both LUT_1D_SIZE and LUT_3D_SIZE keywords.', line_num);
                end
                is3D = false; % Set flag for 1D
                val = sscanf(line(length('LUT_1D_SIZE')+1:end), '%d');
                if isempty(val) || val < 2
                    error('Line %d: Invalid or missing size for LUT_1D_SIZE.', line_num);
                end
                lut_size = val;
                expected_data_points = lut_size;
                temp_lut_data = zeros(expected_data_points, 3); % Preallocate temp storage

            elseif startsWith(line, 'LUT_3D_SIZE', 'IgnoreCase', true)
                 if ~isempty(is3D) % Already found a size keyword
                     error('Line %d: Found both LUT_1D_SIZE and LUT_3D_SIZE keywords.', line_num);
                 end
                 is3D = true; % Set flag for 3D
                 val = sscanf(line(length('LUT_3D_SIZE')+1:end), '%d');
                 if isempty(val) || val < 2
                    error('Line %d: Invalid or missing size for LUT_3D_SIZE.', line_num);
                 end
                 lut_size = val;
                 expected_data_points = lut_size^3;
                 temp_lut_data = zeros(expected_data_points, 3); % Preallocate temp storage

            elseif startsWith(line, 'DOMAIN_MIN', 'IgnoreCase', true)
                vals = sscanf(line(length('DOMAIN_MIN')+1:end), '%f %f %f');
                if length(vals) == 3
                    domain_min = vals'; % Transpose to 1x3
                else
                    warning('Line %d: Malformed DOMAIN_MIN line: %s', line_num, line);
                end

             elseif startsWith(line, 'DOMAIN_MAX', 'IgnoreCase', true)
                 vals = sscanf(line(length('DOMAIN_MAX')+1:end), '%f %f %f');
                 if length(vals) == 3
                    domain_max = vals'; % Transpose to 1x3
                 else
                    warning('Line %d: Malformed DOMAIN_MAX line: %s', line_num, line);
                 end

            else
                % First non-header, non-comment, non-empty line - assume it's data
                in_header = false;

                % --- Header Validation ---
                if isempty(is3D) || isempty(lut_size)
                    error('Parsing Error: Reached data section without finding LUT_1D_SIZE or LUT_3D_SIZE keyword.');
                end

                % Process the current line as the first data point
                vals = sscanf(line, '%f %f %f');
                if length(vals) == 3
                    data_points_read = data_points_read + 1;
                    if data_points_read > expected_data_points
                        error('Line %d: Found more data points (%d) than expected (%d).', line_num, data_points_read, expected_data_points);
                    end
                    temp_lut_data(data_points_read, :) = vals'; % Store as row
                else
                    error('Line %d: Invalid data format (expected 3 numbers): %s', line_num, line);
                end
            end % End header parsing block

        % --- Parse Data Section ---
        else % if not in_header
             if data_points_read >= expected_data_points
                 warning('Line %d: Ignoring extra line found after expected number of data points: %s', line_num, line);
                 continue; % Or break, depending on strictness
             end

             vals = sscanf(line, '%f %f %f');
             if length(vals) == 3
                data_points_read = data_points_read + 1;
                temp_lut_data(data_points_read, :) = vals'; % Store as row
             else
                 warning('Line %d: Invalid data format (expected 3 numbers): %s', line_num, line);
                 % Decide whether to error out or try to continue
                 % error('Line %d: Invalid data format (expected 3 numbers): %s', line_num, line);
                 continue;
             end
        end % End header/data sections check
    end % End while loop (reading lines)

    % --- Final Validation and Data Reshaping ---
    if data_points_read ~= expected_data_points
        error('Parsing Error: Expected %d data points, but found %d.', expected_data_points, data_points_read);
    end

    if is3D
        % Reshape the flat 3D data into the NxNxNx3 structure
        % We need to carefully map the linear index (k from 0 to N^3-1)
        % where B changes fastest, then G, then R, to MATLAB's 1-based
        % (r_idx, g_idx, b_idx) indices.
        N = lut_size;
        lut_table = zeros(N, N, N, 3, 'double'); % Preallocate final table
        for k = 0:(expected_data_points - 1)
            % Calculate 0-based indices from linear index k
            b0 = mod(k, N);
            g0 = mod(floor(k / N), N);
            r0 = floor(k / (N * N));

            % Convert to 1-based MATLAB indices and assign the RGB triplet
            % temp_lut_data is (N^3 x 3), access row (k+1)
            lut_table(r0 + 1, g0 + 1, b0 + 1, :) = temp_lut_data(k + 1, :);
        end
        resolutions = [N, N, N];
    else % if 1D
        lut_table = temp_lut_data; % Assign the Nx3 data directly
        resolutions = [lut_size];
    end

    % Ensure domain min/max are rows
    domain_min = reshape(domain_min, 1, 3);
    domain_max = reshape(domain_max, 1, 3);

end % End function parse_cube_lut
