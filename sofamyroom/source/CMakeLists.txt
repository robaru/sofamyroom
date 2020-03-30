cmake_minimum_required(VERSION 3.13 FATAL_ERROR)

project(sofamyroom C)

if(CMAKE_SIZEOF_VOID_P MATCHES 4)
	message(FATAL_ERROR "32 bit OSs are not supported")
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

set(CMAKE_CONFIGURATION_TYPES "Debug;Release;Unittest")

if(GNU)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ansi")
endif()

set(CMAKE_C_FLAGS_UNITTEST ${CMAKE_C_FLAGS_DEBUG})
set(CMAKE_EXE_LINKER_FLAGS_UNITTEST ${CMAKE_EXE_LINKER_FLAGS_DEBUG})
add_compile_definitions("$<$<CONFIG:Unittest>:UNITTEST>")

add_subdirectory(libsfmt)
add_subdirectory(libroomsim)
add_subdirectory(wavwriter)

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
	add_custom_command(TARGET sofamyroom POST_BUILD
                   		COMMAND ${CMAKE_COMMAND} -E copy_if_different
                   		"${CMAKE_SOURCE_DIR}/libfftw/${OS}/${PLATFORM}/bin/libfftw3-3.dll" $<TARGET_FILE_DIR:sofamyroom>)
    set_property(TARGET sofamyroom libsfmt libroomsim wavwriter PROPERTY
  				MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<OR:$<CONFIG:Debug>,$<CONFIG:Unittest>>:Debug>")
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT sofamyroom)
endif()

target_link_libraries(sofamyroom libroomsim wavwriter)
