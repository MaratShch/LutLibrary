set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_GENERATOR_PLATFORM x64)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

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

 # ADD COMMON COMPILER OPTIONS (These apply to ALL MSVC builds: Debug, Release, both flows)
 add_compile_options(/EHsc /GS /FC /W3 /arch:AVX2) # /Za) # /W3 is okay, /W4 is often preferred.

 # ACCURACY FLOW CHECK: This if/else structure is used to conditionally set base flags and add flow-specific flags.
 if(ENABLE_HIGH_ACCURACY) # High Accuracy Flow
  set(CMAKE_CXX_FLAGS_DEBUG "/Od" CACHE STRING "/Od" FORCE)
  set(CMAKE_CXX_FLAGS_RELEASE "/O2" CACHE STRING "/O2" FORCE)
  set(CMAKE_CXX_FLAGS_MINSIZEREL "/Os" CACHE STRING "/Os" FORCE)
  set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "/O2" CACHE STRING "/O2" FORCE)
  
  message("${COLOR_BOLD_GREEN} Build for Windows OS with HIGH ACCURACY ENABLED${COLOR_RESET}") # Fine.
  add_compile_definitions(WITH_HIGH_ACCURACY_MODE) # Fine.

  add_compile_options(
    "$<$<CONFIG:Debug>:/Od;/ZI;/RTC1;/MDd;/fp:strict;/D_DEBUG;/D_ITERATOR_DEBUG_LEVEL=2>" 
    "$<$<CONFIG:Release>:/O2;/MD;/DNDEBUG;/fp:strict>"
    "$<$<CONFIG:RelWithDebInfo>:/O2;/Zi;/MD;/DNDEBUG;/fp:strict>"
    "$<$<CONFIG:MinSizeRel>:/Os;/MD;/DNDEBUG;/fp:strict>"
 )
 else() # ENABLE_HIGH_ACCURACY is OFF (High Performance Flow)

  # Flags below are for High Performance (MSVC)
  set(CMAKE_CXX_FLAGS_DEBUG "/Od" CACHE STRING "/Od" FORCE)
  set(CMAKE_CXX_FLAGS_RELEASE "/Ox" CACHE STRING "/Ox" FORCE)
  set(CMAKE_CXX_FLAGS_MINSIZEREL "/Os" CACHE STRING "/Os" FORCE)
  set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "/Ox" CACHE STRING "/Ox" FORCE)
  
  message ("${COLOR_YELLOW}Build for Windows OS with HIGH PERFORMANCE ENABLED${COLOR_RESET}")

  add_compile_options(
    "$<$<CONFIG:Debug>:/Od;/ZI;/RTC1;/MDd;/fp:fast;/D_DEBUG;/D_ITERATOR_DEBUG_LEVEL=2>"
    "$<$<CONFIG:Release>:/Ox;/MD;/DNDEBUG;/fp:fast>"
    "$<$<CONFIG:RelWithDebInfo>:/Ox;/Zi;/MD;/DNDEBUG;/fp:fast>"
    "$<$<CONFIG:MinSizeRel>:/Os;/MD;/DNDEBUG;/fp:fast>"
 )
 endif(ENABLE_HIGH_ACCURACY)

 # LINKER OPTIONS BLOCK (Placed inside if(MSVC) is correct. add_link_options is correct command)
 add_link_options( # Correct command.
     "$<$<CONFIG:DEBUG>:/DEBUG>"                             # Generate debug info for linker (Correct)
     "$<$<CONFIG:RELEASE>:/DEBUG:NONE;/OPT:REF;/OPT:ICF>"    # No debug info, linker size/speed optimization (Correct)
     "$<$<CONFIG:RelWithDebInfo>:/DEBUG>"                    # Generate debug info for linker (Correct)
     "$<$<CONFIG:MinSizeRel>:/DEBUG:NONE;/OPT:REF;/OPT:ICF>" # No debug info, linker size/speed optimization (Correct)
     "$<$<OR:$<CONFIG:RELEASE>,$<CONFIG:RelWithDebInfo>,$<CONFIG:MinSizeRel>>:/LTCG>"
 ) 
endif(MSVC) # Correct endif.

include(InstallRequiredSystemLibraries) # Correct command, placement fine.
