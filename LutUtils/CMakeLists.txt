cmake_minimum_required (VERSION 3.25)

project (LutUtils LANGUAGES CXX)

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



add_library (DeflateDecoder
             STATIC
             ${CMAKE_CURRENT_SOURCE_DIR}/src/deflate_decoder.cpp
             ${CMAKE_CURRENT_SOURCE_DIR}/include/deflate_decoder.h
   	     ${CMAKE_CURRENT_SOURCE_DIR}/include/deflate_common.h
)

target_include_directories (DeflateDecoder PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

add_library (DeflateEncoder
             STATIC
             ${CMAKE_CURRENT_SOURCE_DIR}/src/deflate_encoder.cpp
             ${CMAKE_CURRENT_SOURCE_DIR}/include/deflate_encoder.h
   	     ${CMAKE_CURRENT_SOURCE_DIR}/include/deflate_common.h
)

target_include_directories (DeflateEncoder PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

install (TARGETS DeflateDecoder DeflateEncoder
         DESTINATION ${CMAKE_INSTALL_LIB_DIRECTORY})
