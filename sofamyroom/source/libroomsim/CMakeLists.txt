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
	"${CMAKE_SOURCE_DIR}/libfftw/${OS}/${PLATFORM}/include"
	"${CMAKE_SOURCE_DIR}/libsfmt/"
	"${CMAKE_SOURCE_DIR}/libmysofa/${OS}/${PLATFORM}/include"
	)

if(CMAKE_SYSTEM_NAME MATCHES Windows)
	set(FFTW_NAME "${CMAKE_SOURCE_DIR}/libfftw/${OS}/${PLATFORM}/lib/libfftw3-3.lib")
	set(MYSOFA_NAME "mysofa*.lib")
	set(MATH "")
	set(LIBZ "")
elseif(CMAKE_SYSTEM_NAME MATCHES Linux)
	set(FFTW_NAME "-lfftw3")
	if(LIBFOLDER MATCHES Release)
		set(MYSOFA_NAME "libmysofa.a")
	elseif(LIBFOLDER MATCHES Debug)
		set(MYSOFA_NAME "libmysofad.a")
	endif()
	set(MATH "m")
	set(LIBZ "z")
elseif(CMAKE_SYSTEM_NAME MATCHES Darwin)
	if(LIBFOLDER MATCHES Release)
		set(FFTW_NAME "${CMAKE_SOURCE_DIR}/libfftw/${OS}/${PLATFORM}/lib/${LIBFOLDER}/libfftw3.a")
		set(MYSOFA_NAME "libmysofa.a")
	elseif(LIBFOLDER MATCHES Debug)
    		set(FFTW_NAME "${CMAKE_SOURCE_DIR}/libfftw/${OS}/${PLATFORM}/lib/${LIBFOLDER}/libfftw3d.a")
		set(MYSOFA_NAME "libmysofad.a")
	endif()
	set(MATH "")
	set(LIBZ "z")
endif()

target_link_libraries(libroomsim
	"${MATH}"
	libsfmt
	"${FFTW_NAME}"
	"${CMAKE_SOURCE_DIR}/libmysofa/${OS}/${PLATFORM}/lib/${LIBFOLDER}/${MYSOFA_NAME}"
	"${LIBZ}"
	)
