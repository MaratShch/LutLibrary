# Configuration file for build LutLibrary for Linux OS

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include (Test_CpuArch)

if (BUILD_ARCH_X86)
 include(Test_AVX512)
elseif (BUILD_ARCH_ARM)
 include(Test_NEON)
endif()

set(CMAKE_INSTALL_LIB_DIRECTORY ${CMAKE_INSTALL_PREFIX}/lib)
set(CMAKE_INSTALL_HXX_DIRECTORY ${CMAKE_INSTALL_PREFIX}/include)
set(CMAKE_INSTALL_BIN_DIRECTORY ${CMAKE_INSTALL_PREFIX}/bin)
set(CMAKE_INSTALL_TST_DIRECTORY ${CMAKE_INSTALL_PREFIX}/bin/test)
set(CMAKE_INSTALL_LUT_TST_DIRECTORY ${CMAKE_INSTALL_TST_DIRECTORY}/LUT)

message (STATUS "CMAKE_INSTALL_PREFIX =     ${CMAKE_INSTALL_PREFIX}")
message (STATUS "Install LIB folder:        ${CMAKE_INSTALL_LIB_DIRECTORY}")
message (STATUS "Install HEADERS folder:    ${CMAKE_INSTALL_HXX_DIRECTORY}")
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
    -fstack-protector-strong # Add stack smashing protection
    -mfpmath=avx2            # Use vectorized (AVX2) FP math instead of x87
    -march=native            # Tells the compiler to optimize for the specific CPU it's compiling on
)

if (ENABLE_HIGH_ACCURACY) # High Accuracy Flow
 # Flags for High Accuracy (GCC)
 set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g3 -DDEBUG -D_GLIBCXX_DEBUG" CACHE STRING "-O0 -g3 -DDEBUG -D_GLIBCXX_DEBUG" FORCE)
 set(CMAKE_CXX_FLAGS_RELEASE "-O2 -s -DNDEBUG" CACHE STRING "-O2 -s -DNDEBUG" FORCE)
 set(CMAKE_CXX_FLAGS_MINSIZEREL "-Os -g0 -s -DNDEBUG" CACHE STRING "-Os -g0 -s -DNDEBUG" FORCE)
 set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -ggdb -g -DNDEBUG" CACHE STRING "-O2 -ggdb -g -DNDEBUG" FORCE)

 message("${COLOR_BOLD_GREEN}Build for LINUX OS with HIGH ACCURACY ENABLED${COLOR_RESET}") # Fine.
 add_compile_definitions(WITH_HIGH_ACCURACY_MODE) # Fine.

 # These are the crucial FP accuracy flags. They are correctly placed here.
 add_compile_options(
     -fno-fast-math           # Disable fast math (preserves IEEE compliance) - Correct for High Accuracy.
     -frounding-math          # Respect rounding mode (prevents dangerous FP optimizations) - Correct for High Accuracy.
     -fno-associative-math    # Prevent floating point optimizations based on associativity - Correct for High Accuracy.
     -fno-reciprocal-math     # Prevent optimizing division/sqrt using reciprocals - Correct for High Accuracy.
     -ffp-contract=off        # Disable FMA (Fused Multiply-Add) if it might change results - Correct for High Accuracy.
 )
else() # High Performance Flow

 # Flags for High Performance (GCC)
 set(CMAKE_CXX_FLAGS_DEBUG "-O0 -ggdb -g3 -DDEBUG -D_GLIBCXX_DEBUG" CACHE STRING "-O0 -ggdb -g3 -DDEBUG -D_GLIBCXX_DEBUG" FORCE)
 set(CMAKE_CXX_FLAGS_RELEASE "-O3 -s -DNDEBUG" CACHE STRING "-O3 -s -DNDEBUG" FORCE)
 set(CMAKE_CXX_FLAGS_MINSIZEREL "-Os -g0 -s -DNDEBUG" CACHE STRING "-Os -g0 -s -DNDEBUG" FORCE)
 set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O3 -ggdb -g -DNDEBUG" CACHE STRING "-O3 -ggdb -g -DNDEBUG" FORCE)

 message ("${COLOR_YELLOW}Build for LINUX OS with HIGH PERFORMANCE ENABLED${COLOR_RESET}") # Fine.

 # These are the crucial FP performance flags. They are correctly placed here.
 add_compile_options(
     -ffast-math
     -freciprocal-math        # Correct for High Performance.
     -ffp-contract=fast       # Correct for High Performance.
     -ffinite-math-only       # Correct for High Performance.
 )
endif()

include(GNUInstallDirs)
