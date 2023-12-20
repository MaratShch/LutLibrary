# Configuration file for build LutLibrary for Windows OS (Win64 on i64 platform only supported)

set(CMAKE_GENERATOR_PLATFORM x64)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_definitions(-D_LARGEFILE64_SOURCE=1)
	
if(MSVC)
# Microsoft Visual Studio compiler used
    add_definitions(-D_CRT_SECURE_NO_WARNINGS -D_SCL_SECURE_NO_WARNINGS)

	add_compile_options(
		"$<$<CONFIG:DEBUG>:/Od;/Ot>"		# disable optimization, favor fast code
		"$<$<CONFIG:RELEASE>:/O2;/Oi;/Ot>"	# high optimization, inline functions, favor fast code
	)
endif(MSVC)

# Intel compiler used