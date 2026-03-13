@echo off
REM ============================================
REM Clean build/install folders
REM ============================================

setlocal

REM Set WORK_DIR to current directory
set "WORK_DIR=%CD%"

REM Define folder paths
set "BUILD_FOLDER=%WORK_DIR%\build"
set "INSTALL_FOLDER=%WORK_DIR%\install"

echo.
echo Cleaning folders in: %WORK_DIR%
echo.

call :remove_dir "%BUILD_FOLDER%"
call :remove_dir "%INSTALL_FOLDER%"


echo.
echo Cleanup complete.
echo.
goto :eof


REM ============================================
REM Function: remove_dir
REM ============================================
:remove_dir
set "DIR=%~1"

if exist "%DIR%\" (
    echo Removing: %DIR%
    rmdir /S /Q "%DIR%"
) else (
    echo Folder does not exist: %DIR%
)

goto :eof