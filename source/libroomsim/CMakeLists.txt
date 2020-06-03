add_library(libroomsim STATIC
	"${CMAKE_CURRENT_SOURCE_DIR}/source/3D.c"
	"${CMAKE_CURRENT_SOURCE_DIR}/source/dsp.c"
	"${CMAKE_CURRENT_SOURCE_DIR}/source/interface.c"
	"${CMAKE_CURRENT_SOURCE_DIR}/source/interp.c"
	"${CMAKE_CURRENT_SOURCE_DIR}/source/rng.c"
	"${CMAKE_CURRENT_SOURCE_DIR}/source/roomsim.c"
	"${CMAKE_CURRENT_SOURCE_DIR}/source/sensor.c"
	"${CMAKE_CURRENT_SOURCE_DIR}/source/setup.c"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/3D.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/defs.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/dsp.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/global.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/interface.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/interp.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/mem.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/msg.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/mstruct.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/rng.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/sensor.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/setup.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/types.h"
	)
	
include_directories(
	"${CMAKE_CURRENT_SOURCE_DIR}/include"
	"${CMAKE_SOURCE_DIR}/libfftw/${OS}/${PLATFORM}/include"
	"${CMAKE_SOURCE_DIR}/libsfmt/"
	"${CMAKE_SOURCE_DIR}/libmysofa/${OS}/${PLATFORM}/include"
	)

set(CMAKE_DEBUG_POSTFIX "d")

set(MYSOFA "${CMAKE_SOURCE_DIR}/libmysofa/${OS}/${PLATFORM}/lib/$<IF:$<OR:$<CONFIG:Debug>,$<CONFIG:Release>>,$<IF:$<CONFIG:Release>,Release,Debug>,Debug>/libmysofa$<$<OR:$<CONFIG:Debug>,$<CONFIG:Unittest>>:${CMAKE_DEBUG_POSTFIX}>${CMAKE_STATIC_LIBRARY_SUFFIX}")

if(CMAKE_SYSTEM_NAME MATCHES Windows)
	set(FFTW "${CMAKE_SOURCE_DIR}/libfftw/${OS}/${PLATFORM}/lib/libfftw3-3.lib")
	set(MATH "")
	set(LIBZ "")
elseif(CMAKE_SYSTEM_NAME MATCHES Linux)
	set(FFTW "-lfftw3")
	set(MATH "m")
	set(LIBZ "z")
elseif(CMAKE_SYSTEM_NAME MATCHES Darwin)
	set(FFTW "${CMAKE_SOURCE_DIR}/libfftw/${OS}/${PLATFORM}/lib/$<IF:$<OR:$<CONFIG:Debug>,$<CONFIG:Release>>,$<IF:$<CONFIG:Release>,Release,Debug>,Debug>/libfftw3$<$<OR:$<CONFIG:Debug>,$<CONFIG:Unittest>>:${CMAKE_DEBUG_POSTFIX}>${CMAKE_STATIC_LIBRARY_SUFFIX}")
	set(MATH "")
	set(LIBZ "z")
endif()

target_link_libraries(libroomsim
	"${MATH}"
	libsfmt
	"${FFTW}"
	"${MYSOFA}"
	"${LIBZ}"
	)
