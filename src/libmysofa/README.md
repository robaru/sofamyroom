# libmysofa (vendored)

Source code of [libmysofa](https://github.com/hoene/libmysofa) 1.3.5 by
Christian Hoene (Symonics GmbH), BSD-3-Clause, see `LICENSE`.

The `hdf/`, `hrtf/` and `resampler/` directories are copied unmodified from
the `src/` directory of the upstream release tarball. `CMakeLists.txt`,
`config.h` and `hrtf/mysofa_export.h` are written for the sofamyroom build
and replace the files that libmysofa's own CMake build would generate.
libmysofa needs zlib, which is taken from the system.
