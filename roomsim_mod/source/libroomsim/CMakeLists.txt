add_library(libroomsim STATIC
	"${CMAKE_CURRENT_SOURCE_DIR}/source/3D.c"
	"${CMAKE_CURRENT_SOURCE_DIR}/source/dsp.c"
	"${CMAKE_CURRENT_SOURCE_DIR}/source/interface.c"
	"${CMAKE_CURRENT_SOURCE_DIR}/source/interp.c"
	"${CMAKE_CURRENT_SOURCE_DIR}/source/rng.c"
	"${CMAKE_CURRENT_SOURCE_DIR}/source/roomsim.c"
	"${CMAKE_CURRENT_SOURCE_DIR}/source/sensor.c"
	"${CMAKE_CURRENT_SOURCE_DIR}/source/setup.c"
	)
	
include_directories(
	"${CMAKE_CURRENT_SOURCE_DIR}/include"
	"${CMAKE_SOURCE_DIR}/libfftw/${PLATFORM}/include"
	"${CMAKE_SOURCE_DIR}/libsfmt/"
	"${CMAKE_SOURCE_DIR}/libmysofa/${PLATFORM}/include"
	)

target_link_libraries(libroomsim
	libsfmt
	"${CMAKE_SOURCE_DIR}/libfftw/${PLATFORM}/lib/libfftw3-3.lib"
	"${CMAKE_SOURCE_DIR}/libmysofa/${PLATFORM}/lib/${CMAKE_BUILD_TYPE}/mysofa*.lib"
	)