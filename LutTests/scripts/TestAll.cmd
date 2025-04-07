@ECHO OFF
REM --- Script to check and run multiple executables from a list ---
REM --- Save this file with a .cmd extension (e.g., RunMyTests.cmd) ---

REM --- Configuration ---
REM --- Add or remove executable names from this list as needed ---
SET "ExecutableList=Crc32Test.gtest.exe Parse3DL.gtest.exe ParseCsp3d.gtest.exe ParseCube3d.gtest.exe ParseHald.gtest.exe SaveCube3d.gtest.exe VertexTest.gtest.exe"

REM --- Check if the list is empty ---
IF "%ExecutableList%"=="" (
    ECHO ERROR: ExecutableList variable is empty. Please edit the script.
    GOTO EndScript
)

REM --- Loop and Execution Logic ---
ECHO Starting check and run process for executables in the current directory or PATH...
ECHO.

REM Loop through each item in the ExecutableList
REM %%E is the variable holding the current executable name in each loop iteration.
FOR %%E IN (%ExecutableList%) DO (
    ECHO ---------------------------------
    ECHO Checking for: %%E

    REM Check if the file specified by the variable %%E exists
    REM Using WHERE command is slightly more robust as it checks the PATH too
    WHERE %%E >nul 2>nul
    IF %ERRORLEVEL% EQU 0 (
        ECHO File found! Attempting to run %%E
        %%E
        ECHO Launched: %%E (or attempted launch)
    )
    ECHO ---------------------------------
    ECHO.
)

:EndScript
ECHO.
ECHO Script finished processing the list.
PAUSE REM Optional: Keeps the window open to see output. Remove if not needed.