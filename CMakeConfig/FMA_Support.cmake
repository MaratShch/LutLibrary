cmake_minimum_required(VERSION 3.10)
project(FMA_Check_Example LANGUAGES CXX)

# --- Standard CMake module for compile checks ---
# This module PROVIDES the CHECK_CXX_SOURCE_COMPILES command
include(CheckCXXSourceCompiles) # Keep this include

# --- C++ code snippet to test FMA macro definition ---
set(FMA_TEST_CODE "
// Include necessary header for intrinsics/macros
#include <immintrin.h>

#if defined(__GNUC__) || defined(__clang__) // GCC or Clang
    #ifndef __FMA__
        #error \"FMA macro (__FMA__) not defined by compiler flag\"
    #endif
#elif defined(_MSC_VER) // Microsoft Visual C++
    #ifndef __AVX2__
        #error \"AVX2 macro (__AVX2__) not defined by compiler flag (/arch:AVX2)\"
    #endif
#else
    #error \"Unsupported compiler for this FMA check\"
#endif

int main() { return 0; }
")

# --- Perform the Check ---
set(CMAKE_REQUIRED_FLAGS_SAVE ${CMAKE_REQUIRED_FLAGS})
set(FMA_ENABLE_FLAG "")
set(COMPILER_SUPPORTS_FMA_FLAG FALSE)

if(MSVC)
    set(FMA_ENABLE_FLAG "/arch:AVX2")
    set(CMAKE_REQUIRED_FLAGS "${CMAKE_REQUIRED_FLAGS} ${FMA_ENABLE_FLAG}")
    # --- Use the CORRECT uppercase command ---
    CHECK_CXX_SOURCE_COMPILES("${FMA_TEST_CODE}" COMPILER_SUPPORTS_FMA_FLAG)
    set(CMAKE_REQUIRED_FLAGS ${CMAKE_REQUIRED_FLAGS_SAVE})
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    set(FMA_ENABLE_FLAG "-mfma")
    set(CMAKE_REQUIRED_FLAGS "${CMAKE_REQUIRED_FLAGS} ${FMA_ENABLE_FLAG}")
    # --- Use the CORRECT uppercase command ---
    CHECK_CXX_SOURCE_COMPILES("${FMA_TEST_CODE}" COMPILER_SUPPORTS_FMA_FLAG)
    set(CMAKE_REQUIRED_FLAGS ${CMAKE_REQUIRED_FLAGS_SAVE})
else()
    message(STATUS "Compiler not MSVC, GCC, or Clang. Cannot reliably check for FMA flag support.")
endif()


# --- Add Flag and Definition if Supported ---
if(COMPILER_SUPPORTS_FMA_FLAG)
    message(STATUS "Compiler supports FMA flag: ${FMA_ENABLE_FLAG}. Adding it globally.")
    add_compile_options(${FMA_ENABLE_FLAG})
    add_definitions(-DHAS_FMA_SUPPORT=1)
    message(STATUS "Defined preprocessor macro: HAS_FMA_SUPPORT=1")
else()
    message(WARNING "Compiler does not appear to support the FMA flag (${FMA_ENABLE_FLAG}) or the check failed.")
    add_definitions(-DHAS_FMA_SUPPORT=0)
    message(STATUS "Defined preprocessor macro: HAS_FMA_SUPPORT=0")
endif()

# --- Example Target ---
# Ensure you have a source file like main.cpp for this example
# add_executable(my_app main.cpp)
