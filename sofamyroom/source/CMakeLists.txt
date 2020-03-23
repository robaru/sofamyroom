cmake_minimum_required(VERSION 3.10.2 FATAL_ERROR)

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

set(CMAKE_C_FLAGS_UNITTEST "${CMAKE_C_FLAGS_DEBUG} -DUNITTEST")

if(MSVC)
	string(REPLACE "/MDd" "/MTd" CMAKE_C_FLAGS_DEBUG ${CMAKE_C_FLAGS_DEBUG})
	string(REPLACE "/MD" "/MT" CMAKE_C_FLAGS_RELEASE ${CMAKE_C_FLAGS_RELEASE})
	string(REPLACE "/MDd" "/MTd" CMAKE_C_FLAGS_UNITTEST ${CMAKE_C_FLAGS_UNITTEST})
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
