cmake_minimum_required (VERSION 3.25 FATAL_ERROR)

# Cleanup default C++ CMake options
# All parameters for build will be defined explicitly in developer provided CMake files
set(CMAKE_CXX_FLAGS_DEBUG "" CACHE STRING "" FORCE)
set(CMAKE_CXX_FLAGS_RELEASE "" CACHE STRING "" FORCE)
set(CMAKE_CXX_FLAGS_MINSIZEREL "" CACHE STRING "" FORCE)
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "" CACHE STRING "" FORCE)
#

project (LutLibrary LANGUAGES CXX)

# global features
set (BUILD_INCLUDE_BRANCH_NAME ON)

option(ENABLE_HIGH_ACCURACY "Enable floating point computations with highest accuracy (potentially slower)." OFF)

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

set (CMAKE_CONFIGURATIONS_DIR ${LUT_LIB_PROJECT_ROOT}/CMakeConfig)
list (APPEND CMAKE_MODULE_PATH "${CMAKE_CONFIGURATIONS_DIR}")

message (DEBUG "CMAKE_MODULE_PATH = ${CMAKE_MODULE_PATH}")

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

include(ColorPrint)
include(${PROJ_CONFIG_FILE})
set(CMAKE_DEBUG_POSTFIX "d")

if (BUILD_INCLUDE_BRANCH_NAME)
	# determine git branch
	# Execute Git command to get the current branch name
	execute_process(
		COMMAND git branch --show-current
		WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
		OUTPUT_VARIABLE GIT_BRANCH
		ERROR_QUIET
		RESULT_VARIABLE GIT_RESULT
	)

	# Check if the Git command was successful
	if(NOT ${GIT_RESULT} EQUAL "0")
		message(WARNING "Failed to determine Git branch name.")
		set(GIT_BRANCH "unknown")
	else()
		message(STATUS "Current GIT branch name ${GIT_BRANCH}") 	
	endif()

	# Sanitize the branch name (remove leading/trailing whitespace)
	string(STRIP "${GIT_BRANCH}" GIT_BRANCH)

	# Define a preprocessor macro with the branch name
	add_definitions(-DPROJECT_GIT_BRANCH="${GIT_BRANCH}")
endif(BUILD_INCLUDE_BRANCH_NAME)

# add 3rd party components/libs directory
add_subdirectory(3rd)

add_subdirectory(LutUtils)
add_subdirectory(LutObject)
add_subdirectory(LutInterface)
add_subdirectory(LutInterpolator)
if (LUT_LIB_TESTS)
	add_subdirectory(LutTests)
	add_subdirectory(3rd/GoogleTest)
endif()
