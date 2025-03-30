% Gradient Generation Script 
% --- Configuration ---
imageSize = 1024; % Size of the images (square)
outputFolder = 'gradient_images'; % Folder to save the images

% Create output folder if it doesn't exist
if ~exist(outputFolder, 'dir')
    mkdir(outputFolder);
end

% --- Generate Gradients ---

% 1. Black to White
bw_horizontal = createLinearGradient(imageSize, [0 0 0], [1 1 1], 'horizontal');
bw_vertical = createLinearGradient(imageSize, [0 0 0], [1 1 1], 'vertical');
bw_diagonal = createLinearGradient(imageSize, [0 0 0], [1 1 1], 'diagonal');
imwrite(bw_horizontal, fullfile(outputFolder, 'bw_horizontal.png'));
imwrite(bw_vertical, fullfile(outputFolder, 'bw_vertical.png'));
imwrite(bw_diagonal, fullfile(outputFolder, 'bw_diagonal.png'));

% 2. Red to Green
rg_horizontal = createLinearGradient(imageSize, [1 0 0], [0 1 0], 'horizontal');
rg_vertical = createLinearGradient(imageSize, [1 0 0], [0 1 0], 'vertical');
imwrite(rg_horizontal, fullfile(outputFolder, 'rg_horizontal.png'));
imwrite(rg_vertical, fullfile(outputFolder, 'rg_vertical.png'));

% 3. Blue to Yellow (Yellow is [1 1 0])
by_horizontal = createLinearGradient(imageSize, [0 0 1], [1 1 0], 'horizontal');
by_vertical = createLinearGradient(imageSize, [0 0 1], [1 1 0], 'vertical');
imwrite(by_horizontal, fullfile(outputFolder, 'by_horizontal.png'));
imwrite(by_vertical, fullfile(outputFolder, 'by_vertical.png'));

% 4. Cyan to Magenta (Cyan is [0 1 1], Magenta is [1 0 1])
cm_horizontal = createLinearGradient(imageSize, [0 1 1], [1 0 1], 'horizontal');
cm_vertical = createLinearGradient(imageSize, [0 1 1], [1 0 1], 'vertical');
imwrite(cm_horizontal, fullfile(outputFolder, 'cm_horizontal.png'));
imwrite(cm_vertical, fullfile(outputFolder, 'cm_vertical.png'));

% 5. Complex Gradient
complex_gradient = createComplexGradient(imageSize);
imwrite(complex_gradient, fullfile(outputFolder, 'complex_gradient.png'));

% 6. Pure Red
r_horizontal = createLinearGradient(imageSize, [0 0 0], [1 0 0], 'horizontal');
r_vertical = createLinearGradient(imageSize, [0 0 0], [1 0 0], 'vertical');
imwrite(r_horizontal, fullfile(outputFolder, 'r_horizontal.png'));
imwrite(r_vertical, fullfile(outputFolder, 'r_vertical.png'));

% 7. Pure Green
g_horizontal = createLinearGradient(imageSize, [0 0 0], [0 1 0], 'horizontal');
g_vertical = createLinearGradient(imageSize, [0 0 0], [0 1 0], 'vertical');
imwrite(g_horizontal, fullfile(outputFolder, 'g_horizontal.png'));
imwrite(g_vertical, fullfile(outputFolder, 'g_vertical.png'));

% 8. Pure Blue
b_horizontal = createLinearGradient(imageSize, [0 0 0], [0 0 1], 'horizontal');
b_vertical = createLinearGradient(imageSize, [0 0 0], [0 0 1], 'vertical');
imwrite(b_horizontal, fullfile(outputFolder, 'b_horizontal.png'));
imwrite(b_vertical, fullfile(outputFolder, 'b_vertical.png'));

disp('Gradients generated and saved to the "gradient_images" folder.');

% --- Functions ---

% Function to create a linear gradient
function img = createLinearGradient(size, startColor, endColor, direction)
    img = zeros(size, size, 3);
    switch direction
        case 'horizontal'
            for x = 1:size
                color = startColor + (endColor - startColor) * (x - 1) / (size - 1);
                img(:, x, :) = repmat(reshape(color, [1 1 3]), size, 1);
            end
        case 'vertical'
            for y = 1:size
                color = startColor + (endColor - startColor) * (y - 1) / (size - 1);
                img(y, :, :) = repmat(reshape(color, [1 1 3]), 1, size);
            end
        case 'diagonal'
             for y = 1:size
                for x = 1:size
                  color = startColor + (endColor - startColor) * ((x - 1) + (y-1))/ (2*size - 2);
                  img(y, x, :) = reshape(color, [1 1 3]);
                end
            end
        otherwise
            error('Invalid direction.');
    end
end

% Function to create a complex gradient
function img = createComplexGradient(size)
    img = zeros(size, size, 3);
    for y = 1:size
        for x = 1:size
            r = sin(2*pi*x/size) * 0.5 + 0.5;
            g = cos(2*pi*y/size) * 0.5 + 0.5;
            b = sin(2*pi*(x+y)/(2*size)) * 0.5 + 0.5;
            img(y, x, :) = [r g b];
        end
    end
end

