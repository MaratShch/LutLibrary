% Rainbow Gradient Image Generation Script (MATLAB 2018b) - BMP Output

% --- Configuration ---
imageWidth = 1920;   % Image width
imageHeight = 1080;  % Image height
outputFile = 'rainbow_gradients.bmp'; % Output BMP file

% --- Color Definitions ---
colors = [
    1 1 1;        % White
    1 0 0;        % Red
    1 0.5 0;      % Orange
    1 1 0;        % Yellow
    0 1 0;        % Green
    0 0 1;        % Blue
    0.5 0 1;      % Indigo
    1 0 1         % Violet
];

numStripes = size(colors, 1);  % Number of color stripes
stripeHeight = floor(imageHeight / numStripes); % Height of each stripe

% --- Image Generation ---
img = zeros(imageHeight, imageWidth, 3, 'uint8'); % Create uint8 image

% Loop through each stripe
for i = 1:numStripes
    startRow = (i - 1) * stripeHeight + 1;
    endRow = i * stripeHeight;

    % Handle the last stripe to fill the remaining height
    if i == numStripes
        endRow = imageHeight;
    end

    % Generate gradient for the current stripe
    for y = startRow:endRow
        for x = 1:imageWidth
            % Calculate the color based on horizontal position and looping
            intensity = mod(x - 1, 256); % Integer values from 0 to 255

            % Scale the color values to 0-255
            color = uint8(colors(i, :) * intensity);

            % Assign pixel color
            img(y, x, :) = color;
        end
    end
end

% --- Save the Image ---
imwrite(img, outputFile, 'bmp'); % Save as BMP

disp(['Rainbow gradient image generated and saved to: ' outputFile]);
