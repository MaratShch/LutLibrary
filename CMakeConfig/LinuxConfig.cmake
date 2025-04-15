# Configuration file for build LutLibrary for Linux OS

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_INSTALL_LIB_DIRECTORY ${CMAKE_INSTALL_PREFIX}/lib)
set(CMAKE_INSTALL_HXX_DIRECTORY ${CMAKE_INSTALL_PREFIX}/include)
set(CMAKE_INSTALL_BIN_DIRECTORY ${CMAKE_INSTALL_PREFIX}/bin)
set(CMAKE_INSTALL_TST_DIRECTORY ${CMAKE_INSTALL_PREFIX}/bin/test)
set(CMAKE_INSTALL_LUT_TST_DIRECTORY ${CMAKE_INSTALL_TST_DIRECTORY}/LUT)

message (STATUS "CMAKE_INSTALL_PREFIX =     ${CMAKE_INSTALL_PREFIX}")
message (STATUS "Install LIB folder:        ${CMAKE_INSTALL_LIB_DIRECTORY}")
message (STATUS "Install HEADRES folder:    ${CMAKE_INSTALL_HXX_DIRECTORY}")
message (STATUS "Install BIN folder:        ${CMAKE_INSTALL_BIN_DIRECTORY}")
message (STATUS "Install TESTS folder:      ${CMAKE_INSTALL_TST_DIRECTORY}")
message (STATUS "Install TEST LUT's folder: ${CMAKE_INSTALL_LUT_TST_DIRECTORY}")

add_compile_definitions(
    _LARGEFILE64_SOURCE=1
    __USE_LARGEFILE64
    "$<$<NOT:$<CONFIG:Debug>>:_FORTIFY_SOURCE=2>" # Enables checks in glibc functions (like memcpy, sprintf) 
	                                              # for potential buffer overflows
)

add_compile_options(
    -Wall                    # Enable common warnings
    -Wextra                  # Enable more warnings
    -pedantic                # Uncomment for stricter ISO C++/C compliance warnings
    -Werror                  # Uncomment to treat warnings as errors (good for CI/strict dev)
    -fstack-protector-strong # Add stack smashing protection
    -mavx2                   # Target CPU instruction set (Requires AVX2 support at runtime!)
)

add_compile_options(
	"$<$<CONFIG:DEBUG>:-O0 -g3 -D_GLIBCXX_DEBUG>"	# disable optimization, includes debugging information
	"$<$<CONFIG:RELEASE>:-O3 -DNDEBUG"				# highest optimization
)

include(GNUInstallDirs)
