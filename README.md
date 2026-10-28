# sofamyroom

This project proposes a fast, accurate, and flexible "shoebox" room acoustics simulator that supports both specular and diffuse reflections.
The simulator extends the work released by Schimmel et al., called Roomsim, by adding the rendering of Binaural Room Impulse Responses (BRIR) using arbitrary Head Related Transfer Functions (HRTFs) stored with the AES Spatially Oriented Format for Acoustics (SOFA) file format. Moreover, SofaMyRoom can be launched directly from your console of choice.

The SOFA support is possible thanks to MySofa library (hereinafter referred as `libmysofa`) and it can export the results of the simulation to a WAVE file.

Thanks to the Virtual Acoustic Space Traveling (VAST) framework [1], we added the possibility to systematically generate labeled datasets to train and test machine learning algorithms.

:open_book: The full documentation is available [here](https://robaru.github.io/sofamyroom)

:open_book: We are writing an article about sofamyroom and the pre-print is already available on [arXiv](https://arxiv.org/abs/2106.12992).


> **Note (October 2026):** the git history was rewritten to merge the old repositories into a single line. If you cloned sofamyroom before, please re-clone it; existing clones will not pull cleanly.

## Installing sofamyroom

sofamyroom is built from source with CMake (see [Building](#building-sofamyroom)). The third-party code it needs (libmysofa, PocketFFT, SFMT) is included in the repository and compiled together with it, so the resulting program is self-contained. The only system library is zlib, which macOS, the Linux distributions and MATLAB already provide; on Ubuntu Linux the development package is needed to build (`sudo apt-get install zlib1g-dev`), on Windows see [Building on Windows](#building-on-windows). Only 64-bit platforms are supported.

## Usage

sofamyroom reads a text file with the setup parameters and writes one WAVE file per receiver in the current directory. From the repository root, after [building](#building-sofamyroom) and downloading the KEMAR HRTF set (see [Testing](#testing)):

```bash
build/cmake/sofamyroom data/sampleroomsetup.txt
```

`data/sampleroomsetup.txt` is a commented sample of the setup file; the paths in it (SOFA file, output name) are relative to the directory you run the command from.

### Usage with MATLAB

The MEX-file takes the same parameters as a MATLAB structure. `readsetup` evaluates a setup file written as a MATLAB script (`data/sampleroomsetup.m`, same content as the `.txt` sample):

```matlab
addpath matlab
setup = readsetup('data/sampleroomsetup.m');
output = sofamyroom(setup);
```

## Building SofaMyRoom

These instructions will guide you through the steps to build SofaMyRoom on your local machine. CMake is required.

### Building on Windows

You can use CMake to generate a Visual Studio solution that can be used to build sofamyroom. Open a new Command Prompt window and type the following commands:

```bash
> cd build
> cmake ../src -G"Visual Studio 16 2019" -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake
```

zlib must be available to the compiler; the command above takes it from [vcpkg](https://vcpkg.io) (`vcpkg install zlib`). To know how to generate solutions for other versions of Visual Studio, type `cmake --help`.

### Building on Ubuntu Linux

You can use CMake to generate a Makefile for sofamyroom. Open a new Terminal window and type the following commands:

```bash
cd build
cmake ../src -G"Unix Makefiles" -DCMAKE_BUILD_TYPE=BUILD_TYPE
make
```

`CMAKE_BUILD_TYPE` can be `Debug`, `Release` or `Unittest`. If CMAKE_BUILD_TYPE is not set, the default build type is `Release`. Before building the project, install the zlib headers:

```bash
sudo apt-get install zlib1g-dev
```

### Building on MacOS

You can use CMake to generate a XCode project. Open a new Terminal window and type the following commands:

```applescript
cd sofamyroom/build
cmake ../src -G"XCode"
```

### Building with MATLAB

You can use MATLAB to generate a MEX-file. Type the following command in the Command Window:

```matlab
cd src
make
```

You can build the MEX-file in Debug mode. Type the following command:

```matlab
make debug
```

You can also build it in Unittest mode, to check the validity of sofamyroom functions. Type the following command:

```matlab
make test
```

On Windows, pass the location of zlib as extra arguments, for example `make('-IC:\vcpkg\installed\x64-windows\include', '-LC:\vcpkg\installed\x64-windows\lib', '-lzlib')`.

## Known issues and limitations

* When using the MEX library, MATLAB can crash since the memory allocation performed by libmysofa is not persistent.
* The library is single-threaded and keeps the loaded sensors in global state: run one simulation at a time per process. To simulate many rooms in parallel, start several processes.
* Image sources are placed at integer sample delays (no fractional-delay interpolation).
* SOFA receivers must use the SimpleFreeFieldHRIR convention with exactly two receivers (ears).
* The SOFA `Data.Delay` is ignored unless the receiver description contains `delay=1`, in which case it is applied as a whole-sample shift per ear.
* The diffuse tail is rendered from six spatial directions.

## Testing

The unit tests need the MIT KEMAR HRTF set in `data/`:

```bash
curl -L -o data/MIT_KEMAR_normal_pinna.sofa http://sofacoustics.org/data/database/mit/mit_kemar_normal_pinna.sofa
```

Then build with CMake and run `ctest`:

```bash
cmake -S src -B build/cmake -DCMAKE_BUILD_TYPE=Release
cmake --build build/cmake
ctest --test-dir build/cmake --output-on-failure
```

## License

The code written for sofamyroom is licensed under the [EUPL-1.2](https://joinup.ec.europa.eu/sites/default/files/custom-page/attachment/2020-03/EUPL-1.2%20EN.txt) (`LICENSE.txt`).
The files inherited from Roomsim by Steven M. Schimmel (University of Zurich, 2009) keep their original GNU GPL-3.0-or-later licence, and our modifications to them are released under the same GPL terms. Each of these files carries the original Roomsim header followed by a notice that states whether, and how, it was changed for sofamyroom:

* `src/libroomsim/` (all `.c` and `.h` files, except `include/global.h` which is new and EUPL-1.2)
* `src/libroomsim.h`, `src/main.c`, `src/mexmain.c`, `src/unittest.c`
* `data/materials.mat` (the Roomsim absorption-coefficient database used by `editabsorption.m`)
* `matlab/` `estimateRT60.m`, `editabsorption.m`, `plotabsorption.m`, `estimateroombreakfreq.m`, `plotcoordsystem.m`, `plotbrir.m`, `readsetup.m`, `rsound.m`, `yprTr2s.m`, `yprTs2r.m`, `selectabsorption.m`, `plotroom.m`, `setcomplexity.m`

Since the simulator core is GPL, every compiled form of sofamyroom (command line tool, MEX file, or a package that links it) is a combined work distributed under the GNU GPL-3.0-or-later; the EUPL-1.2 permits this through its compatibility clause.

The third-party code included in `src/` (libmysofa, PocketFFT, SFMT and the WAVE writer) is listed with its licence texts in [THIRD_PARTY_LICENSES.md](THIRD_PARTY_LICENSES.md).

## Credits

* Steven M. Schimmel, Martin F. Muller, and Norbert Dillier - [Roomsim](https://sourceforge.net/projects/roomsim/)
* Christian Hoene, Isabel C. Patiño Mejía, Alexandru Cacerovschi - [MySofa](https://github.com/hoene/libmysofa)
* Martin Reinecke - [PocketFFT](https://github.com/mreineck/pocketfft)
* Mutsuo Saito and Makoto Matsumoto - [SFMT](http://www.math.sci.hiroshima-u.ac.jp/m-mat/MT/SFMT/)
* Piotr Majdak et al. - [Spatially Oriented Format for Acoustics (SOFA)](https://www.sofaconventions.org/)
* Andrew Ippoliti - [WAV file writer](http://blog.acipo.com/generating-wave-files-in-c/)
* Antoine Deleforge and Clément Gaultier [VAST](http://thevastproject.inria.fr/dataset/)

## References

[1] C. Gaultier, S. Kataria, and A. Deleforge. "VAST: The virtual acoustic space traveler dataset." International Conference on Latent Variable Analysis and Signal Separation. Springer, Cham, 2017.
