@echo 'Run CMake generation for Windows...'
rmdir /S /Q build
rmdir /S /Q install
md build
cd build
cmake .. -DCMAKE_GENERATOR_PLATFORM=x64 -DLUT_LIB_TESTS=ON
cmake -DCMAKE_INSTALL_PREFIX:PATH=..\install .. && cmake --build . --target install --config Release
cd ..



