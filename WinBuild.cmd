@echo 'Run CMake generation for Windows...'
@echo OFF

set CMAKE_BIN="cmake.exe"
set BUILD_FOLDER=build
set INSTALL_FOLDER=install

REM --- Accuracy Flag Handling (Simplest Command Line Parse) ---
REM Default to high performance (OFF) and a default message
SET CMAKE_ACCURACY_FLAG="-DENABLE_HIGH_ACCURACY=OFF"
SET MODE_MESSAGE="No accuracy flag specified, defaulting to FAST performance."

REM Check if the first argument is "--accuracy"
IF "%1"=="--accuracy" (
    IF "%2"=="high" (
        SET CMAKE_ACCURACY_FLAG=-DENABLE_HIGH_ACCURACY=ON
        SET MODE_MESSAGE="Setting accuracy mode to: HIGH"
    ) ELSE (
        IF "%2"=="fast" (
            SET CMAKE_ACCURACY_FLAG=-DENABLE_HIGH_ACCURACY=OFF
            SET MODE_MESSAGE="Setting accuracy mode to: FAST"
        ) ELSE (
            SET MODE_MESSAGE="Invalid accuracy mode: %2. Defaulting to FAST performance."
        )
    )
)

ECHO %MODE_MESSAGE%
REM --- End Accuracy Flag Handling ---

REM Remove previously created BUILD folder
IF EXIST %BUILD_FOLDER% @rmdir /S /Q %BUILD_FOLDER%

REM Remove previously created INSTALL folder
IF EXIST %INSTALL_FOLDER% @rmdir /S /Q %INSTALL_FOLDER%

md %BUILD_FOLDER%
cd %BUILD_FOLDER%

%CMAKE_BIN% .. -DCMAKE_GENERATOR_PLATFORM=x64 -DLUT_LIB_TESTS=ON -DCMAKE_BUILD_TYPE=Release %CMAKE_ACCURACY_FLAG%
REM %CMAKE_BIN% -DCMAKE_INSTALL_PREFIX:PATH=..\install .. && %CMAKE_BIN% --build . --verbose --target install --config Release -j %NUMBER_OF_PROCESSORS%
%CMAKE_BIN% -DCMAKE_INSTALL_PREFIX:PATH=..\install .. && %CMAKE_BIN% --build . --target install --config Release -j %NUMBER_OF_PROCESSORS%
cd ..
