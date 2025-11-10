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
    -mfpmath=sse             # Use vectorized (SSE/AVX) FP math instead of x87
    -march=native            # Tells the compiler to optimize for the specific CPU it's compiling on
)

if (ENABLE_HIGH_ACCURACY)
 set(CMAKE_CXX_FLAGS_DEBUG "-O0" CACHE STRING "-O0" FORCE)
 set(CMAKE_CXX_FLAGS_RELEASE "-O3" CACHE STRING "-O3" FORCE)
 set(CMAKE_CXX_FLAGS_MINSIZEREL "-O3" CACHE STRING "-O3" FORCE)
 set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-Os" CACHE STRING "-Os" FORCE)

 message("${COLOR_BOLD_GREEN}Build for LINUX OS with HIGH ACCURACY ENABLED${COLOR_RESET}")
 add_compile_definitions(WITH_HIGH_ACCURACY_MODE)
 add_compile_options(
    "$<$<CONFIG:DEBUG>:-O0 -g3 -DDEBUG -D_GLIBCXX_DEBUG>"# disable optimization, includes debugging information
    "$<$<CONFIG:RELEASE>:-O3 -s -DNDEBUG>"		 # high optimization
    "$<$<CONFIG:RelWithDebInfo>:-O3 -ggdb -g -DNDEBUG>"
    "$<$<CONFIG:MinSizeRel>:-Os -g0 -s -DNDEBUG>"
 )
 add_compile_options(
     -fno-fast-math           # Disable fast math (preserves IEEE compliance)
     -frounding-math          # Respect rounding mode (prevents dangerous FP optimizations)
     -fno-associative-math    # Prevent floating point optimizations based on associativity
     -fno-reciprocal-math     # Prevent optimizing division/sqrt using reciprocals
     -ffp-contract=off        # Disable FMA (Fused Multiply-Add) if it might change results
 )
else()
 set(CMAKE_CXX_FLAGS_DEBUG "-O0" CACHE STRING "-O0" FORCE)
 set(CMAKE_CXX_FLAGS_RELEASE "-O3" CACHE STRING "-O3" FORCE)
 set(CMAKE_CXX_FLAGS_MINSIZEREL "-O3" CACHE STRING "-O3" FORCE)
 set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-Os" CACHE STRING "-Os" FORCE)

 message ("${COLOR_YELLOW}Build for LINUX OS with HIGH PERFORMANCE ENABLED${COLOR_RESET}")
 add_compile_options(
    "$<$<CONFIG:DEBUG>:-O0 -g3  -DDEBUG -D_GLIBCXX_DEBUG>"# disable optimization, includes debugging information
    "$<$<CONFIG:RELEASE>:-O3 -s -DNDEBUG>"		  # highest optimization
    "$<$<CONFIG:RelWithDebInfo>:-O3 -ggdb -g -DNDEBUG>"
    "$<$<CONFIG:MinSizeRel>:-Os -g0 -s -DNDEBUG>"
 )
 add_compile_options(
     -ffast-math
     -fno-math-errno 
     -freciprocal-math        # Allow the reciprocal of a value to be used instead of dividing by the value if this enables optimizations
     -ffp-contract=fast       # Tells the compiler it is free to use FMA instructions (if available) or other contractions
     -ffinite-math-only       # Allow optimizations for floating-point arithmetic that assume that arguments and results are not NaNs or +-Infs
 )
endif()

include(GNUInstallDirs)
