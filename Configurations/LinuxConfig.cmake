# Configuration file for build LutLibrary for Linux OS

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_compile_options(
	"$<$<CONFIG:DEBUG>:-O0 -mavx2>"		# disable optimization, AVX2 instruction set
	"$<$<CONFIG:RELEASE>:-O3 -mavx2>"	# highest optimization, AVX2 instruction set
)

include(GNUInstallDirs)
