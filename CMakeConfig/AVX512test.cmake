# ==============================================================
#  Detect AVX-512 instruction set support (Windows & Linux)
#  Works for GCC, Clang, Intel, and MSVC compilers.
#  Defines:
#      AVX512_SUPPORTED  -> TRUE if compiler supports AVX-512
#      AVX512_ENABLE     -> same flag for use in later targets
# ==============================================================

include(CheckCXXSourceCompiles)

# Minimal code sample using AVX-512 intrinsic
set(AVX512_TEST_CODE "
#include <immintrin.h>
int main() {
    __m512i v = _mm512_set1_epi32(42);
    (void)v;
    return 0;
}
")

# Try compile-time detection
check_cxx_source_compiles("${AVX512_TEST_CODE}" AVX512_SUPPORTED)

# -----------------------------------------------------------------
# Handle results
# -----------------------------------------------------------------
if(AVX512_SUPPORTED)
    set(AVX512_ENABLE TRUE CACHE BOOL "Compiler supports AVX-512" FORCE)
    message(STATUS "AVX-512 instruction set detected and available!")

    if(MSVC)
        # For MSVC 2017+ (Community/Pro/Enterprise)
        add_compile_options(/arch:AVX512)
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "Intel")
        add_compile_options(-xCORE-AVX512)
    else()
        # GCC, Clang, MinGW
        add_compile_options(-mavx512f -mavx512cd -mavx512bw -mavx512dq -mavx512vl)
    endif()

else()
    set(AVX512_ENABLE FALSE CACHE BOOL "Compiler does NOT support AVX-512" FORCE)
    message(STATUS "AVX-512 not available on this platform or compiler.")
endif()
