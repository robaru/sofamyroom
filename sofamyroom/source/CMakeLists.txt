cmake_minimum_required(VERSION 3.15 FATAL_ERROR)

project(roomsim C)

if(CMAKE_SIZEOF_VOID_P MATCHES 4)
	message(FATAL_ERROR "32 bit OSs are not supported")
endif()

set(CMAKE_CONFIGURATION_TYPES "Debug;Release;Uinttest")

if (NOT CMAKE_BUILD_TYPE)
	message("No build type selected, default to Release")
	set(CMAKE_BUILD_TYPE "Release")
endif()

set(LIBFOLDER "${CMAKE_BUILD_TYPE}")
if(CMAKE_BUILD_TYPE MATCHES Unittest)
	set(LIBFOLDER "Debug")
endif()

if(CMAKE_SYSTEM_NAME MATCHES Windows)
	set(OS "Windows")
	set(PLATFORM "x64")
elseif(CMAKE_SYSTEM_NAME MATCHES Linux)
	set(OS "Linux")
	set(PLATFORM "64bit")
elseif(CMAKE_SYSTEM_NAME STREQUAL Darwin)
	set(OS "MacOS")
	set(PLATFORM "64bit")
endif()

add_subdirectory(libsfmt)
add_subdirectory(libroomsim)
add_subdirectory(wavwriter)

add_executable(roomsim
	build.c
	main.c
	)

if(MSVC)
	target_link_options(roomsim PRIVATE "/LTCG")
	add_custom_command(TARGET roomsim POST_BUILD
                   	COMMAND ${CMAKE_COMMAND} -E copy_if_different
                   	"${CMAKE_SOURCE_DIR}/libfftw/${OS}/${PLATFORM}/bin/libfftw3-3.dll" $<TARGET_FILE_DIR:roomsim>)
    set_property(TARGET roomsim PROPERTY
  		MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<OR:$<CONFIG:Debug>,$<CONFIG:Unittest>>:Debug>")
  	set(CMAKE_EXE_LINKER_FLAGS_UINTTEST ${CMAKE_EXE_LINKER_FLAGS_DEBUG})
endif()

if(GNU)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ansi")
endif()

include_directories(
	"${CMAKE_SOURCE_DIR}/libroomsim/include"
	"${CMAKE_SOURCE_DIR}/libmysofa/${OS}/${PLATFORM}/include"
	"${CMAKE_SOURCE_DIR}/wavwriter/include"
	)

target_link_libraries(roomsim libroomsim wavwriter)
