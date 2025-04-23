@echo off
setlocal enabledelayedexpansion

REM --- Dynamically find all executable files containing 'gtest' in their name in the current directory ---
set "ExecutableList="

for %%F in (*gtest*.exe) do (
    if exist "%%F" (
        set "ExecutableList=!ExecutableList! %%F"
    )
)

REM --- Check if the list is empty ---
if "%ExecutableList%"=="" (
    echo ERROR: No executable files with 'gtest' in the filename found in current directory: %CD%
    goto EndScript
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
