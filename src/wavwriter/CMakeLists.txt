add_library(wavwriter STATIC 
			"${CMAKE_CURRENT_SOURCE_DIR}/source/wavwriter.c"
			"${CMAKE_CURRENT_SOURCE_DIR}/include/wavwriter.h"
			)
	
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include")

if(CMAKE_SYSTEM_NAME MATCHES Windows)
	set(MATH "")
elseif(CMAKE_SYSTEM_NAME MATCHES Linux)
	set(MATH "m")
elseif(CMAKE_SYSTEM_NAME MATCHES Darwin)
	set(MATH "")
endif()

target_link_libraries(wavwriter
	"${MATH}"
	)
