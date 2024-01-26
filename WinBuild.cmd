@echo 'Run CMake generation for Windows...'
@echo OFF

set CMAKE_BIN="cmake.exe"
set BUILD_FOLDER=build
set INSTALL_FOLDER=install

REM Remove previously created BUILD folder
IF EXIST %BUILD_FOLDER% @rmdir /S /Q %BUILD_FOLDER%

REM Remove previously created INSTALL folder
IF EXIST %INSTALL_FOLDER% @rmdir /S /Q %INSTALL_FOLDER%

md %BUILD_FOLDER%
cd %BUILD_FOLDER%

%CMAKE_BIN% .. -DCMAKE_GENERATOR_PLATFORM=x64 -DLUT_LIB_TESTS=ON
%CMAKE_BIN% -DCMAKE_INSTALL_PREFIX:PATH=..\install .. && %CMAKE_BIN% --build . --target install --config Release
cd ..
