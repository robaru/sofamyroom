# SofaMyRoom

:pencil2: Repository under construction! In a week or two everything should be ready.


SofaMyRoom is a fast, accurate, and flexible "shoebox" room acoustics simulator that supports both specular and diffuse reflections.
The simulator extends the work released by Schimmel et al., called Roomsim, by adding the rendering of Binaural Room Impulse Responses (BRIR) using arbitrary Head Related Transfer Functions (HRTFs) stored with the AES Spatially Oriented Format for Acoustics (SOFA) file format. Moreover, SofaMyRoom can be launched directly from your console of choice.

The SOFA support is possible thanks to MySofa library (hereinafter referred as `libmysofa`) and it can export the results of the simulation to a WAVE file.

Thanks to the Virtual Acoustic Space Traveling (VAST) framework [1], we added the possibility to systematically generate labeled datasets to train and test machine learning algorithms.

:open_book: The full documentation is available here: spatialhearing.github.io/sofamyroom

:mailbox_with_mail: For any question or request of help please write to: roberto(at)robaru(dot)com

## Installing SofaMyRoom

This repository includes the binary files and, if in case of need, the CMake scripts allow a customized compilation. Only 64-bit platforms are supported.
SofaMyRoom binaries are available for Windows 10, Ubuntu Linux 20.04 and MacOS Catalina.

### Installation on Windows

No futher steps are required to run SofaMyRoom on Windows.

### Installation on Ubuntu Linux

`libmysofa` requires the `zlib` library in order to work properly. To install it, open a terminal and enter the following command:

```bash
sudo apt-get install zlib1g
```

### Installation on macOS

No further steps are required to run SofaMyRoom on macOS.

## Usage

To use SofaMyRoom, type this command in a Command Prompt or a Terminal:

```bash
./sofamyroom setup.txt
```

`setup.txt` is the name of the text file containing all the SofaMyRoom setup parameters structure.
A sample of it can be found in `sampleroomsetup.txt`.

### Usage with MATLAB

A MEX-file for 64-bit MATLAB is available. To run it, type these commands in the Command Window:

```matlab
setup = readsetup('sampleroomsetup.txt');
output = sofamyroom(setup);
```

## Building SofaMyRoom

These instructions will guide you through the steps to build SofaMyRoom on your local machine. CMake is required.

### Building on Windows

You can use CMake to generate a Visual Studio solution that can be used to build SofaMyRoom. Open a new Command Prompt window and type the following commands:

```bash
> cd build
> cmake ../src -G"Visual Studio 16 2019"
```

To know how to generate solutions for other versions of Visual Studio, type `cmake --help`.

### Building on Ubuntu Linux

You can use CMake to generate a Makefile for SofaMyRoom. Open a new Terminal window and type the following commands:

```bash
cd build
cmake ../src -G"Unix Makefiles" -DCMAKE_BUILD_TYPE=BUILD_TYPE
make
```

`CMAKE_BUILD_TYPE` can be `Debug`, `Release` or `Unittest`. If CMAKE_BUILD_TYPE is not set, the default build type is `Release`. Before building the project, the following libraries may be needed:

```bash
sudo apt-get install libfftw3-dev
sudo apt-get install zlib1g-dev
sudo apt-get install libcunit1-dev
```

### Building on MacOS

You can use CMake to generate a XCode project. Open a new Terminal window and type the following commands:

```applescript
cd sofamyroom/build
cmake ../src -G"XCode"
```

The `cunit` library may be required. You can install it with a package manager for MacOS, i.e. Homebrew.

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

You can also build it in Unittest mode, to check the validity of SofaMyRoom functions. Type the following command:

```matlab
make test
```

## Known issues

* When using the MEX library, MATLAB can crash since the memory allocation performed by libmysofa is not persistent.

## License

SofaMyRoom is licensed under the [EUPL-1.2](https://joinup.ec.europa.eu/sites/default/files/custom-page/attachment/2020-03/EUPL-1.2%20EN.txt).

## Credits

* Steven M. Schimmel, Martin F. Muller, and Norbert Dillier - [Roomsim](https://sourceforge.net/projects/roomsim/)
* Christian Hoene, Isabel C. Patiño Mejía, Alexandru Cacerovschi - [MySofa](https://github.com/hoene/libmysofa)
* Piotr Majdak et al. - [Spatially Oriented Format for Acoustics (SOFA)](https://www.sofaconventions.org/)
* Andrew Ippoliti - [WAV file writer](http://blog.acipo.com/generating-wave-files-in-c/)
* Antoine Deleforge and Clément Gaultier [VAST](http://thevastproject.inria.fr/dataset/)

## References

[1] C. Gaultier, S. Kataria, and A. Deleforge. "VAST: The virtual acoustic space traveler dataset." International Conference on Latent Variable Analysis and Signal Separation. Springer, Cham, 2017.
