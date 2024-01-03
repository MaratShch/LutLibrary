@echo 'Run CMake generation for Windows...'
@echo OFF

set BUILD_FOLDER=build
set INSTALL_FOLDER=install

REM Remove previously created BUILD folder
IF EXIST %BUILD_FOLDER% @rmdir /S /Q %BUILD_FOLDER%

REM Remove previously created INSTALL folder
IF EXIST %INSTALL_FOLDER% @rmdir /S /Q %INSTALL_FOLDER%

md %BUILD_FOLDER%
cd %BUILD_FOLDER%

cmake .. -DCMAKE_GENERATOR_PLATFORM=x64 -DLUT_LIB_TESTS=ON
cmake -DCMAKE_INSTALL_PREFIX:PATH=..\install .. && cmake --build . --target install --config Release
cd ..
