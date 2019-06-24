add_library(wavwriter STATIC "${CMAKE_CURRENT_SOURCE_DIR}/source/wavwriter.c")
	
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include")

if(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
	set(MATH "")
elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
	set(MATH "m")
elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
	set(MATH "")
endif()

target_link_libraries(wavwriter
	"${MATH}"
	)