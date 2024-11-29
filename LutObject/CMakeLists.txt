cmake_minimum_required (VERSION 3.25)

project (LutObject LANGUAGES CXX)

set (LUT_OBJECT_SRC_ROOT	${LUT_LIB_PROJECT_ROOT}/${PROJECT_NAME})
set (LUT_OBJECT_SRC_CXX_DIR ${LUT_OBJECT_SRC_ROOT}/src)
set (LUT_OBJECT_SRC_HXX_DIR ${LUT_OBJECT_SRC_ROOT}/include)

file(GLOB LUT_OBJECT_CXX ${LUT_OBJECT_SRC_CXX_DIR}/*.cpp)
file(GLOB LUT_OBJECT_HXX ${LUT_OBJECT_SRC_HXX_DIR}/*.hpp ${LUT_OBJECT_SRC_HXX_DIR}/*.h)

message (STATUS "C++ files for build project ${PROJECT_NAME}: ${LUT_OBJECT_CXX}")
 
add_library (
	${PROJECT_NAME}
	STATIC
	${LUT_OBJECT_CXX}
	${LUT_OBJECT_HXX}
)

target_include_directories (
	${PROJECT_NAME}
	PUBLIC BEFORE
	${LUT_OBJECT_SRC_HXX_DIR}
)

target_link_libraries (
	${PROJECT_NAME} PUBLIC StringView HuffmanLib
)
	
install (
    TARGETS ${PROJECT_NAME}
	DESTINATION ${CMAKE_INSTALL_LIB_DIRECTORY}
)

