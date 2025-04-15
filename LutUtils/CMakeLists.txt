cmake_minimum_required (VERSION 3.25)

project (LutUtils LANGUAGES CXX)

add_subdirectory (HuffmanLib)

add_library (StringView INTERFACE)
target_include_directories (StringView INTERFACE 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
target_sources (StringView
        INTERFACE FILE_SET HEADERS
        BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include
        FILES string_view.h
)


add_library (ComputeMode INTERFACE)
target_include_directories (ComputeMode INTERFACE 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
target_sources (ComputeMode
        INTERFACE FILE_SET HEADERS
        BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include
        FILES compute_mode.h
)

