cmake_minimum_required(VERSION 3.13.2 FATAL_ERROR)

project(roomsim C)

if(MSVC)
	if("${CMAKE_SIZEOF_VOID_P}" STREQUAL "8")
		set(PLATFORM "x64")
	elseif("${CMAKE_SIZEOF_VOID_P}" STREQUAL "4")
    	set(PLATFORM "Win32")
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
                   	"${CMAKE_SOURCE_DIR}/libfftw/${PLATFORM}/bin/libfftw3-3.dll" $<TARGET_FILE_DIR:roomsim>)
endif()

include_directories(
	"${CMAKE_SOURCE_DIR}/libroomsim/include"
	"${CMAKE_SOURCE_DIR}/libmysofa/${PLATFORM}/include")

target_link_libraries(roomsim libroomsim)