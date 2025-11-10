# ============================================================
# Detect CPU architecture family (Linux / cross-platform)
# Exports:
#   BUILD_ARCH_X86     -> TRUE if building for Intel/AMD
#   BUILD_ARCH_ARM     -> TRUE if building for ARM / AArch64
# ============================================================

string(TOLOWER "${CMAKE_SYSTEM_PROCESSOR}" SYSTEM_CPU)

set(BUILD_ARCH_X86 FALSE CACHE BOOL "Build for x86 architecture")
set(BUILD_ARCH_ARM FALSE CACHE BOOL "Build for ARM architecture")

message(STATUS "Check Target CPU Architecture and Vendor")

if(SYSTEM_CPU MATCHES "^(x86|x86_64|amd64|i[3-6]86)$")
    set(BUILD_ARCH_X86 TRUE)
    message(STATUS "Building for Intel/AMD architecture: ${CMAKE_SYSTEM_PROCESSOR}")

elseif(SYSTEM_CPU MATCHES "^(arm|aarch64|armv[0-9]+)$")
    set(BUILD_ARCH_ARM TRUE)
    message(STATUS "Building for ARM architecture: ${CMAKE_SYSTEM_PROCESSOR}")

else()
    message(WARNING "Unknown CPU architecture: ${CMAKE_SYSTEM_PROCESSOR}")
endif()
