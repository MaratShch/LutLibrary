# ==============================================================
# Detect NEON SIMD support (Linux / Android / embedded ARM)
# Works for GCC, Clang, ARMClang.
# Exports:
#     NEON_SUPPORTED  -> TRUE if compiler supports NEON
#     NEON_ENABLE     -> TRUE if compiler flags applied
# ==============================================================

include(CheckCXXSourceCompiles)

set(NEON_TEST_CODE "
#include <arm_neon.h>
int main() {
    float32x4_t v = vdupq_n_f32(1.0f);
    (void)v;
    return 0;
}
")

check_cxx_source_compiles("${NEON_TEST_CODE}" NEON_SUPPORTED)

if(NEON_SUPPORTED)
    set(NEON_ENABLE TRUE CACHE BOOL "Compiler supports ARM NEON SIMD" FORCE)
    message(STATUS "NEON SIMD detected and available!")

    if(CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64")
        # AArch64 (ARMv8-A 64-bit) always has NEON — no flag needed
        message(STATUS "AArch64 detected: NEON enabled by default")
    else()
        # 32-bit ARM (ARMv7-A or similar)
        add_compile_options(-mfpu=neon)
    endif()

else()
    set(NEON_ENABLE FALSE CACHE BOOL "Compiler does NOT support ARM NEON" FORCE)
    message(STATUS "NEON not available for this toolchain or architecture.")
endif()
