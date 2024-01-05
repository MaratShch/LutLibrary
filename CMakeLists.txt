cmake_minimum_required (VERSION 3.25 FATAL_ERROR)

project (LutLibrary LANGUAGES CXX)

# Versioning
set(LUT_LIB_MAJOR 0)
set(LUT_LIB_MINOR 0)
set(LUT_LIB_RELEASE 1)
set(LUT_LIB_NAME lib${PROJECT_NAME}${LUT_LIB_MAJOR}${LUT_LIB_MINOR})
set(LUT_LIB_VERSION ${LUT_LIB_MAJOR}.${LUT_LIB_MINOR}.${LUT_LIB_RELEASE})

# Build type
if (NOT CMAKE_BUILD_TYPE)
   SET( CMAKE_BUILD_TYPE Release FORCE )
   message (STATUS "Release build type forced...")
endif()

# common build options
option(LUT_LIB_SHARED "Build shared lib" OFF)
option(LUT_LIB_STATIC "Build static lib" ON)
option(LUT_LIB_TESTS  "Build ${PROJECT_NAME} tests" OFF)

# set project root
set(LUT_LIB_PROJECT_ROOT ${CMAKE_CURRENT_SOURCE_DIR})
message (STATUS "LutLibrary PROJECT ROOT: ${LUT_LIB_PROJECT_ROOT}")

set (CMAKE_CONFIGURATIONS_DIR ${LUT_LIB_PROJECT_ROOT}/Configurations)
list (APPEND CMAKE_MODULE_PATH "${CMAKE_CONFIGURATIONS_DIR}")

message (STATUS "CMAKE_MODULE_PATH = ${CMAKE_MODULE_PATH}")

if (CMAKE_TARGET_EXTERNAL_CONFIG)
	message (STATUS  "External project configuration file defined: ${CMAKE_TARGET_EXTERNAL_CONFIG}")
	set (PROJ_CONFIG_FILE ${CMAKE_TARGET_EXTERNAL_CONFIG})
else () # !CMAKE_TARGET_EXTERNAL_CONFIG
	if (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
		set (PROJ_CONFIG_FILE "WindowsConfig")
	endif() 
	if (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
		set (PROJ_CONFIG_FILE "LinuxConfig")
	endif()
endif(CMAKE_TARGET_EXTERNAL_CONFIG)

if(NOT DEFINED PROJ_CONFIG_FILE)
	message (FATAL_ERROR "Configuration fle not detected or probably incorrectly defined")
else()
	message (STATUS  "Build project with configuration file ${PROJ_CONFIG_FILE}")
endif()

include(${PROJ_CONFIG_FILE})
set(CMAKE_DEBUG_POSTFIX "d")

add_subdirectory(LutObject)
add_subdirectory(LutInterface)
if (LUT_LIB_TESTS)
	add_subdirectory(LutTests)
endif()