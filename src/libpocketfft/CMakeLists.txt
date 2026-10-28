# PocketFFT (C version) by Martin Reinecke, BSD-3-Clause, see LICENSE.md.
# Vendored unmodified from https://github.com/mreineck/pocketfft (master branch).
add_library(libpocketfft STATIC pocketfft.c pocketfft.h)
include_directories("${CMAKE_CURRENT_SOURCE_DIR}")
if(NOT MSVC)
	target_link_libraries(libpocketfft m)
endif()
