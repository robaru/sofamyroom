
# SofaMyRoom

SofaMyRoom is a fast, accurate, and flexible "shoebox" room acoustics simulator that supports both specular and diffuse reflections.
The simulator extends the work released by Schimmel et al., called Roomsim, by adding the rendering of Binaural Room Impulse Response, BRIR.
It supports the AES Spatially Oriented Format for Acoustics (SOFA) file format for storing HRTFs thanks to MySofa library (hereinafter referred as `libmysofa`) and it can export the results of the simulation to a WAVE file.

Thanks to the virtual acoustic space traveling (VAST), it is possible to systematically generate labeled datasets to train and test machine learning algorithms.

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

### Installation on MacOS

No further steps are required to run SofaMyRoom on MacOS.

# Usage

To use SofaMyRoom, type this command in a Command Prompt or a Terminal:

```bash
./sofamyroom setup.txt
```

`setup.txt` is the name of the text file containing all the SofaMyRoom setup parameters structure.
A sample of it can be found in `sampleroomsetup.txt`.

## Usage with MATLAB

A MEX-file for 64-bit MATLAB is available. To run it, type these commands in the Command Window:

```matlab
setup = readsetup('sampleroomsetup.txt');
output = sofamyroom(setup);
```

## Notes about the setup file

This file stores all the information needed by SofaMyRoom to perform the simulation.

### Notes about the options

Besides those related to the simulation, there are two options related to the output file:

```matlab
options.outputname = 'output';
options.mex_saveaswav  =  true; % only for MATLAB MEX
```

`options.outputname` is the first part of the name given to the output file that stores the result of the simulation.
The name must not contain the extension of the file, because SofaMyRoom takes care of that.
`options.mex_saveaswav` tells SofaMyRoom to export the results to a Windows WAVE (.wav) file  (`options.mex_saveaswav = true;`) or to return a MATALB numerical array.
Note that there is one WAVE file for every source-receiver couple.

### Notes about the receiver

The format of the field `receiver(<i>).description` is the following:

```
'RECEIVER_ID PATH_TO_HRTF_FILE interp=interp_value norm=norm_value resampling=resampling_value'
```

`RECEIVER_ID` must be `SOFA` in order to read a `.sofa` file. `PATH_TO_HRTF_FILE` is the relative or absolute path to your `.sofa` file. The other values are:

* `interp`: SofaMyRoom can look up the HRTF that is closest to the given coordinates or it can interpolate the neighboring HRTFs to obtain the desired HRTF. `interp_value` can be T[RUE], t[rue], 1 (interpolation is 
active), or F[ALSE], f[alse], 0 (interpolation is not performed). Words in square brackets are optional, SofaMyRoom just looks for the first character.
* `norm`: SofaMyRoom can normalize HRTF data. `norm_value` behaves just like `interp_value`, described above.
* `resampling`: SofaMyRoom can resample the HRTF data according to the sampling frequency defined in `options.fs`. `resampling_value` behaves just like `interp_value` and `norm_value`, described above.

All the options described above are optional. If not set, interpolation, normalization and resampling are not performed. Unrecognized options are skipped. If options are repeated, only the last one is used.

### Examples

All the examples proposed below are valid:

```
'SOFA ./mySofaFile.sofa'
'SOFA ./mySofaFile.sofa interp=TRUE norm=0 resampling=t'
'SOFA ./mySofaFile.sofa interp=1 norm=f'
'SOFA ./mySofaFile.sofa resampling=0 norm=F interp=0'
```

# Building SofaMyRoom

These instructions will guide you through the steps to build SofaMyRoom on your local machine. CMake is required.

## Building on Windows

You can use CMake to generate a Visual Studio solution that can be used to build SofaMyRoom. Open a new Command Prompt window and type the following commands:

```bash
> cd src\build
> cmake .. -G"Visual Studio 16 2019"
```

To know how to generate solutions for other versions of Visual Studio, type `cmake --help`.

## Building on Ubuntu Linux

You can use CMake to generate a Makefile for SofaMyRoom. Open a new Terminal window and type the following commands:

```bash
cd src/build
cmake .. -G"Unix Makefiles" -DCMAKE_BUILD_TYPE=BUILD_TYPE
make
```

`CMAKE_BUILD_TYPE` can be `Debug`, `Release` or `Unittest`. If CMAKE_BUILD_TYPE is not set, the default build type is `Release`. Before building the project, the following libraries may be needed:

```bash
sudo apt-get install libfftw3-dev
sudo apt-get install zlib1g-dev
sudo apt-get install libcunit1-dev
```

## Building on MacOS

You can use CMake to generate a XCode project. Open a new Terminal window and type the following commands:

```applescript
cd sofamyroom/source/build
cmake .. -G"XCode"
```

The `cunit` library may be required. You can install it with a package manager for MacOS, i.e. Homebrew.

## Building with MATLAB

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

## Building the documentation

You can optionally build the documentation files from the source code. Documentation files are built using [Doxygen](https://www.doxygen.nl/index.html), [Sphinx](https://www.sphinx-doc.org/en/stable/) and [Breathe](https://github.com/michaeljones/breathe).
First of all, you need to install all of the required software. Please refer to the installation guides of each software in order to know the steps to install them on your machine. For MS Windows, we recommend to use the package manager `pip3` to install Sphinx and Breathe ([ref](https://www.sphinx-doc.org/en/master/usage/installation.html)) while with Ubuntu is suggested to run:

```bash
# IMPORTANT: recommonmark version 0.5.0 or later
sudo apt install doxygen
pip3 install sphinx sphinx_rtd_theme recommonmark breathe 
```

When the installation is complete, you need to enable the building of the documentation files. When using CMake, you need to add the following to the CMake command:

```bash
cd src/build
cmake .. -DBUILD_DOCS=True
```

The documentation will be saved into the `docs` folder.

For example, if you are using CMake to generate a Visual Studio solution, the complete command is:

```powershell
cmake .. -G"Visual Studio 16 2019" -DBUILD_DOCS=True
```

If the option is not specified, documentation generation is skipped when building SofaMyRoom. Please note that if you don't need to build the documentation files, you don't need to install the aforementioned software.

A useful tutorial on how to use Doxygen in combination with Sphinx and CMAKE is available [here](https://devblogs.microsoft.com/cppblog/clear-functional-c-documentation-with-sphinx-breathe-doxygen-cmake/).

# Known issues

* When using the MEX library, MATLAB can crash since the memory allocation performed by llibmysofa is not persistent.

# License

SofaMyRoom is licensed under the [EUPL-1.2](https://joinup.ec.europa.eu/sites/default/files/custom-page/attachment/2020-03/EUPL-1.2%20EN.txt).

# Credits

* Steven M. Schimmel, Martin F. Muller, and Norbert Dillier - [Roomsim](https://sourceforge.net/projects/roomsim/)
* Christian Hoene, Isabel C. Patiño Mejía, Alexandru Cacerovschi - [MySofa](https://github.com/hoene/libmysofa)
* Piotr Majdak et al. - [Spatially Oriented Format for Acoustics (SOFA)](https://www.sofaconventions.org/)
* Andrew Ippoliti - [WAV file writer](http://blog.acipo.com/generating-wave-files-in-c/)
* Antoine Deleforge and Clément Gaultier [VAST](http://thevastproject.inria.fr/dataset/)