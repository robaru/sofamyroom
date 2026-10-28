cmake_minimum_required(VERSION 3.13 FATAL_ERROR)

project(sofamyroom C)

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

if(CMAKE_SIZEOF_VOID_P MATCHES 4)
	message(FATAL_ERROR "32 bit OSs are not supported")
endif()

get_property(isMultiConfig GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
if (NOT isMultiConfig AND NOT CMAKE_BUILD_TYPE)
	set(defaultBuildType "Release")
	message ("-- No build type selected, default to '${defaultBuildType}'")
	set(CMAKE_BUILD_TYPE "${defaultBuildType}" CACHE STRING "Choose the type of build." FORCE)
else()
	set(CMAKE_CONFIGURATION_TYPES "Debug;Release;Unittest")
endif()

if(GNU)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ansi")
endif()

set(CMAKE_C_FLAGS_UNITTEST ${CMAKE_C_FLAGS_DEBUG})
set(CMAKE_EXE_LINKER_FLAGS_UNITTEST ${CMAKE_EXE_LINKER_FLAGS_DEBUG})
add_compile_definitions("$<$<CONFIG:Unittest>:UNITTEST>")

option(SOFAMYROOM_PYTHON "Build the shared library used by the pysofamyroom Python package" OFF)
if(SOFAMYROOM_PYTHON)
	set(CMAKE_POSITION_INDEPENDENT_CODE ON)
	set(CMAKE_C_VISIBILITY_PRESET hidden)
endif()

add_subdirectory("libsfmt")
add_subdirectory("libpocketfft")
add_subdirectory("libmysofa")
add_subdirectory("libroomsim")
add_subdirectory("wavwriter")

include_directories(
	"${CMAKE_SOURCE_DIR}/libroomsim/include"
	"${CMAKE_SOURCE_DIR}/wavwriter/include"
	)

add_executable(sofamyroom
	build.c
	main.c
	build.h
	libroomsim.h
	"${CMAKE_SOURCE_DIR}/libroomsim/include/defs.h"
	"${CMAKE_SOURCE_DIR}/libroomsim/include/dsp.h"
	"${CMAKE_SOURCE_DIR}/libroomsim/include/interface.h"
	"${CMAKE_SOURCE_DIR}/libroomsim/include/interp.h"
	"${CMAKE_SOURCE_DIR}/libroomsim/include/msg.h"
	"${CMAKE_SOURCE_DIR}/libroomsim/include/mstruct.h"
	)

if(MSVC)
    set_property(TARGET sofamyroom libsfmt libpocketfft libmysofa libroomsim wavwriter PROPERTY
  				MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<OR:$<CONFIG:Debug>,$<CONFIG:Unittest>>:Debug>DLL")
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT sofamyroom)
	set_property(TARGET sofamyroom libsfmt libpocketfft libmysofa libroomsim wavwriter APPEND_STRING PROPERTY LINK_FLAGS_RELEASE " /INCREMENTAL:NO")
	set_property(TARGET sofamyroom libsfmt libpocketfft libmysofa libroomsim wavwriter APPEND_STRING PROPERTY LINK_FLAGS_RELEASE " /LTCG")
endif()

target_link_libraries(sofamyroom libroomsim wavwriter)

# Unit tests: build, then run `ctest` in the build directory.
# The "empty room" test needs data/MIT_KEMAR_normal_pinna.sofa (see README).
enable_testing()
add_executable(sofamyroom_unittest build.c main.c unittest.c)
set_source_files_properties(unittest.c PROPERTIES HEADER_FILE_ONLY TRUE)
target_compile_definitions(sofamyroom_unittest PRIVATE
	UNITTEST
	TEST_SOFA_PATH="${CMAKE_SOURCE_DIR}/../data/MIT_KEMAR_normal_pinna.sofa")
target_link_libraries(sofamyroom_unittest libroomsim wavwriter)
add_test(NAME unittest COMMAND sofamyroom_unittest)

# Setup validation: coefficients outside [0,1] must be rejected before simulating.
add_test(NAME invalid_absorption
	COMMAND sofamyroom "${CMAKE_SOURCE_DIR}/../data/invalid_absorption.txt")
set_tests_properties(invalid_absorption PROPERTIES
	PASS_REGULAR_EXPRESSION "surface absorption must be in \\[0,1\\]")
add_test(NAME invalid_diffusion
	COMMAND sofamyroom "${CMAKE_SOURCE_DIR}/../data/invalid_diffusion.txt")
set_tests_properties(invalid_diffusion PROPERTIES
	PASS_REGULAR_EXPRESSION "surface diffusion must be in \\[0,1\\]")

# Shared library for the Python package (python/pysofamyroom loads it with ctypes)
if(SOFAMYROOM_PYTHON)
	add_library(pysofamyroom_c SHARED
		"${CMAKE_SOURCE_DIR}/libroomsim/source/capi.c"
		"${CMAKE_SOURCE_DIR}/libroomsim/include/capi.h"
		)
	target_compile_definitions(pysofamyroom_c PRIVATE SMR_EMBED SMR_BUILD_DLL)
	target_include_directories(pysofamyroom_c PRIVATE "${CMAKE_SOURCE_DIR}")
	set_target_properties(pysofamyroom_c PROPERTIES C_VISIBILITY_PRESET hidden)
	target_link_libraries(pysofamyroom_c libroomsim_embed)
	install(TARGETS pysofamyroom_c
		LIBRARY DESTINATION pysofamyroom
		RUNTIME DESTINATION pysofamyroom
		ARCHIVE DESTINATION pysofamyroom)
endif()

if(BUILD_DOCS MATCHES True)
	add_subdirectory ("docsrc")
endif()
