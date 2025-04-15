# Define ANSI Escape sequence characters
# Use hexadecimal '\x1b' which is usually more reliable than octal '\033'
string(ASCII 27 ESC) # ASCII value 27 is the ESC character

# Define color codes (standard ANSI)
set(COLOR_GREEN "${ESC}[32m")        # Green foreground
set(COLOR_BOLD_GREEN "${ESC}[1;32m") # Bold Green foreground
set(COLOR_RED "${ESC}[31m")          # Red foreground
set(COLOR_YELLOW "${ESC}[33m")       # Yellow foreground
set(COLOR_BLUE "${ESC}[34m")         # Blue foreground
# ... define other colors as needed

# Define reset code
set(COLOR_RESET "${ESC}[0m")