add_library(libsfmt STATIC SFMT.c SFMT.h)
target_compile_definitions(libsfmt PUBLIC SFMT_MEXP=19937)
include_directories("${CMAKE_CURRENT_SOURCE_DIR}")
