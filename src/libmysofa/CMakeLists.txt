# libmysofa 1.3.5, BSD-3-Clause (see LICENSE and README.md in this directory).
find_package(ZLIB REQUIRED)

add_library(libmysofa STATIC
	hdf/btree.c
	hdf/dataobject.c
	hdf/fractalhead.c
	hdf/gcol.c
	hdf/gunzip.c
	hdf/superblock.c
	hrtf/cache.c
	hrtf/check.c
	hrtf/easy.c
	hrtf/interpolate.c
	hrtf/kdtree.c
	hrtf/lookup.c
	hrtf/loudness.c
	hrtf/minphase.c
	hrtf/neighbors.c
	hrtf/reader.c
	hrtf/resample.c
	hrtf/spherical.c
	hrtf/tools.c
	resampler/speex_resampler.c
	)
set_target_properties(libmysofa PROPERTIES C_STANDARD 99 OUTPUT_NAME mysofa)
target_include_directories(libmysofa
	PUBLIC  "${CMAKE_CURRENT_SOURCE_DIR}/hrtf"
	PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries(libmysofa PUBLIC ZLIB::ZLIB)
if(NOT MSVC)
	target_link_libraries(libmysofa PUBLIC m)
	# third-party code: do not report its warnings
	target_compile_options(libmysofa PRIVATE -w)
endif()
