set(LIBROOMSIM_SOURCES
	"${CMAKE_CURRENT_SOURCE_DIR}/source/3D.c"
	"${CMAKE_CURRENT_SOURCE_DIR}/source/dsp.c"
	"${CMAKE_CURRENT_SOURCE_DIR}/source/interface.c"
	"${CMAKE_CURRENT_SOURCE_DIR}/source/interp.c"
	"${CMAKE_CURRENT_SOURCE_DIR}/source/mem.c"
	"${CMAKE_CURRENT_SOURCE_DIR}/source/rng.c"
	"${CMAKE_CURRENT_SOURCE_DIR}/source/roomsim.c"
	"${CMAKE_CURRENT_SOURCE_DIR}/source/sensor.c"
	"${CMAKE_CURRENT_SOURCE_DIR}/source/setup.c"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/3D.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/capi.h"
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

add_library(libroomsim STATIC ${LIBROOMSIM_SOURCES})

include_directories(
	"${CMAKE_CURRENT_SOURCE_DIR}/include"
	"${CMAKE_SOURCE_DIR}/libpocketfft/"
	"${CMAKE_SOURCE_DIR}/libsfmt/"
	)

set(CMAKE_DEBUG_POSTFIX "d")

if(CMAKE_SYSTEM_NAME MATCHES Linux)
	set(MATH "m")
else()
	set(MATH "")
endif()

target_link_libraries(libroomsim
	"${MATH}"
	libsfmt
	libpocketfft
	libmysofa
	)

# Same sources compiled for embedding in a host process (pysofamyroom):
# errors unwind to the caller instead of calling exit(), see source/capi.c.
if(SOFAMYROOM_PYTHON)
	add_library(libroomsim_embed STATIC ${LIBROOMSIM_SOURCES})
	target_compile_definitions(libroomsim_embed PRIVATE SMR_EMBED)
	set_target_properties(libroomsim_embed PROPERTIES C_VISIBILITY_PRESET hidden)
	target_link_libraries(libroomsim_embed
		"${MATH}"
		libsfmt
		libpocketfft
		libmysofa
		)
endif()
