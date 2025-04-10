#!/bin/bash

# --- Script to check and run multiple executables sequentially in the CURRENT terminal ---

# --- Configuration ---
EXECUTABLE_LIST=(
    "Crc32Test.gtest"
    "Parse3DL.gtest"
    "ParseCsp3d.gtest"
    "ParseCube3d.gtest"
    "ParseHald.gtest"
    "SaveCube3d.gtest"
    "VertexTest.gtest"
)

# --- Check if the list is empty ---
if [ ${#EXECUTABLE_LIST[@]} -eq 0 ]; then
    echo "ERROR: EXECUTABLE_LIST is empty. Please edit the script."
    exit 1 # Exit with an error code
fi

# --- Loop and Execution Logic ---
echo "Starting sequential check and run process in this terminal..."
echo "IMPORTANT: The script will wait for each test to complete before starting the next."
echo

# Loop through each item in the EXECUTABLE_LIST array
# Using "${EXECUTABLE_LIST[@]}" correctly handles names with spaces
for executable in "${EXECUTABLE_LIST[@]}"; do
    echo "---------------------------------"
    echo "Checking for: $executable"

    # Check if the file exists AND is executable by the current user
    # The '-f' flag checks if it's a regular file (or symlink to one)
    # The '-x' flag checks if it has execute permissions
    # Now we check for the path explicitly starting with ./
    if [ -f "$executable" ] && [ -x "$executable" ]; then
        echo "File found and is executable. Running '$executable' in this terminal (waiting for completion)..."

        # --- Execute directly in this terminal window ---
        # The script will PAUSE here until the executable finishes.
        # Using "$executable" now correctly calls "./Crc32Test.gtest.exe" etc.
        ./"$executable"

        # Capture the exit code of the executable that just ran
        exit_code=$?
        echo "Finished running '$executable' (Exit Code: $exit_code)"

    # This elif block is now less likely to be needed if files are local,
    # but kept for robustness in case an item IS meant to be a PATH command.
    elif command -v "$executable" >/dev/null 2>&1; then
         echo "Command found in PATH (unlikely for './' prefixed names, but checking). Running '$executable'..."
         "$executable" # Execute the command found in PATH
         exit_code=$?
         echo "Finished running '$executable' (Exit Code: $exit_code)"
    else
        # If neither check passed, it's not found or not executable
        if [ ! -e "$executable" ]; then
             # This message should now be less common with ./ prefix
             echo "File '$executable' was NOT found. Skipping."
             echo "(Is it definitely in this directory: $(pwd) ?)"
        elif [ ! -x "$executable" ]; then
             # This becomes the more likely error if the file exists but isn't executable
             echo "File '$executable' was found BUT is NOT executable. Skipping."
             echo "(Hint: You might need to run 'chmod +x $executable')"
        else
             # Catch-all for other issues
             echo "Could not run '$executable' for an unknown reason (exists: $([ -e "$executable" ] && echo yes || echo no), executable: $([ -x "$executable" ] && echo yes || echo no)). Skipping."
        fi
    fi
    echo "---------------------------------"
    echo
done

echo
echo "Script finished processing the list sequentially."

# Exit with success code 0
exit 0
