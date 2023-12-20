# Configuration file for build LutLibrary for Windows OS (Win64 on i64 platform only supported)

set(-DCMAKE_TOOLCHAIN_FILE=64bit.toolchain)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
# Microsoft Visual Studio compiler used
    add_definitions(-D_CRT_SECURE_NO_WARNINGS -D_SCL_SECURE_NO_WARNINGS)
	add_definitions(-D_LARGEFILE64_SOURCE=1)
endif(MSVC)

# Intel compiler used