@echo 'Run CMake generatiob for Windows...'
rmdir /S /Q Build
md Build
cd Build
cmake .. -DCMAKE_GENERATOR_PLATFORM=x64 -DLUT_LIB_TESTS=ON
cmake --build .
cd ..





