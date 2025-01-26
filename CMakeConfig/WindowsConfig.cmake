# Configuration file for build LutLibrary for Windows OS (Win64 on i64 platform only supported)

set(CMAKE_GENERATOR_PLATFORM x64)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_definitions(-D_LARGEFILE64_SOURCE=1 -D__USE_LARGEFILE64)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
	
set(CMAKE_INSTALL_LIB_DIRECTORY ${CMAKE_INSTALL_PREFIX}/lib)
set(CMAKE_INSTALL_HXX_DIRECTORY ${CMAKE_INSTALL_PREFIX}/include)
set(CMAKE_INSTALL_BIN_DIRECTORY ${CMAKE_INSTALL_PREFIX}/bin)
set(CMAKE_INSTALL_TST_DIRECTORY ${CMAKE_INSTALL_BIN_DIRECTORY}/test)
set(CMAKE_INSTALL_LUT_TST_DIRECTORY ${CMAKE_INSTALL_TST_DIRECTORY}/LUT)

message (STATUS "CMAKE_INSTALL_PREFIX =     ${CMAKE_INSTALL_PREFIX}")
message (STATUS "Install LIB folder:        ${CMAKE_INSTALL_LIB_DIRECTORY}")
message (STATUS "Install HEADRES folder:    ${CMAKE_INSTALL_HXX_DIRECTORY}")
message (STATUS "Install BIN folder:        ${CMAKE_INSTALL_BIN_DIRECTORY}")
message (STATUS "Install TESTS folder:      ${CMAKE_INSTALL_TST_DIRECTORY}")
message (STATUS "Install TEST LUT's folder: ${CMAKE_INSTALL_LUT_TST_DIRECTORY}")

if(MSVC)
# Microsoft Visual Studio compiler used
 message (STATUS "Microsofr Visual Studio compiler detected...")
 add_definitions(-D_CRT_SECURE_NO_WARNINGS -D_SCL_SECURE_NO_WARNINGS)

	add_compile_options(
		"$<$<CONFIG:Debug>:/Od;/Ot;/arch:AVX2>"			    		# disable optimization, favor fast code, AVX2 instruction set
		"$<$<CONFIG:Release>:/O2;/Oi;/Ot;/arch:AVX2;/FAs>"			# high optimization, inline functions, favor fast code, AVX2 instruction set, ASM output with Source code
		"$<$<CONFIG:RelWithDebInfo>:/O2;/Oi;/Ot;/arch:AVX2;/FAs>"	# high optimization, inline functions, favor fast code, AVX2 instruction set, ASM output with Source code
		"$<$<CONFIG:MinSizeRel>:/O1;/Ob0;/Os;/arch:AVX2;/FAs>"	    # minimal size, no inline functions, favor small code,  AVX2 instruction set, ASM output with Source code
	)
endif(MSVC)

include(InstallRequiredSystemLibraries)
