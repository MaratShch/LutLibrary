# Configuration file for build LutLibrary for Windows OS (Win64 on i64 platform only supported)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_GENERATOR_PLATFORM x64)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

#include(AVX512test)

add_compile_definitions(-D_FILE_OFFSET_BITS=64)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
	
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

if(MSVC)
# Microsoft Visual Studio compiler used
 message (STATUS "Microsoft Visual Studio compiler detected...")
 add_compile_definitions(-D_CRT_SECURE_NO_WARNINGS -D_SCL_SECURE_NO_WARNINGS)

 # ADD COMMON COMPILER OPTIONS
 # /EHsc - Set standard C++ exception handling model (usually default, but good to be explicit)
 # /GS - Set security checks (usually default in modern VS, good practice)
 # /FC - Set Full Path diagnostic messages (helps locate errors/warnings)
 # /W3 - Recommended level for warnings
 # /arch:AVX2 or /arch:AVX512 - minimal CPU requirements: AVX2 or AVX512 (if available) instructions set
 # /Za - Disables MS language extensions
#if(AVX512_SUPPORTED)
#  add_compile_options(/EHsc /GS /FC /W3 /arch:AVX512) # /Za)
# else()
  add_compile_options(/EHsc /GS /FC /W3 /arch:AVX2) # /Za)
# endif()
 
 if(ENABLE_HIGH_ACCURACY)
  set(CMAKE_CXX_FLAGS_DEBUG "/Od" CACHE STRING "/Od" FORCE)
  set(CMAKE_CXX_FLAGS_RELEASE "/O2" CACHE STRING "/O2" FORCE)
  set(CMAKE_CXX_FLAGS_MINSIZEREL "/Os" CACHE STRING "/Od" FORCE)
  set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "/O2" CACHE STRING "/O2" FORCE)
  message("${COLOR_BOLD_GREEN} Build for Windows OS with HIGH ACCURACY ENABLED${COLOR_RESET}")
  add_compile_definitions(WITH_HIGH_ACCURACY_MODE)
  add_compile_options(
    "$<$<CONFIG:Debug>:/Od;/Ot;/ZI;/RTC1;/MDd;/fp:strict;/D_DEBUG;/D_ITERATOR_DEBUG_LEVEL=2>"
    "$<$<CONFIG:Release>:/O2;/Oi;/Ot;/FAs;/MD;/DNDEBUG;/fp:strict>"
    "$<$<CONFIG:MinSizeRel>:/Os;/Ob0;/Os;/FAs;/MD;/DNDEBUG;/fp:strict>"
    "$<$<CONFIG:RelWithDebInfo>:/O2;/Oi;/Ot;/Zi;/FAs;/MDd;/DNDEBUG;/fp:strict>"
 )
 else()
  set(CMAKE_CXX_FLAGS_DEBUG "/Od" CACHE STRING "/Od" FORCE)
  set(CMAKE_CXX_FLAGS_RELEASE "/Ox" CACHE STRING "/Ox" FORCE)
  set(CMAKE_CXX_FLAGS_MINSIZEREL "/Os" CACHE STRING "/Od" FORCE)
  set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "/Ox" CACHE STRING "/Ox" FORCE)
  message ("${COLOR_YELLOW}Build for Windows OS with HIGH PERFORMANCE ENABLED${COLOR_RESET}")
  add_compile_options(
    "$<$<CONFIG:Debug>:/Od;/Ot;/ZI;/RTC1;/MDd;/fp:fast;/D_DEBUG;/D_ITERATOR_DEBUG_LEVEL=2>"
    "$<$<CONFIG:Release>:/Ox;/Oi;/Ot;/FAs;/MD;/DNDEBUG;/fp:fast>"
    "$<$<CONFIG:MinSizeRel>:/O1;/Ob0;/Os;/FAs;/MD;/DNDEBUG;/fp:fast>"
    "$<$<CONFIG:RelWithDebInfo>:/Ox;/Oi;/Ot;/Zi;/FAs;/MDd;/DNDEBUG;/fp:fast>"
 )
 endif(ENABLE_HIGH_ACCURACY)

 add_link_options(/LTCG)  # /LTCG - Link Time Code Generation flags

 add_link_options(
     "$<$<CONFIG:DEBUG>:/DEBUG>"                             # Generate debug info for linker
     "$<$<CONFIG:RELEASE>:/DEBUG:NONE;/OPT:REF;/OPT:ICF>"    # No debug info, linker size/speed optimization
     "$<$<CONFIG:RelWithDebInfo>:/DEBUG>"                    # Generate debug info for linker
     "$<$<CONFIG:MinSizeRel>:/DEBUG:NONE;/OPT:REF;/OPT:ICF>" # No debug info, linker size/speed optimization
 ) 
endif(MSVC)

include(InstallRequiredSystemLibraries)
