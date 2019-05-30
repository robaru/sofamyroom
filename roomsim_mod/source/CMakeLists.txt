cmake_minimum_required(VERSION 3.10.2 FATAL_ERROR)

project(roomsim C)

if (NOT CMAKE_BUILD_TYPE)
	message("No build type selected, default to Release")
	set(CMAKE_BUILD_TYPE "Release")
endif()

if(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
	set(OS "Windows")
	if("${CMAKE_SIZEOF_VOID_P}" STREQUAL "8")
		set(PLATFORM "x64")
	elseif("${CMAKE_SIZEOF_VOID_P}" STREQUAL "4")
    	set(PLATFORM "Win32")
	endif()
elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
	set(OS "Linux")
	if("${CMAKE_SIZEOF_VOID_P}" STREQUAL "8")
		set(PLATFORM "64bit")
	elseif("${CMAKE_SIZEOF_VOID_P}" STREQUAL "4")
    	set(PLATFORM "32bit")
	endif()
elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
	set(OS "MacOS")
	if("${CMAKE_SIZEOF_VOID_P}" STREQUAL "8")
		set(PLATFORM "64bit")
	elseif("${CMAKE_SIZEOF_VOID_P}" STREQUAL "4")
    	set(PLATFORM "32bit")
	endif()
endif()

if(MSVC)
	string(REPLACE "/MDd" "/MTd" CMAKE_C_FLAGS_DEBUG ${CMAKE_C_FLAGS_DEBUG})
	string(REPLACE "/MD" "/MT" CMAKE_C_FLAGS_RELEASE ${CMAKE_C_FLAGS_RELEASE})
endif()

add_subdirectory(libsfmt)
add_subdirectory(libroomsim)

add_executable(roomsim
	build.c
	main.c
	unittest.c
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
	"${CMAKE_SOURCE_DIR}/libmysofa/${OS}/${PLATFORM}/include")

target_link_libraries(roomsim libroomsim)
