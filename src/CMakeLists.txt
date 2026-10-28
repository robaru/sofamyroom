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

if(CMAKE_SYSTEM_NAME MATCHES Windows)
	set(OS "Windows")
	set(PLATFORM "x64")
elseif(CMAKE_SYSTEM_NAME MATCHES Linux)
	set(OS "Linux")
	set(PLATFORM "64bit")
elseif(CMAKE_SYSTEM_NAME MATCHES Darwin)
	set(OS "MacOS")
	set(PLATFORM "64bit")
endif()

if(GNU)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ansi")
endif()

set(CMAKE_C_FLAGS_UNITTEST ${CMAKE_C_FLAGS_DEBUG})
set(CMAKE_EXE_LINKER_FLAGS_UNITTEST ${CMAKE_EXE_LINKER_FLAGS_DEBUG})
add_compile_definitions("$<$<CONFIG:Unittest>:UNITTEST>")

add_subdirectory("libsfmt")
add_subdirectory("libpocketfft")
add_subdirectory("libroomsim")
add_subdirectory("wavwriter")

include_directories(
	"${CMAKE_SOURCE_DIR}/libroomsim/include"
	"${CMAKE_SOURCE_DIR}/libmysofa/${OS}/${PLATFORM}/include"
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
    set_property(TARGET sofamyroom libsfmt libpocketfft libroomsim wavwriter PROPERTY
  				MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<OR:$<CONFIG:Debug>,$<CONFIG:Unittest>>:Debug>DLL")
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT sofamyroom)
	set_property(TARGET sofamyroom libsfmt libroomsim wavwriter APPEND_STRING PROPERTY LINK_FLAGS_RELEASE " /INCREMENTAL:NO")
	set_property(TARGET sofamyroom libsfmt libroomsim wavwriter APPEND_STRING PROPERTY LINK_FLAGS_RELEASE " /LTCG")
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

if(BUILD_DOCS MATCHES True)
	add_subdirectory ("docsrc")
endif()
